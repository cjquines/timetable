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

int NonSimultaneous::CountAll() {
  int result = 0;
  for (auto it = Constraint::schedule_->GetSectionsBegin();
       it != Constraint::schedule_->GetSectionsEnd(); it++) {
    for (int i = 0; i < Constraint::schedule_->num_days_; i++) {
      std::set<int> seen_subjects;
      for (int j = i*Constraint::schedule_->num_slots_per_day;
           j < (i+1)*Constraint::schedule_->num_slots_per_day; j++) {
        if (!seen_subjects.insert(
            Constraint::schedule_->GetSubjectOf(it, j)).second) result++;
      }
    }
  }
  return result;
}