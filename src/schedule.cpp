#include <cassert>

#include <algorithm>
#include <iostream>
#include <random>

#include "schedule.h"
#include "group.h"
#include "subject.h"
#include "teacher.h"
#include "constraints/distinctperday.h"
#include "constraints/nonsimultaneous.h"
#include "constraints/teachertime.h"

Schedule::Schedule(const int &num_days, const int &num_slots_per_day,
                   const int &seed)
    : num_days_(num_days), num_slots_per_day_(num_slots_per_day),
      hard_satisfied_(false), rand_generator_(seed) {
  num_slots_ = num_days_ * num_slots_per_day_;
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

void Schedule::AddTeacherTime(const int &priority, const int &teacher,
                              const std::vector<int> &unassignable) {
  std::unique_ptr<Constraint> ptr = std::make_unique<TeacherTime>(
    this, priority, teacher, unassignable);
  constraints_.push_back(std::move(ptr));
}

void Schedule::Initialize() {
  for (auto ptr : groups_) {
    for (auto it = ptr->GetSectionsBegin(); it != ptr->GetSectionsEnd(); it++)
      sections_.push_back(*it);
    for (auto it = ptr->GetSubjectsBegin(); it != ptr->GetSubjectsEnd(); it++)
      subjects_.push_back(*it);
  }

  std::unique_ptr<Constraint> ptr = std::make_unique<DistinctPerDay>(this);
  constraints_.push_back(std::move(ptr));
  ptr = std::make_unique<NonSimultaneous>(this);
  constraints_.push_back(std::move(ptr));

  timetable_.assign(sections_.size(), std::vector<int>(num_slots_, -1));
  teacher_table_.assign(teachers_.size(), std::vector<int>(num_slots_, 0));
}

void Schedule::SoftInitialize() {
  assert(HardCount() == 0);
  hard_satisfied_ = true;
  teacher_table_.assign(teachers_.size(), std::vector<int>(num_slots_, -1));
  for (int it = 0; it < sections_.size(); it++)
    for (int jt = 0; jt < num_slots_; jt++)
      if (timetable_[it][jt] != -1)
        teacher_table_[GetTeacherOf(it, jt)][jt] = it;
}

int Schedule::GetSubjectOf(const int &section, const int &timeslot) {
  while (timetable_[section][timeslot] == -2) {
    timeslot--;
    assert(timeslot >= 0);
  }
  return timetable_[section][timeslot];
}

int Schedule::GetTeacherOf(const int &section, const int &timeslot) {
  assert(GetSubjectOf(section, timeslot) != -1);
  return GetSubject(GetSubjectOf(section, timeslot))->GetTeacher();
}

int Schedule::GetLengthOf(const int &section, const int &timeslot) {
  int lbound = timeslot, rbound = timeslot;
  while (timetable_[section][lbound] == -2) {
    lbound--;
    assert(lbound >= 0);
  }
  while (timetable_[section][rbound] == -2 && rbound < num_slots_) rbound++;
  return rbound - lbound;
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

std::pair<int, int> Schedule::ClampDay(const int &timeslot) {
  // Returns [ltimeslot, rtimeslot) for the day timeslot is in.
  int lbound = timeslot / num_slots_per_day_ * num_slots_per_day_;
  return std::make_pair(lbound, lbound + num_slots_per_day_);
}

int Schedule::HardCountAssign(const int &subject, const int &section,
                              const int &timeslot) {
  int result = 0;
  for (auto& ptr : constraints_)
    if (ptr->GetPriority() <= 0)
      result += ptr->CountAssign(subject, section, timeslot);
  return result;
}

void Schedule::HardAssign(const int &subject, const int &section,
                          const int &timeslot) {
  timetable_[section][timeslot] = subject;
  teacher_table_[GetSubject(subject)->GetTeacher()][timeslot]++;
}

int Schedule::HardCountTranslate(const int &section, const int &timeslot,
                                 const int &open_timeslot) {
  int result = 0;
  for (auto& ptr : constraints_)
    if (ptr->GetPriority() <= 0)
      result += ptr->CountTranslate(section, timeslot, open_timeslot);
  return result;
}

void Schedule::HardTranslate(const int &section, const int &timeslot,
                             const int &open_timeslot) {
  int subject = GetSubjectOf(section, timeslot);
  timetable_[section][timeslot] = -1;
  timetable_[section][open_timeslot] = subject;
  teacher_table_[GetSubject(subject)->GetTeacher()][timeslot]--;
  teacher_table_[GetSubject(subject)->GetTeacher()][open_timeslot]++;
}

int Schedule::HardCountSwap(const int &section, const int &lhs_timeslot,
                            const int &rhs_timeslot) {
  int result = 0;
  for (auto& ptr : constraints_)
    if (ptr->GetPriority() <= 0)
      result += ptr->CountSwapTimeslot(section, lhs_timeslot, rhs_timeslot);
  return result;
}

void Schedule::HardSwap(const int &section, const int &lhs_timeslot,
                        const int &rhs_timeslot) {
  int lhs_subject = GetSubjectOf(section, lhs_timeslot);
  int rhs_subject = GetSubjectOf(section, rhs_timeslot);
  timetable_[section][lhs_timeslot] = rhs_subject;
  timetable_[section][rhs_timeslot] = lhs_subject;
  teacher_table_[GetSubject(lhs_subject)->GetTeacher()][lhs_timeslot]--;
  teacher_table_[GetSubject(lhs_subject)->GetTeacher()][rhs_timeslot]++;
  teacher_table_[GetSubject(rhs_subject)->GetTeacher()][rhs_timeslot]--;
  teacher_table_[GetSubject(rhs_subject)->GetTeacher()][lhs_timeslot]++;
}

int Schedule::SoftCountTranslate(const int &section, const int &timeslot,
                                 const int &open_timeslot) {
  int result = 0;
  for (auto& ptr : constraints_)
    if (ptr->GetPriority() > 0)
      result += ptr->CountTranslate(section, timeslot, open_timeslot);
  return result;
}

void Schedule::SoftTranslate(const int &section, const int &timeslot,
                             const int &open_timeslot) {
  int subject = GetSubjectOf(section, timeslot);
  timetable_[section][timeslot] = -1;
  timetable_[section][open_timeslot] = subject;
  teacher_table_[GetSubject(subject)->GetTeacher()][timeslot] = -1;
  teacher_table_[GetSubject(subject)->GetTeacher()][open_timeslot] = section;
}

int Schedule::SoftCountSwap(const int &section, const int &lhs_timeslot,
                            const int &rhs_timeslot) {
  int result = 0;
  for (auto& ptr : constraints_)
    if (ptr->GetPriority() > 0)
      result += ptr->CountSwapTimeslot(section, lhs_timeslot, rhs_timeslot);
  return result;
}

void Schedule::SoftSwap(const int &section, const int &lhs_timeslot,
                        const int &rhs_timeslot) {
  int lhs_subject = GetSubjectOf(section, lhs_timeslot);
  int rhs_subject = GetSubjectOf(section, rhs_timeslot);
  timetable_[section][lhs_timeslot] = rhs_subject;
  timetable_[section][rhs_timeslot] = lhs_subject;
  teacher_table_[GetSubject(lhs_subject)->GetTeacher()][lhs_timeslot] = -1;
  teacher_table_[GetSubject(rhs_subject)->GetTeacher()][lhs_timeslot] = section;
  teacher_table_[GetSubject(rhs_subject)->GetTeacher()][rhs_timeslot] = -1;
  teacher_table_[GetSubject(lhs_subject)->GetTeacher()][rhs_timeslot] = section;
}

int Schedule::HardCount() {
  int result = 0;
  for (auto& ptr : constraints_)
    if (ptr->GetPriority() <= 0)
      result += ptr->CountAll();
  return result;
}

int Schedule::SoftCount() {
  int result = 0;
  for (auto& ptr : constraints_)
    if (ptr->GetPriority() > 0)
      result += ptr->CountAll();
  return result;
}

void Schedule::InitialSchedule() {
  for (auto ptr : groups_) {
    for (auto it = ptr->GetSectionsBegin(); it != ptr->GetSectionsEnd(); it++) {
      std::vector<int> unassigned;
      for (auto jt = ptr->GetSubjectsBegin(); jt != ptr->GetSubjectsEnd(); jt++)
        for (int kt = 0; kt < (*jt)->GetNumSlots(); kt++)
          unassigned.push_back((*jt)->GetId());
      std::shuffle(unassigned.begin(), unassigned.end(), rand_generator_);
      for (auto jt : unassigned) {
        int min = 1000000, min_index = -1;
        for (int kt = 0; kt < num_slots_; kt++) {
          if (timetable_[(*it)->GetId()][kt] == -1 &&
              HardCountAssign(jt, (*it)->GetId(), kt) < min) {
            min = HardCountAssign(jt, (*it)->GetId(), kt);
            min_index = kt;
          }
        }
        HardAssign(jt, (*it)->GetId(), min_index);
      }
    }
  }
}

int Schedule::HardSolver() {
  int result = HardLocalSearch();
  int delta;
  do {
    delta = HardTabuSearch();
    result += delta;
  } while (delta > 0);
  return result;
}

int Schedule::HardLocalSearch() {
  std::vector< std::pair<int, int> > to_swap;
  for (auto ptr : groups_)
    for (auto it = ptr->GetSectionsBegin(); it != ptr->GetSectionsEnd(); it++)
      for (auto jt = 0; jt < num_slots_; jt++)
        to_swap.emplace_back((*it)->GetId(), jt);
  std::shuffle(to_swap.begin(), to_swap.end(), rand_generator_);
  int result = 0;
  for (int i = 0; i < to_swap.size(); i++) {
    int section = to_swap[i].first;
    if (timetable_[section][to_swap[i].second] == -1) continue;
    bool to_continue = false;
    for (int j = i+1; j < to_swap.size(); j++) {
      if (to_swap[j].first == section) {
        if (timetable_[section][to_swap[j].second] == -1) {
          int delta = HardCountTranslate(section, to_swap[i].second,
                                         to_swap[j].second);
          if (delta <= 0) {
            HardTranslate(section, to_swap[i].second, to_swap[j].second);
            result += delta;
            to_continue = true;
            break;
          }
        } else {
          int delta = HardCountSwap(section, to_swap[i].second,
                                    to_swap[j].second);
          if (delta <= 0) {
            HardSwap(section, to_swap[i].second, to_swap[j].second);
            result += delta;
            to_continue = true;
            break;
          }
        }
      }
    }
    if (to_continue) continue;
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
  for (int i = 0; i < to_swap.size(); i++) {
    int section = to_swap[i].first;
    if (timetable_[section][to_swap[i].second] == -1) continue;
    for (int j = i+1; j < to_swap.size(); j++) {
      if (to_swap[j].first == section) {
        int delta;
        if (timetable_[section][to_swap[j].second] == -1)
          delta = HardCountTranslate(section, to_swap[i].second,
                                     to_swap[j].second);
        else
          delta = HardCountSwap(section, to_swap[i].second, to_swap[j].second);
        if (delta < best) {
          best = delta;
          lhs_best = i;
          rhs_best = j;
        }
      }
    }
  }
  if (best == 0) return best;
  else if (timetable_[to_swap[lhs_best].first][to_swap[rhs_best].second] == -1)
    HardTranslate(to_swap[lhs_best].first, to_swap[lhs_best].second,
                  to_swap[rhs_best].second);
  else
    HardSwap(to_swap[lhs_best].first, to_swap[lhs_best].second,
             to_swap[rhs_best].second);
  return best;
}

int Schedule::SoftSolver() {
  std::vector< std::pair<int, int> > to_swap;
  for (auto ptr : groups_)
    for (auto it = ptr->GetSectionsBegin(); it != ptr->GetSectionsEnd(); it++)
      for (auto jt = 0; jt < num_slots_; jt++)
        to_swap.emplace_back((*it)->GetId(), jt);
  std::shuffle(to_swap.begin(), to_swap.end(), rand_generator_);
  for (int i = 0; i < to_swap.size(); i++) {
    int section = to_swap[i].first;
    if (timetable_[section][to_swap[i].second] == -1) continue;
    for (int j = i+1; j < to_swap.size(); j++) {
      if (to_swap[j].first == section) {
        if (timetable_[section][to_swap[j].second] == -1) {
          if (HardCountTranslate(section, to_swap[i].second,
                                 to_swap[j].second) > 0) continue;
          int delta = SoftCountTranslate(section, to_swap[i].second,
                                         to_swap[j].second);
          if (delta <= 0) {
            SoftTranslate(section, to_swap[i].second, to_swap[j].second);
            return delta;
          }
        } else {
          if (HardCountSwap(section, to_swap[i].second,
                            to_swap[j].second) > 0) continue;
          int delta = SoftCountSwap(section, to_swap[i].second,
                                    to_swap[j].second);
          if (delta <= 0) {
            SoftSwap(section, to_swap[i].second, to_swap[j].second);
            return delta;
          }
        }
      }
    }
  }
}

void Schedule::TestPrint() {
  std::cout << "   ";
  for (int i = 0; i < num_slots_; i++) std::cout << "x   ";
  std::cout << std::endl;
  for (int it = 0; it < sections_.size(); it++) {
    std::cout << it << ' ';
    for (auto jt : timetable_[it]) {
      if (jt >= 0) std::cout << ' ' << GetSubject(jt)->GetName();
      else std::cout << " ---";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  for (int it = 0; it < teachers_.size(); it++) {
    std::cout << it << ' ';
    for (auto jt : teacher_table_[it]) {
      if (jt >= 0) std::cout << ' ' << jt;
      else std::cout << " -";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  if (!hard_satisfied_) std::cout << HardCount() << std::endl;
  else std::cout << SoftCount() << std::endl;
  std::cout << std::endl;
}