#include "minsubjects.h"
#include "../schedule.h"
#include "../section.h"

MinSubjects::MinSubjects(Schedule* schedule, const int &priority,
                         const int &min_subjects)
    : Constraint(schedule, priority), min_subjects_(min_subjects) {}

int MinSubjects::CountTranslate(const int &section, const int &timeslot,
                                const int &open_timeslot) {
  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = schedule_->ClampDay(open_timeslot);
  int result = 0;

  int num_subjects = 0;
  for (int j = lbound; j < rbound; j++)
    if (schedule_->GetSubjectOf(section, j) >= 0)
      num_subjects++;
  if (num_subjects == min_subjects_) result++;

  num_subjects = 0;
  for (int j = open_lbound; j < open_rbound; j++)
    if (schedule_->GetSubjectOf(section, j) >= 0)
      num_subjects++;
  if (num_subjects == min_subjects_ - 1) result--;

  if (priority_ > 0) return result*priority_;
  return result;
}

int MinSubjects::CountAll() {
  int result = 0;
  for (auto it = schedule_->GetSectionsBegin();
       it != schedule_->GetSectionsEnd(); it++) {
    for (int i = 0; i < schedule_->GetNumDays(); i++) {
      int num_subjects = 0;
      for (int j = i*schedule_->GetNumSlotsPerDay();
           j < (i+1)*schedule_->GetNumSlotsPerDay(); j++)
        if (schedule_->GetSubjectOf((*it)->GetId(), j) >= 0)
          num_subjects++;
      if (num_subjects < min_subjects_) result++;
    }
  }

  if (priority_ > 0) return result*priority_;
  return result;
}