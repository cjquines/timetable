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
  else return (unassignable_[open_timeslot]-unassignable_[timeslot])*
              Constraint::priority_;
}

int TeacherTime::CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                                   const int &rhs_timeslot) {
  if (Constraint::schedule_->GetTeacherOf(section, lhs_timeslot) == teacher_)
    return (unassignable_[rhs_timeslot]-unassignable_[lhs_timeslot])*
           Constraint::priority_;
  else if
    (Constraint::schedule_->GetTeacherOf(section, rhs_timeslot) == teacher_)
    return (unassignable_[lhs_timeslot]-unassignable_[rhs_timeslot])*
           Constraint::priority_;
  else return 0;
}

int TeacherTime::CountAll() {
  int result = 0;
  for (int i = 0; i < Constraint::schedule_->GetNumSlots(); i++)
    if (unassignable_[i] &&
        Constraint::schedule_->GetSectionOf(teacher_, i) != -1) result++;
  return result;
}