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
  assert(hard_satisfied_);
  teacher_table_.assign(teachers_.size(), std::vector<int>(num_slots_, -1));
  for (int it = 0; it < sections_.size(); it++)
    for (int jt = 0; jt < num_slots_; jt++)
      if (timetable_[it][jt] != -1)
        teacher_table_[GetTeacherOf(it, jt)][jt] = it;
}

int Schedule::GetSubjectOf(const int &section, const int &timeslot) {
  return timetable_[section][timeslot];
}

int Schedule::GetTeacherOf(const int &section, const int &timeslot) {
  assert(timetable_[section][timeslot] != -1);
  return GetSubject(timetable_[section][timeslot])->GetTeacher();
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

void Schedule::HardSolver(int current) {
  if (current <= 0) {
    hard_satisfied_ = true;
    return;
  }
  for (auto ptr : groups_) {
    std::vector< std::pair<int, int> > to_swap;
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
            int delta = HardCountTranslate(section, to_swap[i].second,
                                           to_swap[j].second);
            std::cout << delta << std::endl;
            if (delta < 0) {
              HardTranslate(section, to_swap[i].second, to_swap[j].second);
              return HardSolver(current + delta);
            }
          } else {
            int delta = HardCountSwap(to_swap[i].first, to_swap[i].second,
                                      to_swap[j].second);
            if (delta < 0) {
              HardSwap(to_swap[i].first, to_swap[i].second, to_swap[j].second);
              return HardSolver(current + delta);
            }
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
  std::cout << HardCount() << std::endl;
  std::cout << std::endl;
}