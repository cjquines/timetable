#include "minsubjects.h"
#include "../schedule.h"
#include "../section.h"

MinSubjects::MinSubjects(Schedule* schedule, const int &priority,
                         const int &min_subjects)
    : Constraint(schedule, priority), min_subjects_(min_subjects) {}

int MinSubjects::CountSubjects(const int &section, const int &lbound,
                               const int &rbound) {
  int num_subjects = 0;
  for (int i = lbound; i < rbound; i++) {
    if (!schedule_->IsFree(section, i)) num_subjects++;
  }
  return num_subjects;
}

int MinSubjects::CountTranslate(const int &section, const int &timeslot,
                                const int &open_timeslot) {
  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = schedule_->ClampDay(open_timeslot);

  int result = 
      (CountSubjects(section, lbound, rbound) == min_subjects_)
    - (CountSubjects(section, open_lbound, open_rbound) == min_subjects_ - 1);

  if (priority_ > 0) return result*priority_;
  return result;
}

int MinSubjects::CountAll() {
  int result = 0;
  for (const auto &it : schedule_->GetSections()) {
    for (int i = 0; i < schedule_->GetNumDays(); i++) {
      if (CountSubjects(it->GetId(), i*schedule_->GetNumSlotsPerDay(),
                        (i+1)*schedule_->GetNumSlotsPerDay() < min_subjects_))
        result++;
    }
  }

  if (priority_ > 0) return result*priority_;
  return result;
}