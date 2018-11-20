#include "minsubjects.h"
#include "../schedule.h"
#include "../section.h"

MinSubjects::MinSubjects(Schedule* schedule, const int &priority,
                         const int &min_subjects)
    : Constraint(schedule, priority), min_subjects_(min_subjects) {}

int MinSubjects::CountTranslate(const int &section, const int &timeslot,
                                const int &open_timeslot) {
  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = Constraint::schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = Constraint::schedule_->ClampDay(open_timeslot);
  int result = 0;

  int num_subjects = 0;
  for (int j = lbound; j < rbound; j++)
    if (Constraint::schedule_->GetSubjectOf(section, j) >= 0)
      num_subjects++;
  if (num_subjects == min_subjects_) result++;

  num_subjects = 0;
  for (int j = open_lbound; j < open_rbound; j++)
    if (Constraint::schedule_->GetSubjectOf(section, j) >= 0)
      num_subjects++;
  if (num_subjects == min_subjects_ - 1) result--;

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}

int MinSubjects::CountAll() {
  int result = 0;
  for (auto it = Constraint::schedule_->GetSectionsBegin();
       it != Constraint::schedule_->GetSectionsEnd(); it++) {
    for (int i = 0; i < Constraint::schedule_->GetNumDays(); i++) {
      int num_subjects = 0;
      for (int j = i*Constraint::schedule_->GetNumSlotsPerDay();
           j < (i+1)*Constraint::schedule_->GetNumSlotsPerDay(); j++)
        if (Constraint::schedule_->GetSubjectOf((*it)->GetId(), j) >= 0)
          num_subjects++;
      if (num_subjects < min_subjects_) result++;
    }
  }

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}