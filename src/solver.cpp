#include <ctime>
#include <cmath>
#include <cassert>

#include <algorithm>
#include <fstream>
#include <queue>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "group.h"
#include "section.h"
#include "solver.h"
#include "schedule.h"

#include <iostream>

Solver::Solver(Schedule* schedule, int seed)
    : schedule_(schedule), rand_generator_(seed) {}

bool Solver::InitialSchedule() {
  for (const auto &ptr : schedule_->GetGroups()) {
    for (const auto &it : ptr->GetSections()) {
      std::vector< std::pair<int, int> > breaks;
      int break_slots = 0;

      std::vector< std::vector< std::pair<int, int> > > to_assign(
        schedule_->GetNumDays());
      std::priority_queue< std::pair<int, int>,
                           std::vector< std::pair<int, int> >,
                           std::greater< std::pair<int, int> > > day_slots;

      for (int i = 0; i < schedule_->GetNumDays(); i++) day_slots.emplace(0, i);

      for (int k = 1; k <= schedule_->GetNumDays(); k++) {
        for (const auto &jt : ptr->GetSubjects()) {
          if (int(jt->GetSlots().size()) != k) continue;

          if (jt->GetTeacher() == 0) {
            int num_slots = jt->GetSlots()[0];
            breaks.emplace_back(jt->GetId(), num_slots);
            break_slots += num_slots;
            continue;
          }

          std::vector< std::pair<int, int> > days;

          for (int j = 0; j < k; j++) {
            int old_slots = day_slots.top().first;
            int day = day_slots.top().second;
            day_slots.pop();

            int num_slots = jt->GetSlots()[j];
            to_assign[day].emplace_back(jt->GetId(), num_slots);
            days.emplace_back(old_slots + num_slots, day);
          }

          for (int j = 0; j < k; j++) day_slots.push(days[j]);
        }
      }

      for (const auto &jt : ptr->GetSubjects()) {
        if (int(jt->GetSlots().size()) <= schedule_->GetNumDays()) continue;
        for (const auto &num_slots : jt->GetSlots()) {
          int old_slots = day_slots.top().first;
          int day = day_slots.top().second;
          day_slots.pop();

          to_assign[day].emplace_back(jt->GetId(), num_slots);
          day_slots.emplace(old_slots + num_slots, day);
        }
      }

      while (!day_slots.empty()) {
        if (day_slots.top().first + break_slots
          > schedule_->GetNumSlotsPerDay()) return false;
        day_slots.pop();
      }

      for (int i = 0; i < schedule_->GetNumDays(); i++) {
        int last_slot = i*schedule_->GetNumSlotsPerDay();
        std::shuffle(to_assign[i].begin(), to_assign[i].end(), rand_generator_);

        int d = (to_assign[i].size() + breaks.size() + 1) / (breaks.size() + 1);
        auto kt = to_assign[i].begin() + d - 1;
        for (auto jt : breaks) kt = to_assign[i].insert(kt, jt) + d;

        for (auto sb : to_assign[i]) {
          int section = it->GetId();
          int subject = sb.first;
          int num_slots = sb.second;

          schedule_->HardAssign(subject, section, last_slot, num_slots);
          last_slot += num_slots;
        }
      }
    }
  }
  return true;
}

int Solver::HardSolver(int time_limit) {
  int hard_count = schedule_->HardCount();
  std::time_t start = std::time(NULL);
  while (hard_count > 0 && std::difftime(std::time(NULL), start) < time_limit) {
    int result = HardLocalSearch();
    int delta;
    do {
      delta = HardTabuSearch();

      result += delta;
    } while (delta > 0);
    hard_count += result;
  }
  return hard_count;
}

int Solver::HardLocalSearch() {
  int result = 0;

  auto translate = [this, &result]
      (int section, int timeslot, int open_timeslot, int delta)
      -> std::pair<int, int> {
    if (delta <= 0) {
      schedule_->HardTranslate(section, timeslot, open_timeslot);
      result += delta;
      return std::make_pair(2, 0);
    }
    return std::make_pair(0, 0);
  };

  auto swap = [this, &result]
      (int section, int lhs_timeslot, int rhs_timeslot, int delta)
      -> std::pair<int, int> {
    if (delta <= 0) {
      schedule_->HardSwap(section, lhs_timeslot, rhs_timeslot);
      result += delta;
      return std::make_pair(2, 0);
    }
    return std::make_pair(0, 0);
  };

  auto adjswap = [this, &result]
      (int section, int lhs_timeslot, int rhs_timeslot, int delta)
      -> std::pair<int, int> {
    if (delta <= 0) {
      schedule_->HardAdjSwap(section, lhs_timeslot, rhs_timeslot);
      result += delta;
      return std::make_pair(2, 0);
    }
    return std::make_pair(0, 0);
  };

  HardSearchTemplate(translate, swap, adjswap);
  return result;
}

int Solver::HardTabuSearch() {
  int best = 0, best_section = -1, best_lhs = -1, best_rhs = -1;

  auto translate = [this, &best, &best_section, &best_lhs, &best_rhs]
      (int section, int timeslot, int open_timeslot, int delta)
      -> std::pair<int, int> {
    int subject = schedule_->GetSubjectOf(section, timeslot);
    if (delta < best && !subject_tabus_[subject][open_timeslot]) {
      best = delta;
      best_section = section;
      best_lhs = timeslot;
      best_rhs = open_timeslot;
    }
    return std::make_pair(0, 0);
  };

  auto swap = [this, &best, &best_section, &best_lhs, &best_rhs]
      (int section, int lhs_timeslot, int rhs_timeslot, int delta)
      -> std::pair<int, int> {
    int lhs_subject = schedule_->GetSubjectOf(section, lhs_timeslot);
    int rhs_subject = schedule_->GetSubjectOf(section, rhs_timeslot);
    if (delta < best && !subject_tabus_[lhs_subject][rhs_timeslot]
     && !subject_tabus_[rhs_subject][lhs_timeslot]) {
      best = delta;
      best_section = section;
      best_lhs = lhs_timeslot;
      best_rhs = rhs_timeslot;
    }
    return std::make_pair(0, 0);
  };

  auto adjswap = [this, &best, &best_section, &best_lhs, &best_rhs]
      (int section, int lhs_timeslot, int rhs_timeslot, int delta)
      -> std::pair<int, int> {
    int lhs_subject = schedule_->GetSubjectOf(section, lhs_timeslot);
    int rhs_subject = schedule_->GetSubjectOf(section, rhs_timeslot);
    int new_rhs_slot = schedule_->NewRHSSlot(section, lhs_timeslot,
                                             rhs_timeslot);
    if (delta < best && !subject_tabus_[lhs_subject][new_rhs_slot]
     && !subject_tabus_[rhs_subject][lhs_timeslot]) {
      best = delta;
      best_section = section;
      best_lhs = lhs_timeslot;
      best_rhs = rhs_timeslot;
    }
    return std::make_pair(0, 0);
  };

  HardSearchTemplate(translate, swap, adjswap);

  if (best == 0) return 0;
  if (schedule_->IsFree(best_section, best_rhs)) {
    int subject = schedule_->GetSubjectOf(best_section, best_lhs);
    subject_tabus_[subject][best_rhs] = true;
    schedule_->HardTranslate(best_section, best_lhs, best_rhs);
  } else {
    int lhs_subject = schedule_->GetSubjectOf(best_section, best_lhs);
    int rhs_subject = schedule_->GetSubjectOf(best_section, best_rhs);
    if (schedule_->GetLengthOf(best_section, best_lhs)
     == schedule_->GetLengthOf(best_section, best_rhs)) {
      subject_tabus_[lhs_subject][best_rhs] = true;
      subject_tabus_[rhs_subject][best_lhs] = true;
      schedule_->HardSwap(best_section, best_lhs, best_rhs);
    } else {
      int new_rhs_slot = schedule_->NewRHSSlot(best_section, best_lhs,
                                               best_rhs);
      subject_tabus_[lhs_subject][new_rhs_slot] = true;
      subject_tabus_[rhs_subject][best_lhs] = true;
      schedule_->HardAdjSwap(best_section, best_lhs, best_rhs);
    }
  }

  return best;
}

int Solver::SoftSolver(int time_limit, int num_samples, double kappa, int tau,
                         double alpha) {
  int soft_count = schedule_->SoftCount();
  std::time_t start = std::time(NULL);
  int loops = 0;
  while (soft_count > 0 && std::difftime(std::time(NULL), start) < time_limit) {
    if (loops >= 1000) {
      soft_count = SoftSimulatedAnnealing(time_limit
                                        - std::difftime(std::time(NULL), start),
                                          num_samples, kappa, tau, alpha);
      loops = 0;
    } else {
      int delta = SoftLocalSearch(true, 0);
      if (delta == 0) loops++;
      else loops = 0;
      soft_count += delta;
    }
  }
  return soft_count;
}

int Solver::SoftLocalSearch(bool accept_side, int threshold) {
  auto translate = [this, accept_side, threshold]
      (int section, int timeslot, int open_timeslot, int delta)
      -> std::pair<int, int> {
    if (delta < 0 || (delta == 0 && accept_side)
     || (delta > 0 && delta < threshold)) {
      schedule_->SoftTranslate(section, timeslot, open_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  auto swap = [this, accept_side, threshold]
      (int section, int lhs_timeslot, int rhs_timeslot, int delta)
      -> std::pair<int, int> {
    if (delta < 0 || (delta == 0 && accept_side)
     || (delta > 0 && delta < threshold)) {
      schedule_->SoftSwap(section, lhs_timeslot, rhs_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  auto adjswap = [this, accept_side, threshold]
      (int section, int lhs_timeslot, int rhs_timeslot, int delta)
      -> std::pair<int, int> {
    if (delta < 0 || (delta == 0 && accept_side)
     || (delta > 0 && delta < threshold)) {
      schedule_->SoftAdjSwap(section, lhs_timeslot, rhs_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  return SoftSearchTemplate(translate, swap, adjswap);
}

int Solver::SoftSimulatedAnnealing(int time_limit, int num_samples,
                                     double kappa, int tau, double alpha) {
  double initial_temperature = kappa * SimulatedAnnealingSample(num_samples);
  double temperature = initial_temperature;
  int loops = 0;
  int length = tau * schedule_->GetNumSlots() * schedule_->GetSections().size()
             * schedule_->GetSubjects().size();
  int soft_count = schedule_->SoftCount();
  std::time_t start = std::time(NULL);
  while (soft_count > 0 && std::difftime(std::time(NULL), start) < time_limit) {
    int delta = SimulatedAnnealingSearch(temperature);
    soft_count += delta;
    temperature *= alpha;
    if (delta >= 0) loops++;
    if (loops >= length) {
      loops = 0;
      temperature += initial_temperature;
    }
  }
  return soft_count;
}

double Solver::SimulatedAnnealingSample(int num_samples) {
  int samples = 0, total = 0;
  auto translate = [this, &samples, &total, num_samples]
      (int section, int timeslot, int open_timeslot, int delta)
      -> std::pair<int, int> {
    samples++;
    total += delta;
    if (samples >= num_samples) return std::make_pair(2, 0);
    return std::make_pair(0, 0);
  };

  auto swap = [this, &samples, &total, num_samples]
      (int section, int lhs_timeslot, int rhs_timeslot, int delta)
      -> std::pair<int, int> {
    samples++;
    total += delta;
    if (samples >= num_samples) return std::make_pair(2, 0);
    return std::make_pair(0, 0);
  };

  auto adjswap = [this, &samples, &total, num_samples]
      (int section, int lhs_timeslot, int rhs_timeslot, int delta)
      -> std::pair<int, int> {
    samples++;
    total += delta;
    if (samples >= num_samples) return std::make_pair(2, 0);
    return std::make_pair(0, 0);
  };

  SoftSearchTemplate(translate, swap, adjswap);
  return double(total) / double(samples);
}

int Solver::SimulatedAnnealingSearch(double temperature) {
  std::uniform_real_distribution<double> prob(0, 1);

  auto translate = [this, &prob, temperature]
      (int section, int timeslot, int open_timeslot, int delta)
      -> std::pair<int, int> {
    if (delta <= 0
     || prob(rand_generator_) < std::exp(-double(delta) / temperature)) {
      schedule_->SoftTranslate(section, timeslot, open_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  auto swap = [this, &prob, temperature]
      (int section, int lhs_timeslot, int rhs_timeslot, int delta)
      -> std::pair<int, int> {
    if (delta <= 0
     || prob(rand_generator_) < std::exp(-double(delta) / temperature)) {
      schedule_->SoftSwap(section, lhs_timeslot, rhs_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  auto adjswap = [this, &prob, temperature]
      (int section, int lhs_timeslot, int rhs_timeslot, int delta)
      -> std::pair<int, int> {
    if (delta <= 0
     || prob(rand_generator_) < std::exp(-double(delta) / temperature)) {
      schedule_->SoftAdjSwap(section, lhs_timeslot, rhs_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  return SoftSearchTemplate(translate, swap, adjswap);
}

int Solver::Solve(int time_limit, int attempts, int max_best, int num_samples,
                    double kappa, int tau, double alpha) {
  std::ofstream log;
  log.open("log.txt", std::ios_base::app);

  schedule_->Initialize();
  subject_tabus_.assign(schedule_->GetSubjects().size(),
                        std::vector<bool>(schedule_->GetNumSlots(), 0));

  for (int i = 0; i < attempts; i++) {
    log << "Attempt " << i << ": " << std::endl;
    std::time_t start = std::time(NULL);
    schedule_->ResetTimetable();
    bool initial = false;
    while (!initial && std::difftime(std::time(NULL), start) < time_limit) {
      initial = InitialSchedule();
    }
    if (!initial) {
      log << "  failed to assign initial schedule." << std::endl;
      continue;
    }
    HardSolver(time_limit);
    if (schedule_->HardCount()) {
      log << "  failed to satisfy hard constraints." << std::endl;
      continue;
    }
    schedule_->SoftInitialize();
    int soft_count = SoftSolver(time_limit
                              - std::difftime(std::time(NULL), start),
                                num_samples, kappa, tau, alpha);
    if (soft_count != schedule_->SoftCount()) {
      log << "  soft_count does not match SoftCount()." << std::endl;
      log << "  soft_count value: " << soft_count << "." << std::endl;
      soft_count = schedule_->SoftCount();
      log << "  Actual soft count: " << soft_count << "." << std::endl;
    }
    log << "  Passed with soft count " << soft_count << "." << std::endl;
    Schedule* new_schedule = new Schedule(*schedule_);
    best_schedules_.emplace(soft_count, new_schedule);
    if (int(best_schedules_.size()) > max_best) {
      auto it = --best_schedules_.end();
      delete it->second;
      best_schedules_.erase(it);
    }
  }

  if (int(best_schedules_.size()) == 0) {
    log << "Did not find a working schedule." << std::endl;
    throw std::runtime_error("did not find a working schedule.");
  }

  if (int(best_schedules_.size()) < max_best) {
    log << "Found only " << best_schedules_.size() << " schedule(s); ";
    log << max_best << " needed." << std::endl;
  } else {
    log << "Found " << best_schedules_.size() << " schedule(s)." << std::endl;
  }

  log << "Best soft count: " << best_schedules_.begin()->first;
  log << std::endl;
  log.close();
  return best_schedules_.size();
}

std::pair<int, Schedule*> Solver::GetBestSchedule(int schedule) {
  auto it = best_schedules_.begin();
  for (int i = 0; i < schedule; i++) it++;
  return std::make_pair(it->first, it->second);
}