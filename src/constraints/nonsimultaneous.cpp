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

int NonSimultaneous::CountSwapTimeslot(const int &section,
                                       const int &lhs_timeslot,
                                       const int &rhs_timeslot) {
  int lhs_teacher = Constraint::schedule_->GetTeacherOf(section, lhs_timeslot);
  int rhs_teacher = Constraint::schedule_->GetTeacherOf(section, rhs_timeslot);
  if (lhs_teacher == rhs_teacher) return 0;
  if (Constraint::schedule_->GetSectionOf(lhs_teacher, rhs_timeslot) != -1)
    return 1;
  if (Constraint::schedule_->GetSectionOf(rhs_teacher, lhs_timeslot) != -1)
    return 1;
  return 0;
}