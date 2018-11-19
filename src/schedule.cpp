#include <cassert>
#include <cmath>
#include <ctime>

#include <algorithm>
#include <limits>
#include <iostream>
#include <random>

#include "schedule.h"
#include "group.h"
#include "subject.h"
#include "teacher.h"
#include "constraints/distinctperday.h"
#include "constraints/evendismissal.h"
#include "constraints/nonsimultaneous.h"
#include "constraints/reqfirstsubject.h"
#include "constraints/subjectgaps.h"
#include "constraints/subjecttime.h"
#include "constraints/teachertime.h"

Schedule::Schedule(const int &num_days, const int &num_slots_per_day,
                   const int &seed)
    : num_days_(num_days), num_slots_per_day_(num_slots_per_day),
      hard_satisfied_(false), rand_generator_(seed) {
  num_slots_ = num_days_ * num_slots_per_day_;
  best_soft_count_ = std::numeric_limits<int>::max();
}

int Schedule::GetNumDays() { return num_days_; }

int Schedule::GetNumSlots() { return num_slots_; }

int Schedule::GetNumSlotsPerDay() { return num_slots_per_day_; }

Group* Schedule::GetGroup(const int &idx) { return groups_[idx]; }

std::vector<Group*>::iterator Schedule::GetGroupsBegin() {
  return groups_.begin();
}

std::vector<Group*>::iterator Schedule::GetGroupsEnd() {
  return groups_.end();
}

Section* Schedule::GetSection(const int &idx) { return sections_[idx]; }

std::vector<Section*>::iterator Schedule::GetSectionsBegin() {
  return sections_.begin();
}

std::vector<Section*>::iterator Schedule::GetSectionsEnd() {
  return sections_.end();
}

Subject* Schedule::GetSubject(const int &idx) { return subjects_[idx]; }

std::vector<Subject*>::iterator Schedule::GetSubjectsBegin() {
  return subjects_.begin();
}

std::vector<Subject*>::iterator Schedule::GetSubjectsEnd() {
  return subjects_.end();
}

Teacher* Schedule::GetTeacher(const int &idx) { return teachers_[idx]; }

std::vector<Teacher*>::iterator Schedule::GetTeachersBegin() {
  return teachers_.begin();
}

std::vector<Teacher*>::iterator Schedule::GetTeachersEnd() {
  return teachers_.end();
}

void Schedule::AddGroup(const int &id) {
  Group* ptr = new Group(id);
  groups_.push_back(ptr);
}

void Schedule::AddTeacher(const int &id, const std::string &name) {
  Teacher* ptr = new Teacher(id, name);
  teachers_.push_back(ptr);
}

void Schedule::AddDistinctPerDay(const int &priority) {
  std::unique_ptr<Constraint> ptr = std::make_unique<DistinctPerDay>(
    this, priority);
  if (priority <= 0) hard_constraints_.push_back(std::move(ptr));
  else soft_constraints_.push_back(std::move(ptr));
}

void Schedule::AddEvenDismissal(const int &priority) {
  std::unique_ptr<Constraint> ptr = std::make_unique<EvenDismissal>(
    this, priority);
  if (priority <= 0) hard_constraints_.push_back(std::move(ptr));
  else soft_constraints_.push_back(std::move(ptr));
}

void Schedule::AddNonSimultaneous(const int &priority) {
  std::unique_ptr<Constraint> ptr = std::make_unique<NonSimultaneous>(
    this, priority);
  if (priority <= 0) hard_constraints_.push_back(std::move(ptr));
  else soft_constraints_.push_back(std::move(ptr));
}

void Schedule::AddReqFirstSubject(const int &priority) {
  std::unique_ptr<Constraint> ptr = std::make_unique<ReqFirstSubject>(
    this, priority);
  if (priority <= 0) hard_constraints_.push_back(std::move(ptr));
  else soft_constraints_.push_back(std::move(ptr));
}

void Schedule::AddSubjectGaps(const int &priority) {
  std::unique_ptr<Constraint> ptr = std::make_unique<SubjectGaps>(
    this, priority);
  if (priority <= 0) hard_constraints_.push_back(std::move(ptr));
  else soft_constraints_.push_back(std::move(ptr));
}

void Schedule::AddSubjectTime(const int &priority, const int &subject,
                              const std::vector<int> &unassignable) {
  std::unique_ptr<Constraint> ptr = std::make_unique<SubjectTime>(
    this, priority, subject, unassignable);
  if (priority <= 0) hard_constraints_.push_back(std::move(ptr));
  else soft_constraints_.push_back(std::move(ptr));
}

void Schedule::AddTeacherTime(const int &priority, const int &teacher,
                              const std::vector<int> &unassignable) {
  std::unique_ptr<Constraint> ptr = std::make_unique<TeacherTime>(
    this, priority, teacher, unassignable);
  if (priority <= 0) hard_constraints_.push_back(std::move(ptr));
  else soft_constraints_.push_back(std::move(ptr));
}

void Schedule::Initialize() {
  for (auto ptr : groups_) {
    for (auto it = ptr->GetSectionsBegin(); it != ptr->GetSectionsEnd(); it++)
      sections_.push_back(*it);
    for (auto it = ptr->GetSubjectsBegin(); it != ptr->GetSubjectsEnd(); it++)
      subjects_.push_back(*it);
  }

  AddDistinctPerDay(0);
  AddNonSimultaneous(0);
  AddReqFirstSubject(0);

  hard_satisfied_ = false;
  timetable_.assign(sections_.size(), std::vector<int>(num_slots_, -1));
  teacher_table_.assign(teachers_.size(), std::vector<int>(num_slots_, 0));
  subject_tabus_.assign(subjects_.size(), std::vector<bool>(num_slots_, 0));
}

void Schedule::SoftInitialize() {
  assert(HardCount() == 0);
  hard_satisfied_ = true;
  teacher_table_.assign(teachers_.size(), std::vector<int>(num_slots_, -1));
  for (std::vector<int>::size_type it = 0; it < sections_.size(); it++)
    for (int jt = 0; jt < num_slots_; jt++)
      if (timetable_[it][jt] != -1)
        teacher_table_[GetTeacherOf(it, GetHeadOf(it, jt))][jt] = it;
}

int Schedule::GetSubjectOf(const int &section, const int &timeslot) {
  return timetable_[section][timeslot];
}

int Schedule::GetHeadOf(const int &section, const int &timeslot) {
  int head = timeslot;
  while (timetable_[section][head] == -2) {
    head--;
    assert(head >= 0);
  }
  return head;
}

int Schedule::GetTeacherOf(const int &section, const int &timeslot) {
  // Get teacher of subject with head at timeslot.
  assert(timetable_[section][timeslot] >= 0);
  return GetSubject(GetSubjectOf(section, timeslot))->GetTeacher();
}

int Schedule::GetLengthOf(const int &section, const int &timeslot) {
  // Get length of subject with head at timeslot.
  assert(timetable_[section][timeslot] >= 0);
  int last = timeslot + 1;
  while (last < num_slots_ && timetable_[section][last] == -2) last++;
  return last - timeslot;
}

int Schedule::CountSectionsOf(const int &teacher, const int &timeslot) {
  if (hard_satisfied_) {
    if (teacher_table_[teacher][timeslot] == -1) return 0;
    return 1;
  }
  return teacher_table_[teacher][timeslot];
}

int Schedule::GetSectionOf(const int &teacher, const int &timeslot) {
  assert(hard_satisfied_);
  return teacher_table_[teacher][timeslot];
}

bool Schedule::IsFree(const int &section, const int &timeslot,
                      const int &num_slots) {
  // Returns True if section has num_slots free slots, starting with timeslot,
  // such that all free slots are on the same day.
  if (num_slots == 0) return true;
  if (timetable_[section][timeslot] != -1) return false;
  int rbound = timeslot, cur_moved = 0, rtimeslot = ClampDay(timeslot).second;
  while (timetable_[section][rbound] == -1 && rbound < rtimeslot &&
         cur_moved < num_slots) rbound++, cur_moved++;
  return cur_moved == num_slots;
}

bool Schedule::IsValidHardTranslate(const int &section, const int &timeslot,
                                    const int &open_timeslot) {
  // Returns True if the section's subject with head at timeslot can be
  // hard translated to open_timeslot.
  assert(timetable_[section][timeslot] >= 0);
  assert(timetable_[section][open_timeslot] == -1);
  return IsFree(section, open_timeslot, GetLengthOf(section, timeslot));
}

bool Schedule::IsValidHardSwap(const int &section, const int &lhs_timeslot,
                               const int &rhs_timeslot) {
  // Returns True if the section's subject with head at lhs_timeslot can be
  // hard swapped to subject with head at rhs_timeslot. 
  assert(timetable_[section][lhs_timeslot] >= 0);
  assert(timetable_[section][rhs_timeslot] >= 0);
  return GetLengthOf(section, lhs_timeslot) ==
         GetLengthOf(section, rhs_timeslot);
}

bool Schedule::IsValidSoftTranslate(const int &section, const int &timeslot,
                                    const int &open_timeslot) {
  return IsValidHardTranslate(section, timeslot, open_timeslot) &&
         (HardCountTranslate(section, timeslot, open_timeslot) == 0);
}

bool Schedule::IsValidSoftSwap(const int &section, const int &lhs_timeslot,
                               const int &rhs_timeslot) {
  return IsValidHardSwap(section, lhs_timeslot, rhs_timeslot) &&
         (HardCountSwap(section, lhs_timeslot, rhs_timeslot) == 0);
}

std::pair<int, int> Schedule::ClampDay(const int &timeslot) {
  // Returns [ltimeslot, rtimeslot) for the day timeslot is in.
  int lbound = timeslot / num_slots_per_day_ * num_slots_per_day_;
  return std::make_pair(lbound, lbound + num_slots_per_day_);
}

void Schedule::HardAssign(const int &subject, const int &section,
                          const int &timeslot, const int &num_slots) {
  int teacher = GetSubject(subject)->GetTeacher();
  for (int i = 0; i < num_slots; i++) {
    timetable_[section][timeslot+i] = -2;
    teacher_table_[teacher][timeslot+i]++;
  }
  timetable_[section][timeslot] = subject;
}

int Schedule::HardCountTranslate(const int &section, const int &timeslot,
                                 const int &open_timeslot) {
  int result = 0;
  for (auto& ptr : hard_constraints_)
    result += ptr->CountTranslate(section, timeslot, open_timeslot);
  return result;
}

void Schedule::HardTranslate(const int &section, const int &timeslot,
                             const int &open_timeslot) {
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

int Schedule::HardCountSwap(const int &section, const int &lhs_timeslot,
                            const int &rhs_timeslot) {
  int result = 0;
  for (auto& ptr : hard_constraints_)
    result += ptr->CountSwapTimeslot(section, lhs_timeslot, rhs_timeslot);
  return result;
}

void Schedule::HardSwap(const int &section, const int &lhs_timeslot,
                        const int &rhs_timeslot) {
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

int Schedule::SoftCountTranslate(const int &section, const int &timeslot,
                                 const int &open_timeslot) {
  int result = 0;
  for (auto& ptr : soft_constraints_)
    result += ptr->CountTranslate(section, timeslot, open_timeslot);
  return result;
}

void Schedule::SoftTranslate(const int &section, const int &timeslot,
                             const int &open_timeslot) {
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

int Schedule::SoftCountSwap(const int &section, const int &lhs_timeslot,
                            const int &rhs_timeslot) {
  int result = 0;
  for (auto& ptr : soft_constraints_)
    result += ptr->CountSwapTimeslot(section, lhs_timeslot, rhs_timeslot);
  return result;
}

void Schedule::SoftSwap(const int &section, const int &lhs_timeslot,
                        const int &rhs_timeslot) {
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

void Schedule::InitialSchedule() {
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
        assert(assigned);
      }
    }
  }
}

int Schedule::HardSolver(const int &time_limit) {
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
  std::vector< std::pair<int, int> > to_swap;
  for (auto ptr : groups_)
    for (auto it = ptr->GetSectionsBegin(); it != ptr->GetSectionsEnd(); it++)
      for (auto jt = 0; jt < num_slots_; jt++)
        to_swap.emplace_back((*it)->GetId(), jt);
  std::shuffle(to_swap.begin(), to_swap.end(), rand_generator_);
  int result = 0;
  for (std::vector<int>::size_type i = 0; i < to_swap.size(); i++) {
    int section = to_swap[i].first;
    if (timetable_[section][to_swap[i].second] < 0) continue;
    for (std::vector<int>::size_type j = i+1; j < to_swap.size(); j++) {
      if (to_swap[j].first == section) {
        if (timetable_[section][to_swap[j].second] >= 0 &&
            IsValidHardSwap(section, to_swap[i].second, to_swap[j].second)) {
          int delta = HardCountSwap(section, to_swap[i].second,
                                    to_swap[j].second);
          if (delta <= 0) {
            HardSwap(section, to_swap[i].second, to_swap[j].second);
            result += delta;
            break;
          }
        } else if (timetable_[section][to_swap[j].second] == -1 &&
                   IsValidHardTranslate(section, to_swap[i].second,
                                        to_swap[j].second)) {
          int delta = HardCountTranslate(section, to_swap[i].second,
                                         to_swap[j].second);
          if (delta <= 0) {
            HardTranslate(section, to_swap[i].second, to_swap[j].second);
            result += delta;
            break;
          }
        }
      }
    }
  }
  return result;
}

int Schedule::HardTabuSearch() {
  int best = 0, lhs_best = -1, rhs_best = -1;
  std::vector< std::pair<int, int> > to_swap;
  for (auto ptr : groups_)
    for (auto it = ptr->GetSectionsBegin(); it != ptr->GetSectionsEnd(); it++)
      for (auto jt = 0; jt < num_slots_; jt++)
        to_swap.emplace_back((*it)->GetId(), jt);
  std::shuffle(to_swap.begin(), to_swap.end(), rand_generator_);
  for (std::vector<int>::size_type i = 0; i < to_swap.size(); i++) {
    int section = to_swap[i].first;
    int subject = GetSubjectOf(section, to_swap[i].second);
    if (subject < 0) continue;
    for (std::vector<int>::size_type j = i+1; j < to_swap.size(); j++) {
      if (to_swap[j].first == section) {
        int delta = 0;
        if (timetable_[section][to_swap[j].second] >= 0 &&
            IsValidHardSwap(section, to_swap[i].second, to_swap[j].second))
          delta = HardCountSwap(section, to_swap[i].second, to_swap[j].second);
        else if (timetable_[section][to_swap[j].second] == -1 &&
            IsValidHardTranslate(section, to_swap[i].second, to_swap[j].second))
          delta = HardCountTranslate(section, to_swap[i].second,
                                     to_swap[j].second);
        if (delta < best && subject_tabus_[subject][to_swap[j].second] == 0) {
          best = delta;
          lhs_best = i;
          rhs_best = j;
        }
      }
    }
  }
  if (best == 0) return best;
  int subject = GetSubjectOf(to_swap[lhs_best].first, to_swap[lhs_best].second);
  subject_tabus_[subject][to_swap[rhs_best].second] = 1;
  if (timetable_[to_swap[lhs_best].first][to_swap[rhs_best].second] == -1) {
    HardTranslate(to_swap[lhs_best].first, to_swap[lhs_best].second,
                  to_swap[rhs_best].second);
  } else {
    HardSwap(to_swap[lhs_best].first, to_swap[lhs_best].second,
             to_swap[rhs_best].second);
  }
  return best;
}

int Schedule::SoftSolver(const int &time_limit, const double &kappa,
                         const int &tau, const double &alpha) {
  int soft_count = SoftCount();
  std::time_t start = std::time(NULL);
  int loops = 0;
  while (soft_count > 0 && std::difftime(std::time(NULL), start) < time_limit) {
    if (loops >= 1000) {
      soft_count = SoftSimulatedAnnealing(time_limit
                                        - std::difftime(std::time(NULL), start),
                                          kappa, tau, alpha);
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

int Schedule::SoftLocalSearch(const bool &accept_side, const int &threshold) {
  std::vector< std::pair<int, int> > to_swap;
  for (auto ptr : groups_)
    for (auto it = ptr->GetSectionsBegin(); it != ptr->GetSectionsEnd(); it++)
      for (auto jt = 0; jt < num_slots_; jt++)
        to_swap.emplace_back((*it)->GetId(), jt);
  std::shuffle(to_swap.begin(), to_swap.end(), rand_generator_);
  for (std::vector<int>::size_type i = 0; i < to_swap.size(); i++) {
    int section = to_swap[i].first;
    if (timetable_[section][to_swap[i].second] < 0) continue;
    for (std::vector<int>::size_type j = i+1; j < to_swap.size(); j++) {
      if (to_swap[j].first == section) {
        if (timetable_[section][to_swap[j].second] >= 0) {
          if (!IsValidSoftSwap(section, to_swap[i].second, to_swap[j].second))
            continue;
          int delta = SoftCountSwap(section, to_swap[i].second,
                                    to_swap[j].second);
          if (delta < 0 ||
              (delta == 0 && accept_side) ||
              (delta > 0 && delta < threshold)) {
            SoftSwap(section, to_swap[i].second, to_swap[j].second);
            return delta;
          }
        } else if (timetable_[section][to_swap[j].second] == -1) {
          if (!IsValidSoftTranslate(section, to_swap[i].second,
                                    to_swap[j].second)) continue;
          int delta = SoftCountTranslate(section, to_swap[i].second,
                                         to_swap[j].second);
          if (delta < 0 ||
              (delta == 0 && accept_side) ||
              (delta > 0 && delta < threshold)) {
            SoftTranslate(section, to_swap[i].second, to_swap[j].second);
            return delta;
          }
        }
      }
    }
  }
  return 0;
}

int Schedule::SoftSimulatedAnnealing(const int &time_limit, const double &kappa,
                                     const int &tau, const double &alpha) {
  double initial_temperature = kappa * SimulatedAnnealingSample(100);
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

double Schedule::SimulatedAnnealingSample(const int &num_samples) {
  int samples = 0, total = 0;
  std::vector< std::pair<int, int> > to_swap;
  for (auto ptr : groups_)
    for (auto it = ptr->GetSectionsBegin(); it != ptr->GetSectionsEnd(); it++)
      for (auto jt = 0; jt < num_slots_; jt++)
        to_swap.emplace_back((*it)->GetId(), jt);
  std::shuffle(to_swap.begin(), to_swap.end(), rand_generator_);
  for (std::vector<int>::size_type i = 0; i < to_swap.size(); i++) {
    int section = to_swap[i].first;
    if (timetable_[section][to_swap[i].second] < 0) continue;
    for (std::vector<int>::size_type j = i+1; j < to_swap.size(); j++) {
      if (to_swap[j].first == section) {
        if (timetable_[section][to_swap[j].second] >= 0) {
          if (!IsValidSoftSwap(section, to_swap[i].second, to_swap[j].second))
            continue;
          samples++;
          total += SoftCountSwap(section, to_swap[i].second, to_swap[j].second);
          if (samples >= num_samples) break;
        } else if (timetable_[section][to_swap[j].second] == -1) {
          if (!IsValidSoftTranslate(section, to_swap[i].second,
                                    to_swap[j].second)) continue;
          samples++;
          total += SoftCountTranslate(section, to_swap[i].second,
                                      to_swap[j].second);
          if (samples >= num_samples) break;
        }
      }
    }
    if (samples >= num_samples) break;
  }
  return double(total) / double(samples);
}

int Schedule::SimulatedAnnealingSearch(const double &temperature) {
  std::uniform_real_distribution<double> prob(0, 1);
  std::vector< std::pair<int, int> > to_swap;
  for (auto ptr : groups_)
    for (auto it = ptr->GetSectionsBegin(); it != ptr->GetSectionsEnd(); it++)
      for (auto jt = 0; jt < num_slots_; jt++)
        to_swap.emplace_back((*it)->GetId(), jt);
  std::shuffle(to_swap.begin(), to_swap.end(), rand_generator_);
  for (std::vector<int>::size_type i = 0; i < to_swap.size(); i++) {
    int section = to_swap[i].first;
    if (timetable_[section][to_swap[i].second] < 0) continue;
    for (std::vector<int>::size_type j = i+1; j < to_swap.size(); j++) {
      if (to_swap[j].first == section) {
        if (timetable_[section][to_swap[j].second] >= 0) {
          if (!IsValidSoftSwap(section, to_swap[i].second, to_swap[j].second))
            continue;
          int delta = SoftCountSwap(section, to_swap[i].second,
                                    to_swap[j].second);
          if (delta <= 0 ||
              prob(rand_generator_) < exp(-double(delta) / temperature)) {
            SoftSwap(section, to_swap[i].second, to_swap[j].second);
            return delta;
          }
        } else if (timetable_[section][to_swap[j].second] == -1) {
          if (!IsValidSoftTranslate(section, to_swap[i].second,
                                    to_swap[j].second)) continue;
          int delta = SoftCountTranslate(section, to_swap[i].second,
                                         to_swap[j].second);
          if (delta <= 0 ||
              prob(rand_generator_) < exp(-double(delta) / temperature)) {
            SoftTranslate(section, to_swap[i].second, to_swap[j].second);
            return delta;
          }
        }
      }
    }
  }
  return 0;
}

void Schedule::Solve(const int &time_limit, const int &attempts) {
  for (int i = 0; i < attempts; i++) {
    std::time_t start = std::time(NULL);
    Initialize();
    InitialSchedule();
    HardSolver(time_limit);
    SoftInitialize();
    int soft_count = SoftSolver(time_limit
                              - std::difftime(std::time(NULL), start) + 1);
    if (soft_count < best_soft_count_) {
      best_soft_count_ = soft_count;
      best_time_table_ = timetable_;
      best_teacher_table_ = teacher_table_;
    }
  }
  timetable_ = best_time_table_;
  teacher_table_ = best_teacher_table_;
}

void Schedule::TestPrint() {
  std::cout << "  ";
  for (std::vector<int>::size_type i = 0; i < sections_.size(); i++)
    std::cout << (i%10) << "   ";
  std::cout << std::endl;
  for (int j = 0; j < num_slots_; j++) {
    std::cout << (j%10);
    for (std::vector<int>::size_type it = 0; it < sections_.size(); it++) {
      auto jt = timetable_[it][j];
      if (jt >= 0) std::cout << ' ' << GetSubject(jt)->GetName();
      else if (jt == -1) std::cout << "    ";
      else std::cout << " ---"; 
    }
    std::cout << std::endl;
    if (j % num_slots_per_day_ == num_slots_per_day_ - 1 && j != num_slots_-1) {
      std::cout << std::endl;
      std::cout << "  ";
      for (std::vector<int>::size_type i = 0; i < sections_.size(); i++)
        std::cout << (i%10) << "   ";
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;

  std::cout << "  ";
  for (std::vector<int>::size_type i = 0; i < teachers_.size(); i++)
    std::cout << (i%10) << " ";
  std::cout << std::endl;
  for (int j = 0; j < num_slots_; j++) {
    std::cout << (j%10);
    for (std::vector<int>::size_type it = 0; it < teachers_.size(); it++) {
      auto jt = teacher_table_[it][j];
      if (jt >= 0) std::cout << ' ' << jt;
      else std::cout << "  ";
    }
    std::cout << std::endl;
    if (j % num_slots_per_day_ == num_slots_per_day_ - 1 && j != num_slots_-1) {
      std::cout << std::endl;
      std::cout << "  ";
      for (std::vector<int>::size_type i = 0; i < teachers_.size(); i++)
        std::cout << (i%10) << " ";
      std::cout << std::endl;
    }
  }

  std::cout << std::endl;
  if (!hard_satisfied_) std::cout << HardCount() << std::endl;
  else std::cout << SoftCount() << std::endl;
  std::cout << std::endl;
}