#include <vector>

#include "teachertime.h"
#include "../schedule.h"

TeacherTime::TeacherTime(Schedule *schedule, const int &priority,
                         const int &teacher, const std::vector<int> &unassignable)
    : Constraint(schedule, priority), teacher_(teacher),
      unassignable_(Constraint::schedule_->GetNumSlots(), 0) {
  for (auto i : unassignable) unassignable_[i] = 1;
}

int TeacherTime::CountTranslate(const int &section, const int &timeslot,
                                const int &open_timeslot) {
  if (Constraint::schedule_->GetTeacherOf(section, timeslot) != teacher_)
    return 0;
  int num_slots = Constraint::schedule_->GetLengthOf(section, timeslot);
  int result = 0;
  for (int i = 0; i < num_slots; i++)
    result += unassignable_[open_timeslot+i] - unassignable_[timeslot+i];
  return result*Constraint::priority_;
}

int TeacherTime::CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                                   const int &rhs_timeslot) {
  int num_slots = Constraint::schedule_->GetLengthOf(section, lhs_timeslot);
  int result = 0;
  if (Constraint::schedule_->GetTeacherOf(section, lhs_timeslot) == teacher_)
    for (int i = 0; i < num_slots; i++)
      result += unassignable_[rhs_timeslot+i] - unassignable_[lhs_timeslot+i];
  if (Constraint::schedule_->GetTeacherOf(section, rhs_timeslot) == teacher_)
    for (int i = 0; i < num_slots; i++)
      result += unassignable_[lhs_timeslot+i] - unassignable_[rhs_timeslot+i];
  return result*Constraint::priority_;
}

int TeacherTime::CountAll() {
  int result = 0;
  for (int i = 0; i < Constraint::schedule_->GetNumSlots(); i++)
    if (unassignable_[i] &&
        Constraint::schedule_->GetSectionOf(teacher_, i) != -1) result++;
  return result*Constraint::priority_;
}