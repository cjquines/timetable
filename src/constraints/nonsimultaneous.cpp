#include "nonsimultaneous.h"
#include "../schedule.h"
#include "../teacher.h"

NonSimultaneous::NonSimultaneous(Schedule* schedule, const int &priority)
    : Constraint(schedule, priority) {}

int NonSimultaneous::CountTranslate(const int &section, const int &timeslot,
                                    const int &open_timeslot) {
  int teacher = schedule_->GetTeacherOf(section, timeslot);
  int length = schedule_->GetLengthOf(section, timeslot);
  int result = 0;

  for (int i = 0; i < length; i++) {
    if (schedule_->CountSectionsOf(teacher, timeslot+i) > 1) result--;
    if (schedule_->CountSectionsOf(teacher, open_timeslot+i) > 0) result++;
  }

  if (priority_ > 0) return result*priority_;
  return result;
}

int NonSimultaneous::CountSwapTimeslot(const int &section,
                                       const int &lhs_timeslot,
                                       const int &rhs_timeslot) {
  int lhs_teacher = schedule_->GetTeacherOf(section, lhs_timeslot);
  int rhs_teacher = schedule_->GetTeacherOf(section, rhs_timeslot);
  if (lhs_teacher == rhs_teacher) return 0;
  int length = schedule_->GetLengthOf(section, lhs_timeslot);
  int result = 0;

  for (int i = 0; i < length; i++) {
    if (schedule_->CountSectionsOf(lhs_teacher, lhs_timeslot+i) > 1) result--;
    if (schedule_->CountSectionsOf(lhs_teacher, rhs_timeslot+i) > 0) result++;
    if (schedule_->CountSectionsOf(rhs_teacher, rhs_timeslot+i) > 1) result--;
    if (schedule_->CountSectionsOf(rhs_teacher, lhs_timeslot+i) > 0) result++;
  }

  if (priority_ > 0) return result*priority_;
  return result;
}

int NonSimultaneous::CountAll() {
  int result = 0;
  
  for (const auto &it : schedule_->GetTeachers()) {
    for (int j = 0; j < schedule_->GetNumSlots(); j++) {
      result += std::max(0, schedule_->CountSectionsOf(it->GetId(), j) - 1);
    }
  }

  if (priority_ > 0) return result*priority_;
  return result;
}