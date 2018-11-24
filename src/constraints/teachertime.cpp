#include <vector>

#include "teachertime.h"
#include "../schedule.h"

TeacherTime::TeacherTime(Schedule *schedule, const int &priority,
                         const int &teacher, const std::vector<int> &unassignable)
    : Constraint(schedule, priority), teacher_(teacher),
      unassignable_(schedule_->GetNumSlots(), 0) {
  for (auto i : unassignable) unassignable_[i] = 1;
}

int TeacherTime::HalfCount(const int &section, const int &lhs_timeslot,
                           const int &rhs_timeslot) {
  int length = schedule_->GetLengthOf(section, lhs_timeslot);
  int result = 0;
  for (int i = 0; i < length; i++) {
    result += unassignable_[rhs_timeslot+i] - unassignable_[lhs_timeslot+i];
  }
  return result;
}

int TeacherTime::CountTranslate(const int &section, const int &timeslot,
                                const int &open_timeslot) {
  if (schedule_->GetTeacherOf(section, timeslot) != teacher_) return 0;
  int result = HalfCount(section, timeslot, open_timeslot);

  if (priority_ > 0) return result*priority_;
  return result;
}

int TeacherTime::CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                                   const int &rhs_timeslot) {
  int result = 0;
  if (schedule_->GetTeacherOf(section, lhs_timeslot) == teacher_)
    result += HalfCount(section, lhs_timeslot, rhs_timeslot);
  if (schedule_->GetTeacherOf(section, rhs_timeslot) == teacher_)
    result += HalfCount(section, rhs_timeslot, lhs_timeslot);

  if (priority_ > 0) return result*priority_;
  return result;
}

int TeacherTime::CountAll() {
  int result = 0;
  for (int i = 0; i < schedule_->GetNumSlots(); i++) {
    if (unassignable_[i] && schedule_->CountSectionsOf(teacher_, i) > 0)
      result += schedule_->CountSectionsOf(teacher_, i);
  }

  if (priority_ > 0) return result*priority_;
  return result;
}