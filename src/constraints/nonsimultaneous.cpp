#include "nonsimultaneous.h"
#include "../schedule.h"
#include "../subject.h"
#include "../teacher.h"

NonSimultaneous::NonSimultaneous(Schedule* schedule)
    : Constraint(schedule, -1) {}

int NonSimultaneous::CountAssign(const int &subject, const int &section,
                                 const int &timeslot, const int &num_slots) {
  int teacher = Constraint::schedule_->GetSubject(subject)->GetTeacher();
  int result = 0;
  for (int i = 0; i < num_slots; i++)
    result += Constraint::schedule_->CountSectionsOf(teacher, timeslot+i);
  return result;
}

int NonSimultaneous::CountTranslate(const int &section, const int &timeslot,
                                    const int &open_timeslot) {
  int teacher = Constraint::schedule_->GetTeacherOf(section, timeslot);
  int num_slots = Constraint::schedule_->GetLengthOf(section, timeslot);
  int result = 0;
  for (int i = 0; i < num_slots; i++) {
    result += Constraint::schedule_->CountSectionsOf(teacher, open_timeslot+i) + 1;
    result -= Constraint::schedule_->CountSectionsOf(teacher, timeslot+i);
  }
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
    result += Constraint::schedule_->CountSectionsOf(lhs_teacher, rhs_timeslot+i) + 1;
    result -= Constraint::schedule_->CountSectionsOf(lhs_teacher, lhs_timeslot+i);
    result += Constraint::schedule_->CountSectionsOf(rhs_teacher, lhs_timeslot+i) + 1;
    result -= Constraint::schedule_->CountSectionsOf(rhs_teacher, rhs_timeslot+i);
  }
  return result;
}

int NonSimultaneous::CountAll() {
  int result = 0;
  for (auto it = Constraint::schedule_->GetTeachersBegin();
       it != Constraint::schedule_->GetTeachersEnd(); it++)
    for (int j = 0; j < Constraint::schedule_->GetNumSlots(); j++)
      result += std::max(0,
        Constraint::schedule_->CountSectionsOf((*it)->GetId(), j) - 1);
  return result;
}