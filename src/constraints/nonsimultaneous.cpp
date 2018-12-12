#include "nonsimultaneous.h"
#include "../schedule.h"
#include "../teacher.h"

NonSimultaneous::NonSimultaneous(Schedule* schedule, int priority)
    : Constraint(schedule, priority) {}

int NonSimultaneous::HalfCount(int section, int lhs_timeslot,
                               int rhs_timeslot) {
  int teacher = schedule_->GetTeacherOf(section, lhs_timeslot);
  if (teacher == 0) return 0;
  int length = schedule_->GetLengthOf(section, lhs_timeslot);
  int result = 0;
  for (int i = 0; i < length; i++) {
    if (schedule_->CountSectionsOf(teacher, lhs_timeslot+i) > 1) result--;
    if (schedule_->CountSectionsOf(teacher, rhs_timeslot+i) > 0) result++;
  }
  return result;
}

int NonSimultaneous::CountTranslate(int section, int timeslot,
                                    int open_timeslot) {
  int result = HalfCount(section, timeslot, open_timeslot);

  if (priority_ > 0) return result*priority_;
  return result;
}

int NonSimultaneous::CountSwapTimeslot(int section, int lhs_timeslot,
                                       int rhs_timeslot) {
  return CountAdjSwap(section, lhs_timeslot, rhs_timeslot);
}

int NonSimultaneous::CountAdjSwap(int section, int lhs_timeslot,
                                  int rhs_timeslot) {
  int new_rhs_slot = schedule_->NewRHSSlot(section, lhs_timeslot, rhs_timeslot);
  int result = HalfCount(section, lhs_timeslot, new_rhs_slot)
             + HalfCount(section, rhs_timeslot, lhs_timeslot);

  if (priority_ > 0) return result*priority_;
  return result;
}

int NonSimultaneous::CountAll() {
  int result = 0;
  
  for (const auto &it : schedule_->GetTeachers()) {
    if (it->GetId() == 0) continue;
    for (int j = 0; j < schedule_->GetNumSlots(); j++) {
      result += std::max(0, schedule_->CountSectionsOf(it->GetId(), j) - 1);
    }
  }

  if (priority_ > 0) return result*priority_;
  return result;
}