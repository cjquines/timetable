#include <algorithm>
#include <vector>

#include "evendismissal.h"
#include "../schedule.h"
#include "../section.h"

EvenDismissal::EvenDismissal(Schedule* schedule, const int &priority,
                             const std::vector<int> &sections)
    : Constraint(schedule, priority), sections_(sections) {}

int EvenDismissal::CountTranslate(const int &section, const int &timeslot,
                                  const int &open_timeslot) {
  if (std::find(sections_.begin(), sections_.end(), section) == sections_.end())
    return 0;

  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = schedule_->ClampDay(open_timeslot);
  int length = schedule_->GetLengthOf(section, timeslot);
  int old_last, new_last, delta, day, result = 0;
  int num_sections = sections_.size();

  std::vector<int> sum_hours(schedule_->GetNumDays(), 0);
  for (auto it : sections_) {
    for (int i = 0; i < schedule_->GetNumDays(); i++) {
      int j = (i + 1)*schedule_->GetNumSlotsPerDay() - 1;
      for (; j >= i*schedule_->GetNumSlotsPerDay(); j--)
        if (schedule_->GetSubjectOf(it, j) != -1)
          break;
      j = (i + 1)*schedule_->GetNumSlotsPerDay() - 1 - j;
      sum_hours[i] += j;
    }
  }

  if (lbound == open_lbound) {
    old_last = 0; new_last = 0;
    for (int j = rbound - 1; j >= lbound; j--) {
      if (schedule_->GetSubjectOf(section, j) != -1) break;
      old_last++;
    }
    for (int j = rbound - 1; j >= lbound; j--) {
      if (timeslot <= j && j < timeslot + length) new_last++;
      else if (schedule_->GetSubjectOf(section, j) != -1) break;
      else if (open_timeslot <= j && j < open_timeslot + length) break;
      else new_last++;
    }
    delta = new_last - old_last;
    day = lbound / schedule_->GetNumSlotsPerDay();
    result = num_sections * (2*old_last*delta + delta*delta)
           - 2*sum_hours[day]*delta
           - delta*delta;
  } else {
    old_last = 0; new_last = 0;
    for (int j = rbound - 1; j >= lbound; j--) {
      if (schedule_->GetSubjectOf(section, j) != -1) break;
      old_last++;
    }
    for (int j = rbound - 1; j >= lbound; j--) {
      if (timeslot <= j && j < timeslot + length) new_last++;
      else if (schedule_->GetSubjectOf(section, j) != -1) break;
      else new_last++;
    }
    delta = new_last - old_last;
    day = lbound / schedule_->GetNumSlotsPerDay();
    result += num_sections * (2*old_last*delta + delta*delta)
            - 2*sum_hours[day]*delta
            - delta*delta;

    old_last = 0; new_last = 0;
    for (int j = open_rbound - 1; j >= open_lbound; j--) {
      if (schedule_->GetSubjectOf(section, j) != -1) break;
      old_last++;
    }
    for (int j = open_rbound - 1; j >= open_lbound; j--) {
      if (schedule_->GetSubjectOf(section, j) != -1) break;
      if (open_timeslot <= j && j < open_timeslot + length) break;
      new_last++;
    }
    delta = new_last - old_last;
    day = open_lbound / schedule_->GetNumSlotsPerDay();
    result += num_sections * (2*old_last*delta + delta*delta)
            - 2*sum_hours[day]*delta
            - delta*delta;
  }

  if (priority_ > 0) return result*priority_;
  return result;
}

int EvenDismissal::CountAll() {
  int num_sections = sections_.size();
  std::vector<int> squared_sum(schedule_->GetNumDays(), 0);
  std::vector<int> sum_hours(schedule_->GetNumDays(), 0);
  int result = 0;

  for (auto it : sections_) {
    for (int i = 0; i < schedule_->GetNumDays(); i++) {
      int j = (i + 1)*schedule_->GetNumSlotsPerDay() - 1;
      for (; j >= i*schedule_->GetNumSlotsPerDay(); j--)
        if (schedule_->GetSubjectOf(it, j) != -1)
          break;
      j = (i + 1)*schedule_->GetNumSlotsPerDay() - 1 - j;
      squared_sum[i] += j*j;
      sum_hours[i] += j;
    }
  }

  for (int i = 0; i < schedule_->GetNumDays(); i++)
    result += num_sections*squared_sum[i] - sum_hours[i]*sum_hours[i];

  if (priority_ > 0) return result*priority_;
  return result;
}