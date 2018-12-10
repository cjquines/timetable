#include <cassert>

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "schedule.h"
#include "group.h"
#include "subject.h"
#include "teacher.h"

Schedule::Schedule(int num_days, int num_slots_per_day)
    : num_days_(num_days), num_slots_per_day_(num_slots_per_day),
      hard_satisfied_(false) {
  num_slots_ = num_days_ * num_slots_per_day_;
}

int Schedule::GetNumDays() { return num_days_; }

int Schedule::GetNumSlots() { return num_slots_; }

int Schedule::GetNumSlotsPerDay() { return num_slots_per_day_; }

Group* Schedule::GetGroup(int idx) { return groups_[idx]; }

const std::vector<Group*>& Schedule::GetGroups() const { return groups_; }

Section* Schedule::GetSection(int idx) { return sections_[idx]; }

const std::vector<Section*>& Schedule::GetSections() const { return sections_; }

Subject* Schedule::GetSubject(int idx) { return subjects_[idx]; }

const std::vector<Subject*>& Schedule::GetSubjects() const { return subjects_; }

Teacher* Schedule::GetTeacher(int idx) { return teachers_[idx]; }

const std::vector<Teacher*>& Schedule::GetTeachers() const { return teachers_; }

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
}

void Schedule::Initialize() {
  for (auto ptr : groups_) {
    for (const auto &it : ptr->GetSections()) sections_.push_back(it);
    for (const auto &it : ptr->GetSubjects()) subjects_.push_back(it);
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
  return (tr_timeslot <= timeslot && timeslot < tr_timeslot + length)
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

bool Schedule::IsValidHardAdjSwap(int section, int lhs_timeslot,
                                  int rhs_timeslot) {
  // Returns True if the section's subject with head at lhs_timeslot can be
  // hard adj swapped to subject with head at rhs_timeslot. 
  assert(timetable_[section][lhs_timeslot] >= 0);
  assert(timetable_[section][rhs_timeslot] >= 0);
  if (rhs_timeslot < lhs_timeslot) return false;
  int lhs_length = GetLengthOf(section, lhs_timeslot);
  return (ClampDay(lhs_timeslot).first == ClampDay(rhs_timeslot).first)
       && IsFree(section, lhs_timeslot + lhs_length,
                 rhs_timeslot - (lhs_timeslot + lhs_length));
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

bool Schedule::IsValidSoftAdjSwap(int section, int lhs_timeslot,
                                  int rhs_timeslot) {
  return IsValidHardAdjSwap(section, lhs_timeslot, rhs_timeslot) &&
         (HardCountAdjSwap(section, lhs_timeslot, rhs_timeslot) == 0);
}

std::pair<int, int> Schedule::ClampDay(int timeslot) {
  // Returns [ltimeslot, rtimeslot) for the day timeslot is in.
  int lbound = timeslot / num_slots_per_day_ * num_slots_per_day_;
  return std::make_pair(lbound, lbound + num_slots_per_day_);
}

int Schedule::NewRHSSlot(int section, int lhs_timeslot, int rhs_timeslot) {
  return rhs_timeslot + GetLengthOf(section, rhs_timeslot)
       - GetLengthOf(section, lhs_timeslot);
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

int Schedule::HardCountAdjSwap(int section, int lhs_timeslot,
                               int rhs_timeslot) {
  int result = 0;
  for (auto& ptr : hard_constraints_)
    result += ptr->CountAdjSwap(section, lhs_timeslot, rhs_timeslot);
  return result;
}

void Schedule::HardAdjSwap(int section, int lhs_timeslot, int rhs_timeslot) {
  int lhs_subject = GetSubjectOf(section, lhs_timeslot);
  int rhs_subject = GetSubjectOf(section, rhs_timeslot);
  int lhs_teacher = GetSubject(lhs_subject)->GetTeacher();
  int rhs_teacher = GetSubject(rhs_subject)->GetTeacher();
  int lhs_length = GetLengthOf(section, lhs_timeslot);
  int rhs_length = GetLengthOf(section, rhs_timeslot);
  int new_rhs_slot = NewRHSSlot(section, lhs_timeslot, rhs_timeslot);

  for (int i = 0; i < lhs_length; i++) {
    teacher_table_[lhs_teacher][lhs_timeslot+i]--;
    timetable_[section][lhs_timeslot+i] = -1;
  }

  for (int i = 0; i < rhs_length; i++) {
    teacher_table_[rhs_teacher][rhs_timeslot+i]--;
    timetable_[section][rhs_timeslot+i] = -1;
  }

  for (int i = 0; i < lhs_length; i++) {
    teacher_table_[lhs_teacher][new_rhs_slot+i]++;
    timetable_[section][new_rhs_slot+i] = -2;
  }

  for (int i = 0; i < rhs_length; i++) {
    teacher_table_[rhs_teacher][lhs_timeslot+i]++;
    timetable_[section][lhs_timeslot+i] = -2;
  }

  timetable_[section][lhs_timeslot] = rhs_subject;
  timetable_[section][new_rhs_slot] = lhs_subject;
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

int Schedule::SoftCountAdjSwap(int section, int lhs_timeslot,
                                            int rhs_timeslot) {
  int result = 0;
  for (auto& ptr : soft_constraints_)
    result += ptr->CountAdjSwap(section, lhs_timeslot, rhs_timeslot);
  return result;
}

void Schedule::SoftAdjSwap(int section, int lhs_timeslot, int rhs_timeslot) {
  int lhs_subject = GetSubjectOf(section, lhs_timeslot);
  int rhs_subject = GetSubjectOf(section, rhs_timeslot);
  int lhs_teacher = GetSubject(lhs_subject)->GetTeacher();
  int rhs_teacher = GetSubject(rhs_subject)->GetTeacher();
  int lhs_length = GetLengthOf(section, lhs_timeslot);
  int rhs_length = GetLengthOf(section, rhs_timeslot);
  int new_rhs_slot = NewRHSSlot(section, lhs_timeslot, rhs_timeslot);

  for (int i = 0; i < lhs_length; i++) {
    teacher_table_[lhs_teacher][lhs_timeslot+i] = -1;
    timetable_[section][lhs_timeslot+i] = -1;
  }

  for (int i = 0; i < rhs_length; i++) {
    teacher_table_[rhs_teacher][rhs_timeslot+i] = -1;
    timetable_[section][rhs_timeslot+i] = -1;
  }

  for (int i = 0; i < lhs_length; i++) {
    teacher_table_[lhs_teacher][new_rhs_slot+i] = section;
    timetable_[section][new_rhs_slot+i] = -2;
  }

  for (int i = 0; i < rhs_length; i++) {
    teacher_table_[rhs_teacher][lhs_timeslot+i] = section;
    timetable_[section][lhs_timeslot+i] = -2;
  }

  timetable_[section][lhs_timeslot] = rhs_subject;
  timetable_[section][new_rhs_slot] = lhs_subject;
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