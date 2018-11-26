#include <cassert>
#include <cmath>
#include <ctime>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>

#include "schedule.h"
#include "group.h"
#include "subject.h"
#include "teacher.h"

Schedule::Schedule(int num_days, int num_slots_per_day, int seed)
    : num_days_(num_days), num_slots_per_day_(num_slots_per_day),
      hard_satisfied_(false), rand_generator_(seed) {
  num_slots_ = num_days_ * num_slots_per_day_;
}

int Schedule::GetNumDays() { return num_days_; }

int Schedule::GetNumSlots() { return num_slots_; }

int Schedule::GetNumSlotsPerDay() { return num_slots_per_day_; }

Group* Schedule::GetGroup(int idx) { return groups_[idx]; }

const std::vector<Group*>& Schedule::GetGroups() const { return groups_; }

std::vector<Group*>::iterator Schedule::GetGroupsBegin() {
  return groups_.begin();
}

std::vector<Group*>::iterator Schedule::GetGroupsEnd() {
  return groups_.end();
}

Section* Schedule::GetSection(int idx) { return sections_[idx]; }

const std::vector<Section*>& Schedule::GetSections() const { return sections_; }

std::vector<Section*>::iterator Schedule::GetSectionsBegin() {
  return sections_.begin();
}

std::vector<Section*>::iterator Schedule::GetSectionsEnd() {
  return sections_.end();
}

Subject* Schedule::GetSubject(int idx) { return subjects_[idx]; }

const std::vector<Subject*>& Schedule::GetSubjects() const { return subjects_; }

std::vector<Subject*>::iterator Schedule::GetSubjectsBegin() {
  return subjects_.begin();
}

std::vector<Subject*>::iterator Schedule::GetSubjectsEnd() {
  return subjects_.end();
}

Teacher* Schedule::GetTeacher(int idx) { return teachers_[idx]; }

const std::vector<Teacher*>& Schedule::GetTeachers() const { return teachers_; }

std::vector<Teacher*>::iterator Schedule::GetTeachersBegin() {
  return teachers_.begin();
}

std::vector<Teacher*>::iterator Schedule::GetTeachersEnd() {
  return teachers_.end();
}

void Schedule::AddGroup(int id) {
  Group* ptr = new Group(id);
  groups_.push_back(ptr);
}

void Schedule::AddTeacher(int id, const std::string &name) {
  Teacher* ptr = new Teacher(id, name);
  teachers_.push_back(ptr);
}

void Schedule::ResetTimetable() {
  hard_satisfied_ = false;
  timetable_.assign(sections_.size(), std::vector<int>(num_slots_, -1));
  teacher_table_.assign(teachers_.size(), std::vector<int>(num_slots_, 0));
  subject_tabus_.assign(subjects_.size(), std::vector<bool>(num_slots_, 0));
}

void Schedule::Initialize() {
  for (auto ptr : groups_) {
    for (auto it = ptr->GetSectionsBegin(); it != ptr->GetSectionsEnd(); it++)
      sections_.push_back(*it);
    for (auto it = ptr->GetSubjectsBegin(); it != ptr->GetSubjectsEnd(); it++)
      subjects_.push_back(*it);
  }
  ResetTimetable();
}

void Schedule::SoftInitialize() {
  hard_satisfied_ = true;
  teacher_table_.assign(teachers_.size(), std::vector<int>(num_slots_, -1));
  for (std::vector<int>::size_type it = 0; it < sections_.size(); it++)
    for (int jt = 0; jt < num_slots_; jt++)
      if (timetable_[it][jt] != -1)
        teacher_table_[GetTeacherOf(it, GetHeadOf(it, jt))][jt] = it;
}

int Schedule::GetSubjectOf(int section, int timeslot) {
  return timetable_[section][timeslot];
}

int Schedule::GetHeadOf(int section, int timeslot) {
  int head = timeslot;
  while (timetable_[section][head] == -2) head--;
  return head;
}

int Schedule::GetTeacherOf(int section, int timeslot) {
  // Get teacher of subject with head at timeslot.
  assert(timetable_[section][timeslot] >= 0);
  return GetSubject(GetSubjectOf(section, timeslot))->GetTeacher();
}

int Schedule::GetLengthOf(int section, int timeslot) {
  // Get length of subject with head at timeslot.
  assert(timetable_[section][timeslot] >= 0);
  int last = timeslot + 1;
  while (last < num_slots_ && timetable_[section][last] == -2) last++;
  return last - timeslot;
}

int Schedule::CountSectionsOf(int teacher, int timeslot) {
  if (hard_satisfied_) {
    if (teacher_table_[teacher][timeslot] == -1) return 0;
    return 1;
  }
  return teacher_table_[teacher][timeslot];
}

int Schedule::CountSectionsTranslate(int teacher, int timeslot, int section,
                                     int tr_timeslot, int open_timeslot) {
  int length = GetLengthOf(section, tr_timeslot);
  int result = CountSectionsOf(teacher, timeslot);
  if (tr_timeslot <= timeslot && timeslot < tr_timeslot + length) result--;
  if (open_timeslot <= timeslot && timeslot < open_timeslot + length) result++;
  return result;
}

int Schedule::GetSectionOf(int teacher, int timeslot) {
  assert(hard_satisfied_);
  return teacher_table_[teacher][timeslot];
}

bool Schedule::IsFree(int section, int timeslot, int num_slots) {
  // Returns True if section has num_slots free slots, starting with timeslot,
  // such that all free slots are on the same day.
  if (num_slots == 0) return true;
  if (timetable_[section][timeslot] != -1) return false;
  int rbound = timeslot, cur_moved = 0, rtimeslot = ClampDay(timeslot).second;
  while (timetable_[section][rbound] == -1 && rbound < rtimeslot &&
         cur_moved < num_slots) rbound++, cur_moved++;
  return cur_moved == num_slots;
}

bool Schedule::IsFreeTranslate(int timeslot, int section, int tr_timeslot,
                               int open_timeslot) {
  int length = GetLengthOf(section, tr_timeslot);
  return (tr_timeslot <= timeslot && timeslot < timeslot + length)
      || (IsFree(section, timeslot)
       && !(open_timeslot <= timeslot && timeslot < open_timeslot + length));
}

bool Schedule::IsValidHardTranslate(int section, int timeslot,
                                    int open_timeslot) {
  // Returns True if the section's subject with head at timeslot can be
  // hard translated to open_timeslot.
  assert(timetable_[section][timeslot] >= 0);
  assert(timetable_[section][open_timeslot] == -1);
  return IsFree(section, open_timeslot, GetLengthOf(section, timeslot));
}

bool Schedule::IsValidHardSwap(int section, int lhs_timeslot,
                               int rhs_timeslot) {
  // Returns True if the section's subject with head at lhs_timeslot can be
  // hard swapped to subject with head at rhs_timeslot. 
  assert(timetable_[section][lhs_timeslot] >= 0);
  assert(timetable_[section][rhs_timeslot] >= 0);
  return GetLengthOf(section, lhs_timeslot) ==
         GetLengthOf(section, rhs_timeslot);
}

bool Schedule::IsValidSoftTranslate(int section, int timeslot,
                                    int open_timeslot) {
  return IsValidHardTranslate(section, timeslot, open_timeslot) &&
         (HardCountTranslate(section, timeslot, open_timeslot) == 0);
}

bool Schedule::IsValidSoftSwap(int section, int lhs_timeslot,
                               int rhs_timeslot) {
  return IsValidHardSwap(section, lhs_timeslot, rhs_timeslot) &&
         (HardCountSwap(section, lhs_timeslot, rhs_timeslot) == 0);
}

std::pair<int, int> Schedule::ClampDay(int timeslot) {
  // Returns [ltimeslot, rtimeslot) for the day timeslot is in.
  int lbound = timeslot / num_slots_per_day_ * num_slots_per_day_;
  return std::make_pair(lbound, lbound + num_slots_per_day_);
}

void Schedule::HardAssign(int subject, int section, int timeslot,
                          int num_slots) {
  int teacher = GetSubject(subject)->GetTeacher();
  for (int i = 0; i < num_slots; i++) {
    timetable_[section][timeslot+i] = -2;
    teacher_table_[teacher][timeslot+i]++;
  }
  timetable_[section][timeslot] = subject;
}

int Schedule::HardCountTranslate(int section, int timeslot, int open_timeslot) {
  int result = 0;
  for (auto& ptr : hard_constraints_)
    result += ptr->CountTranslate(section, timeslot, open_timeslot);
  return result;
}

void Schedule::HardTranslate(int section, int timeslot, int open_timeslot) {
  int subject = GetSubjectOf(section, timeslot);
  int teacher = GetSubject(subject)->GetTeacher();
  int num_slots = GetLengthOf(section, timeslot);
  for (int i = 0; i < num_slots; i++) {
    timetable_[section][timeslot+i] = -1;
    timetable_[section][open_timeslot+i] = -2;
    teacher_table_[teacher][timeslot+i]--;
    teacher_table_[teacher][open_timeslot+i]++;
  }
  timetable_[section][open_timeslot] = subject;
}

int Schedule::HardCountSwap(int section, int lhs_timeslot, int rhs_timeslot) {
  int result = 0;
  for (auto& ptr : hard_constraints_)
    result += ptr->CountSwapTimeslot(section, lhs_timeslot, rhs_timeslot);
  return result;
}

void Schedule::HardSwap(int section, int lhs_timeslot, int rhs_timeslot) {
  int lhs_subject = GetSubjectOf(section, lhs_timeslot);
  int rhs_subject = GetSubjectOf(section, rhs_timeslot);
  int lhs_teacher = GetSubject(lhs_subject)->GetTeacher();
  int rhs_teacher = GetSubject(rhs_subject)->GetTeacher();
  int num_slots = GetLengthOf(section, lhs_timeslot);
  for (int i = 0; i < num_slots; i++) {
    teacher_table_[lhs_teacher][lhs_timeslot+i]--;
    teacher_table_[lhs_teacher][rhs_timeslot+i]++;
    teacher_table_[rhs_teacher][rhs_timeslot+i]--;
    teacher_table_[rhs_teacher][lhs_timeslot+i]++;
  }
  timetable_[section][lhs_timeslot] = rhs_subject;
  timetable_[section][rhs_timeslot] = lhs_subject;
}

int Schedule::SoftCountTranslate(int section, int timeslot, int open_timeslot) {
  int result = 0;
  for (auto& ptr : soft_constraints_)
    result += ptr->CountTranslate(section, timeslot, open_timeslot);
  return result;
}

void Schedule::SoftTranslate(int section, int timeslot, int open_timeslot) {
  int subject = GetSubjectOf(section, timeslot);
  int teacher = GetSubject(subject)->GetTeacher();
  int num_slots = GetLengthOf(section, timeslot);
  for (int i = 0; i < num_slots; i++) {
    timetable_[section][timeslot+i] = -1;
    timetable_[section][open_timeslot+i] = -2;
    teacher_table_[teacher][timeslot+i] = -1;
    teacher_table_[teacher][open_timeslot+i] = section;
  }
  timetable_[section][open_timeslot] = subject;
}

int Schedule::SoftCountSwap(int section, int lhs_timeslot, int rhs_timeslot) {
  int result = 0;
  for (auto& ptr : soft_constraints_)
    result += ptr->CountSwapTimeslot(section, lhs_timeslot, rhs_timeslot);
  return result;
}

void Schedule::SoftSwap(int section, int lhs_timeslot, int rhs_timeslot) {
  int lhs_subject = GetSubjectOf(section, lhs_timeslot);
  int rhs_subject = GetSubjectOf(section, rhs_timeslot);
  int lhs_teacher = GetSubject(lhs_subject)->GetTeacher();
  int rhs_teacher = GetSubject(rhs_subject)->GetTeacher();
  int num_slots = GetLengthOf(section, lhs_timeslot);
  for (int i = 0; i < num_slots; i++) {
    teacher_table_[lhs_teacher][lhs_timeslot+i] = -1;
    teacher_table_[rhs_teacher][rhs_timeslot+i] = -1;
    teacher_table_[lhs_teacher][rhs_timeslot+i] = section;
    teacher_table_[rhs_teacher][lhs_timeslot+i] = section;
  }
  timetable_[section][lhs_timeslot] = rhs_subject;
  timetable_[section][rhs_timeslot] = lhs_subject;
}

int Schedule::HardCount() {
  int result = 0;
  for (auto& ptr : hard_constraints_) 
    result += ptr->CountAll();
  return result;
}

int Schedule::SoftCount() {
  int result = 0;
  for (auto& ptr : soft_constraints_)
    result += ptr->CountAll();
  return result;
}

bool Schedule::InitialSchedule() {
  for (auto ptr : groups_) {
    for (auto it = ptr->GetSectionsBegin(); it != ptr->GetSectionsEnd(); it++) {
      std::vector< std::pair<int, int> > unassigned;
      for (auto jt = ptr->GetSubjectsBegin(); jt != ptr->GetSubjectsEnd(); jt++)
        for (auto kt = (*jt)->GetSlotsBegin(); kt != (*jt)->GetSlotsEnd(); kt++)
          unassigned.emplace_back((*jt)->GetId(), *kt);
      std::shuffle(unassigned.begin(), unassigned.end(), rand_generator_);
      for (auto sb : unassigned) {
        int section = (*it)->GetId(), subject = sb.first, num_slots = sb.second;
        bool assigned = false;
        for (int kt = 0; kt < num_slots_; kt++) {
          if (IsFree(section, kt, num_slots)) {
            HardAssign(subject, section, kt, num_slots);
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

int Schedule::HardSolver(int time_limit) {
  int hard_count = HardCount();
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

int Schedule::HardLocalSearch() {
  int result = 0;

  auto translate = [this, &result]
      (int section, int timeslot, int open_timeslot) -> std::pair<int, int> {
    if (!IsValidHardTranslate(section, timeslot, open_timeslot))
      return std::make_pair(0, 0);
    int delta = HardCountTranslate(section, timeslot, open_timeslot);
    if (delta <= 0) {
      HardTranslate(section, timeslot, open_timeslot);
      result += delta;
      return std::make_pair(2, 0);
    }
    return std::make_pair(0, 0);
  };

  auto swap = [this, &result]
      (int section, int lhs_timeslot, int rhs_timeslot) -> std::pair<int, int> {
    if (!IsValidHardSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    int delta = HardCountSwap(section, lhs_timeslot, rhs_timeslot);
    if (delta <= 0) {
      HardSwap(section, lhs_timeslot, rhs_timeslot);
      result += delta;
      return std::make_pair(2, 0);
    }
    return std::make_pair(0, 0);
  };

  SearchTemplate(translate, swap);
  return result;
}

int Schedule::HardTabuSearch() {
  int best = 0, best_section = -1, best_lhs = -1, best_rhs = -1;

  auto translate = [this, &best, &best_section, &best_lhs, &best_rhs]
      (int section, int timeslot, int open_timeslot) -> std::pair<int, int> {
    if (!IsValidHardTranslate(section, timeslot, open_timeslot))
      return std::make_pair(0, 0);
    int delta = HardCountTranslate(section, timeslot, open_timeslot);
    int subject = GetSubjectOf(section, timeslot);
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
    if (!IsValidHardSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    int delta = HardCountSwap(section, lhs_timeslot, rhs_timeslot);
    int subject = GetSubjectOf(section, lhs_timeslot);
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
  int subject = GetSubjectOf(best_section, best_lhs);
  subject_tabus_[subject][best_rhs] = true;
  if (IsFree(best_section, best_rhs))
    HardTranslate(best_section, best_lhs, best_rhs);
  else HardSwap(best_section, best_lhs, best_rhs);
  return best;
}

int Schedule::SoftSolver(int time_limit, int num_samples, double kappa, int tau,
                         double alpha) {
  int soft_count = SoftCount();
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

int Schedule::SoftLocalSearch(bool accept_side, int threshold) {
  auto translate = [this, accept_side, threshold]
      (int section, int timeslot, int open_timeslot) -> std::pair<int, int> {
    if (!IsValidSoftTranslate(section, timeslot, open_timeslot))
      return std::make_pair(0, 0);
    int delta = SoftCountTranslate(section, timeslot, open_timeslot);
    if (delta < 0 || (delta == 0 && accept_side)
     || (delta > 0 && delta < threshold)) {
      SoftTranslate(section, timeslot, open_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  auto swap = [this, accept_side, threshold]
      (int section, int lhs_timeslot, int rhs_timeslot) -> std::pair<int, int> {
    if (!IsValidSoftSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    int delta = SoftCountSwap(section, lhs_timeslot, rhs_timeslot);
    if (delta < 0 || (delta == 0 && accept_side)
     || (delta > 0 && delta < threshold)) {
      SoftSwap(section, lhs_timeslot, rhs_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  return SearchTemplate(translate, swap);
}

int Schedule::SoftSimulatedAnnealing(int time_limit, int num_samples,
                                     double kappa, int tau, double alpha) {
  double initial_temperature = kappa * SimulatedAnnealingSample(num_samples);
  double temperature = initial_temperature;
  int loops = 0;
  int length = tau * num_slots_ * sections_.size() * subjects_.size();
  int soft_count = SoftCount();
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

double Schedule::SimulatedAnnealingSample(int num_samples) {
  int samples = 0, total = 0;
  auto translate = [this, &samples, &total, num_samples]
      (int section, int timeslot, int open_timeslot) -> std::pair<int, int> {
    if (!IsValidSoftTranslate(section, timeslot, open_timeslot))
      return std::make_pair(0, 0);
    samples++;
    total += SoftCountTranslate(section, timeslot, open_timeslot);
    if (samples >= num_samples) return std::make_pair(2, 0);
    return std::make_pair(0, 0);
  };

  auto swap = [this, &samples, &total, num_samples]
      (int section, int lhs_timeslot, int rhs_timeslot) -> std::pair<int, int> {
    if (!IsValidSoftSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    samples++;
    total += SoftCountSwap(section, lhs_timeslot, rhs_timeslot);
    if (samples >= num_samples) return std::make_pair(2, 0);
    return std::make_pair(0, 0);
  };

  SearchTemplate(translate, swap);
  return double(total) / double(samples);
}

int Schedule::SimulatedAnnealingSearch(double temperature) {
  std::uniform_real_distribution<double> prob(0, 1);

  auto translate = [this, &prob, temperature]
      (int section, int timeslot, int open_timeslot) -> std::pair<int, int> {
    if (!IsValidSoftTranslate(section, timeslot, open_timeslot))
      return std::make_pair(0, 0);
    int delta = SoftCountTranslate(section, timeslot, open_timeslot);
    if (delta <= 0
     || prob(rand_generator_) < std::exp(-double(delta) / temperature)) {
      SoftTranslate(section, timeslot, open_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  auto swap = [this, &prob, temperature]
      (int section, int lhs_timeslot, int rhs_timeslot) -> std::pair<int, int> {
    if (!IsValidSoftSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    int delta = SoftCountSwap(section, lhs_timeslot, rhs_timeslot);
    if (delta <= 0
     || prob(rand_generator_) < std::exp(-double(delta) / temperature)) {
      SoftSwap(section, lhs_timeslot, rhs_timeslot);
      return std::make_pair(1, delta);
    }
    return std::make_pair(0, 0);
  };

  return SearchTemplate(translate, swap);
}

int Schedule::Solve(int time_limit, int attempts, int max_best, int num_samples,
                    double kappa, int tau, double alpha) {
  std::ofstream log;
  log.open("log.txt", std::ios_base::app);

  Initialize();
  for (int i = 0; i < attempts; i++) {
    log << "Attempt " << i << ": " << std::endl;
    std::time_t start = std::time(NULL);
    ResetTimetable();
    bool initial = false;
    while (!initial && std::difftime(std::time(NULL), start) < time_limit)
      initial = InitialSchedule();
    if (!initial) {
      log << "  failed to assign initial schedule." << std::endl;
      continue;
    }
    HardSolver(time_limit);
    if (HardCount()) {
      log << "  failed to satisfy hard constraints." << std::endl;
      continue;
    }
    SoftInitialize();
    int soft_count = SoftSolver(time_limit
                              - std::difftime(std::time(NULL), start),
                                num_samples, kappa, tau, alpha);
    if (soft_count != SoftCount()) {
      log << "  soft_count does not match SoftCount()." << std::endl;
      log << "  soft_count value: " << soft_count << "." << std::endl;
      soft_count = SoftCount();
      log << "  Actual soft count: " << soft_count << "." << std::endl;
    }
    log << "  Passed with soft count " << soft_count << "." << std::endl;
    best_tables_.emplace(soft_count, std::make_pair(timetable_, teacher_table_));
    if (int(best_tables_.size()) > max_best) {
      best_tables_.erase(--best_tables_.end());
    }
  }

  if (int(best_tables_.size()) == 0) {
    log << "Did not find a working schedule." << std::endl;
    throw std::runtime_error("did not find a working schedule.");
  }

  if (int(best_tables_.size()) < max_best) {
    log << "Found only " << best_tables_.size() << " schedule(s); ";
    log << max_best << " needed." << std::endl;
  } else {
    log << "Found " << best_tables_.size() << " schedule(s)." << std::endl;
  }

  SwitchScheduleTo(0);
  log << "Best soft count: " << SoftCount() << std::endl;
  log.close();
  return best_tables_.size();
}

void Schedule::SwitchScheduleTo(int schedule) {
  auto it = best_tables_.begin();
  for (int i = 0; i < schedule; i++) it++;
  timetable_ = it->second.first;
  teacher_table_ = it->second.second;
  hard_satisfied_ = true;
}

void Schedule::TestPrint() {
  std::cout << "   ";
  for (std::vector<int>::size_type i = 0; i < sections_.size(); i++)
    std::cout << (i%10) << "   ";
  std::cout << std::endl;
  for (int j = 0; j < num_slots_; j++) {
    if (j < 10) std::cout << " ";
    std::cout << (j%100);
    for (std::vector<int>::size_type it = 0; it < sections_.size(); it++) {
      auto jt = timetable_[it][j];
      if (jt >= 0) {
        int id = GetSubject(jt)->GetId();
        if (id < 10) std::cout << "   ";
        else std::cout << "  ";
        std::cout << id;
      } else if (jt == -1) std::cout << "    ";
      else std::cout << " ---"; 
    }
    std::cout << std::endl;
    if (j % num_slots_per_day_ == num_slots_per_day_ - 1 && j != num_slots_-1) {
      std::cout << std::endl;
      std::cout << "   ";
      for (std::vector<int>::size_type i = 0; i < sections_.size(); i++)
        std::cout << (i%10) << "   ";
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;

  // std::cout << "  ";
  // for (std::vector<int>::size_type i = 0; i < teachers_.size(); i++)
  //   std::cout << (i%10) << " ";
  // std::cout << std::endl;
  // for (int j = 0; j < num_slots_; j++) {
  //   if (j < 10) std::cout << " ";
  //   std::cout << (j%100);
  //   for (std::vector<int>::size_type it = 0; it < teachers_.size(); it++) {
  //     auto jt = teacher_table_[it][j];
  //     if (jt >= 0) std::cout << ' ' << jt;
  //     else std::cout << "  ";
  //   }
  //   std::cout << std::endl;
  //   if (j % num_slots_per_day_ == num_slots_per_day_ - 1 && j != num_slots_-1) {
  //     std::cout << std::endl;
  //     std::cout << "  ";
  //     for (std::vector<int>::size_type i = 0; i < teachers_.size(); i++)
  //       std::cout << (i%10) << " ";
  //     std::cout << std::endl;
  //   }
  // }

  std::cout << std::endl;
  if (!hard_satisfied_) std::cout << HardCount() << std::endl;
  else std::cout << SoftCount() << std::endl;
  std::cout << std::endl;
}