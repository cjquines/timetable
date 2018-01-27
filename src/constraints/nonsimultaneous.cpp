#include "nonsimultaneous.h"
#include "../schedule.h"
#include "../subject.h"

NonSimultaneous::NonSimultaneous(Schedule* schedule)
    : Constraint(schedule, -1) {}

int NonSimultaneous::CountAssign(const int &subject, const int &section,
                                 const int &timeslot) {
  int teacher = Constraint::schedule_->GetSubject(subject)->GetTeacher();
  if (Constraint::schedule_->CountSectionsOf(teacher, timeslot) > 0)
    return 1;
  return 0;
}