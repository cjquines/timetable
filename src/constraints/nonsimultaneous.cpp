#include "nonsimultaneous.h"
#include "../schedule.h"
#include "../subject.h"
#include "../teacher.h"

NonSimultaneous::NonSimultaneous(Schedule* schedule, const int &priority)
    : Constraint(schedule, priority) {}

int NonSimultaneous::CountTranslate(const int &section, const int &timeslot,
                                    const int &open_timeslot) {
  int teacher = Constraint::schedule_->GetTeacherOf(section, timeslot);
  int num_slots = Constraint::schedule_->GetLengthOf(section, timeslot);
  int result = 0;

  for (int i = 0; i < num_slots; i++) {
    if (Constraint::schedule_->CountSectionsOf(teacher, open_timeslot+i) > 0) result++;
    if (Constraint::schedule_->CountSectionsOf(teacher, timeslot+i) > 1) result--;
  }

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}

int NonSimultaneous::CountSwapTimeslot(const int &section,
                                       const int &lhs_timeslot,
                                       const int &rhs_timeslot) {
  int lhs_teacher = Constraint::schedule_->GetTeacherOf(section, lhs_timeslot);
  int rhs_teacher = Constraint::schedule_->GetTeacherOf(section, rhs_timeslot);
  if (lhs_teacher == rhs_teacher) return 0;
  int num_slots = Constraint::schedule_->GetLengthOf(section, lhs_timeslot);
  int result = 0;

  for (int i = 0; i < num_slots; i++) {
    if (Constraint::schedule_->CountSectionsOf(lhs_teacher, rhs_timeslot+i) > 0) result++;
    if (Constraint::schedule_->CountSectionsOf(lhs_teacher, lhs_timeslot+i) > 1) result--;
    if (Constraint::schedule_->CountSectionsOf(rhs_teacher, lhs_timeslot+i) > 0) result++;
    if (Constraint::schedule_->CountSectionsOf(rhs_teacher, rhs_timeslot+i) > 1) result--;
  }

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}

int NonSimultaneous::CountAll() {
  int result = 0;
  
  for (auto it = Constraint::schedule_->GetTeachersBegin();
       it != Constraint::schedule_->GetTeachersEnd(); it++)
    for (int j = 0; j < Constraint::schedule_->GetNumSlots(); j++)
      result += std::max(0,
        Constraint::schedule_->CountSectionsOf((*it)->GetId(), j) - 1);

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}