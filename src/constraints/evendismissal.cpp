#include <algorithm>
#include <vector>

#include "evendismissal.h"
#include "../schedule.h"

EvenDismissal::EvenDismissal(Schedule* schedule, int priority,
                             const std::vector<int> &sections)
    : Constraint(schedule, priority), sections_(sections) {
  std::sort(sections_.begin(), sections_.end());
}

int EvenDismissal::Dismissal(int section, int lbound, int rbound) {
  int result = 0;
  for (int i = rbound - 1; i >= lbound && schedule_->IsFree(section, i); i--)
    result++;
  return result;
}

int EvenDismissal::DismissalTranslate(int section, int lbound, int rbound,
                                      int timeslot, int open_timeslot) {
  int result = 0;
  for (int i = rbound - 1; i >= lbound
    && schedule_->IsFreeTranslate(i, section, timeslot, open_timeslot); i--)
    result++;
  return result;
}

int EvenDismissal::HalfCountTranslate(int section, int lbound, int rbound,
                                      int timeslot, int open_timeslot) {
  int sum_hours = 0;
  for (auto it : sections_) sum_hours += Dismissal(it, lbound, rbound);
  int old_last = Dismissal(section, lbound, rbound);
  int delta = DismissalTranslate(section, lbound, rbound, timeslot,
                                 open_timeslot) - old_last;
  return sections_.size() * (2*old_last*delta + delta*delta) - 2*sum_hours*delta
       - delta*delta;
}

int EvenDismissal::CountTranslate(int section, int timeslot,
                                  int open_timeslot) {
  if (!std::binary_search(sections_.begin(), sections_.end(), section))
    return 0;

  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = schedule_->ClampDay(open_timeslot);

  int result = HalfCountTranslate(section, lbound, rbound, timeslot,
                                  open_timeslot)
             + HalfCountTranslate(section, open_lbound, open_rbound, timeslot,
                                  open_timeslot);

  if (lbound == open_lbound) result /= 2;
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
      int dismissal = Dismissal(it, i*schedule_->GetNumSlotsPerDay(),
                                (i + 1)*schedule_->GetNumSlotsPerDay());
      squared_sum[i] += dismissal*dismissal;
      sum_hours[i] += dismissal;
    }
  }

  for (int i = 0; i < schedule_->GetNumDays(); i++)
    result += num_sections*squared_sum[i] - sum_hours[i]*sum_hours[i];

  if (priority_ > 0) return result*priority_;
  return result;
}