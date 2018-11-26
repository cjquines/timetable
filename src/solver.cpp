#include <ctime>
#include <cmath>

#include <algorithm>
#include <fstream>
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
    for (auto it = ptr->GetSectionsBegin(); it != ptr->GetSectionsEnd(); it++) {
      std::vector< std::pair<int, int> > unassigned;
      for (auto jt = ptr->GetSubjectsBegin(); jt != ptr->GetSubjectsEnd(); jt++)
        for (auto kt = (*jt)->GetSlotsBegin(); kt != (*jt)->GetSlotsEnd(); kt++)
          unassigned.emplace_back((*jt)->GetId(), *kt);
      std::shuffle(unassigned.begin(), unassigned.end(), rand_generator_);
      for (auto sb : unassigned) {
        int section = (*it)->GetId(), subject = sb.first, num_slots = sb.second;
        bool assigned = false;
        for (int kt = 0; kt < schedule_->GetNumSlots(); kt++) {
          if (schedule_->IsFree(section, kt, num_slots)) {
            schedule_->HardAssign(subject, section, kt, num_slots);
            assigned = true;
            break;
          }
        }
        if (!assigned) return false;
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
      (int section, int timeslot, int open_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidHardTranslate(section, timeslot, open_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->HardCountTranslate(section, timeslot, open_timeslot);
    if (delta <= 0) {
      schedule_->HardTranslate(section, timeslot, open_timeslot);
      result += delta;
      return std::make_pair(2, 0);
    }
    return std::make_pair(0, 0);
  };

  auto swap = [this, &result]
      (int section, int lhs_timeslot, int rhs_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidHardSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->HardCountSwap(section, lhs_timeslot, rhs_timeslot);
    if (delta <= 0) {
      schedule_->HardSwap(section, lhs_timeslot, rhs_timeslot);
      result += delta;
      return std::make_pair(2, 0);
    }
    return std::make_pair(0, 0);
  };

  SearchTemplate(translate, swap);
  return result;
}

int Solver::HardTabuSearch() {
  int best = 0, best_section = -1, best_lhs = -1, best_rhs = -1;

  auto translate = [this, &best, &best_section, &best_lhs, &best_rhs]
      (int section, int timeslot, int open_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidHardTranslate(section, timeslot, open_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->HardCountTranslate(section, timeslot, open_timeslot);
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
      (int section, int lhs_timeslot, int rhs_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidHardSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->HardCountSwap(section, lhs_timeslot, rhs_timeslot);
    int subject = schedule_->GetSubjectOf(section, lhs_timeslot);
    if (delta < best && !subject_tabus_[subject][rhs_timeslot]) {
      best = delta;
      best_section = section;
      best_lhs = lhs_timeslot;
      best_rhs = rhs_timeslot;
    }
    return std::make_pair(0, 0);
  };

  SearchTemplate(translate, swap);

  if (best == 0) return 0;
  int subject = schedule_->GetSubjectOf(best_section, best_lhs);
  subject_tabus_[subject][best_rhs] = true;
  if (schedule_->IsFree(best_section, best_rhs))
    schedule_->HardTranslate(best_section, best_lhs, best_rhs);
  else schedule_->HardSwap(best_section, best_lhs, best_rhs);
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
      (int section, int timeslot, int open_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidSoftTranslate(section, timeslot, open_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->SoftCountTranslate(section, timeslot, open_timeslot);
    if (delta < 0 || (delta == 0 && accept_side)
     || (delta > 0 && delta < threshold)) {
      schedule_->SoftTranslate(section, timeslot, open_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  auto swap = [this, accept_side, threshold]
      (int section, int lhs_timeslot, int rhs_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidSoftSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->SoftCountSwap(section, lhs_timeslot, rhs_timeslot);
    if (delta < 0 || (delta == 0 && accept_side)
     || (delta > 0 && delta < threshold)) {
      schedule_->SoftSwap(section, lhs_timeslot, rhs_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  return SearchTemplate(translate, swap);
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
      (int section, int timeslot, int open_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidSoftTranslate(section, timeslot, open_timeslot))
      return std::make_pair(0, 0);
    samples++;
    total += schedule_->SoftCountTranslate(section, timeslot, open_timeslot);
    if (samples >= num_samples) return std::make_pair(2, 0);
    return std::make_pair(0, 0);
  };

  auto swap = [this, &samples, &total, num_samples]
      (int section, int lhs_timeslot, int rhs_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidSoftSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    samples++;
    total += schedule_->SoftCountSwap(section, lhs_timeslot, rhs_timeslot);
    if (samples >= num_samples) return std::make_pair(2, 0);
    return std::make_pair(0, 0);
  };

  SearchTemplate(translate, swap);
  return double(total) / double(samples);
}

int Solver::SimulatedAnnealingSearch(double temperature) {
  std::uniform_real_distribution<double> prob(0, 1);

  auto translate = [this, &prob, temperature]
      (int section, int timeslot, int open_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidSoftTranslate(section, timeslot, open_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->SoftCountTranslate(section, timeslot, open_timeslot);
    if (delta <= 0
     || prob(rand_generator_) < std::exp(-double(delta) / temperature)) {
      schedule_->SoftTranslate(section, timeslot, open_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  auto swap = [this, &prob, temperature]
      (int section, int lhs_timeslot, int rhs_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidSoftSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->SoftCountSwap(section, lhs_timeslot, rhs_timeslot);
    if (delta <= 0
     || prob(rand_generator_) < std::exp(-double(delta) / temperature)) {
      schedule_->SoftSwap(section, lhs_timeslot, rhs_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  return SearchTemplate(translate, swap);
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

  log << "Best soft count: " << best_schedules_.begin()->second->SoftCount();
  log << std::endl;
  log.close();
  return best_schedules_.size();
}

Schedule* Solver::GetBestSchedule(int schedule) {
  auto it = best_schedules_.begin();
  for (int i = 0; i < schedule; i++) it++;
  return it->second;
}