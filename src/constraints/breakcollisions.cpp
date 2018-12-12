#include "breakcollisions.h"
#include "../schedule.h"

BreakCollisions::BreakCollisions(Schedule* schedule, int priority,
                                 int max_collisions)
    : Constraint(schedule, priority), max_collisions_(max_collisions) {}

int BreakCollisions::Score(int lbound, int rbound, int section, int timeslot,
                           int open_timeslot) {
  int result = 0;
  for (int i = lbound; i < rbound; i++) {
    int count = 0;
    if (section == -1) count = schedule_->CountSectionsOf(0, i);
    else count = schedule_->CountSectionsTranslate(0, i, section, timeslot,
                                                   open_timeslot);
    if (count > max_collisions_) result += count - max_collisions_;
  }
  return result;
}

int BreakCollisions::CountTranslate(int section, int timeslot,
                                    int open_timeslot) {
  if (schedule_->GetTeacherOf(section, timeslot) != 0) return 0;
  int length = schedule_->GetLengthOf(section, timeslot);
  int result = Score(timeslot, timeslot + length, section, timeslot,
                     open_timeslot)
             - Score(timeslot, timeslot + length)
             + Score(open_timeslot, open_timeslot + length, section, timeslot,
                     open_timeslot)
             - Score(open_timeslot, open_timeslot + length);

  if (priority_ > 0) return result*priority_;
  return result;
}

int BreakCollisions::CountSwapTimeslot(int section, int lhs_timeslot,
                                       int rhs_timeslot) {
  return CountAdjSwap(section, lhs_timeslot, rhs_timeslot);
}

int BreakCollisions::CountAdjSwap(int section, int lhs_timeslot,
                                  int rhs_timeslot) {
  int lhs_teacher = schedule_->GetTeacherOf(section, lhs_timeslot);
  int rhs_teacher = schedule_->GetTeacherOf(section, rhs_timeslot);
  if (lhs_teacher != 0 && rhs_teacher != 0) return 0;
  int lhs_length = schedule_->GetLengthOf(section, lhs_timeslot);
  int rhs_length = schedule_->GetLengthOf(section, rhs_timeslot);
  if (lhs_teacher == rhs_teacher && lhs_length == rhs_length) return 0;
  int new_rhs_slot = schedule_->NewRHSSlot(section, lhs_timeslot, rhs_timeslot);

  return CountTranslate(section, lhs_timeslot, new_rhs_slot)
       + CountTranslate(section, rhs_timeslot, lhs_timeslot);
}

int BreakCollisions::CountAll() {
  int result = Score(0, schedule_->GetNumSlots());

  if (priority_ > 0) return result*priority_;
  return result;
}