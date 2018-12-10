#include "nonsimultaneous.h"
#include "../schedule.h"
#include "../teacher.h"

NonSimultaneous::NonSimultaneous(Schedule* schedule, int priority)
    : Constraint(schedule, priority) {}

int NonSimultaneous::CountTranslate(int section, int timeslot,
                                    int open_timeslot) {
  int teacher = schedule_->GetTeacherOf(section, timeslot);
  if (teacher == 0) return 0;
  int length = schedule_->GetLengthOf(section, timeslot);
  int result = 0;

  for (int i = 0; i < length; i++) {
    if (schedule_->CountSectionsOf(teacher, timeslot+i) > 1) result--;
    if (schedule_->CountSectionsOf(teacher, open_timeslot+i) > 0) result++;
  }

  if (priority_ > 0) return result*priority_;
  return result;
}

int NonSimultaneous::CountSwapTimeslot(int section, int lhs_timeslot,
                                       int rhs_timeslot) {
  return CountAdjSwap(section, lhs_timeslot, rhs_timeslot);
}

int NonSimultaneous::CountAdjSwap(int section, int lhs_timeslot,
                                  int rhs_timeslot) {
  int lhs_teacher = schedule_->GetTeacherOf(section, lhs_timeslot);
  int rhs_teacher = schedule_->GetTeacherOf(section, rhs_timeslot);
  if (lhs_teacher == rhs_teacher) return 0;
  int lhs_length = schedule_->GetLengthOf(section, lhs_timeslot);
  int rhs_length = schedule_->GetLengthOf(section, rhs_timeslot);
  int new_rhs_slot = schedule_->NewRHSSlot(section, lhs_timeslot, rhs_timeslot);
  int result = 0;

  if (lhs_teacher != 0) {
    for (int i = 0; i < lhs_length; i++) {
      if (schedule_->CountSectionsOf(lhs_teacher, lhs_timeslot+i) > 1) result--;
      if (schedule_->CountSectionsOf(lhs_teacher, new_rhs_slot+i) > 0) result++;
    }
  }

  if (rhs_teacher != 0) {
    for (int i = 0; i < rhs_length; i++) {
      if (schedule_->CountSectionsOf(rhs_teacher, rhs_timeslot+i) > 1) result--;
      if (schedule_->CountSectionsOf(rhs_teacher, lhs_timeslot+i) > 0) result++;
    }
  }

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