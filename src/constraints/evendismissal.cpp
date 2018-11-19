#include <vector>

#include "evendismissal.h"
#include "../schedule.h"
#include "../section.h"

EvenDismissal::EvenDismissal(Schedule* schedule, const int &priority)
    : Constraint(schedule, priority) {}

int EvenDismissal::CountTranslate(const int &section, const int &timeslot,
                                  const int &open_timeslot) {
  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = Constraint::schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = Constraint::schedule_->ClampDay(open_timeslot);
  int length = Constraint::schedule_->GetLengthOf(section, timeslot);
  int old_last, new_last, delta, day, result = 0;
  int num_sections = std::distance(Constraint::schedule_->GetSectionsBegin(),
                                   Constraint::schedule_->GetSectionsEnd());

  std::vector<int> sum_hours(Constraint::schedule_->GetNumDays(), 0);
  for (auto it = Constraint::schedule_->GetSectionsBegin();
       it != Constraint::schedule_->GetSectionsEnd(); it++) {
    for (int i = 0; i < Constraint::schedule_->GetNumDays(); i++) {
      int j = (i + 1)*Constraint::schedule_->GetNumSlotsPerDay() - 1;
      for (; j >= i*Constraint::schedule_->GetNumSlotsPerDay(); j--)
        if (Constraint::schedule_->GetSubjectOf((*it)->GetId(), j) != -1)
          break;
      j = (i + 1)*Constraint::schedule_->GetNumSlotsPerDay() - 1 - j;
      sum_hours[i] += j;
    }
  }

  if (lbound == open_lbound) {
    old_last = 0; new_last = 0;
    for (int j = rbound - 1; j >= lbound; j--) {
      if (Constraint::schedule_->GetSubjectOf(section, j) != -1) break;
      old_last++;
    }
    for (int j = rbound - 1; j >= lbound; j--) {
      if (timeslot <= j && j < timeslot + length) new_last++;
      else if (Constraint::schedule_->GetSubjectOf(section, j) != -1) break;
      else if (open_timeslot <= j && j < open_timeslot + length) break;
      else new_last++;
    }
    delta = new_last - old_last;
    day = lbound / Constraint::schedule_->GetNumSlotsPerDay();
    result = num_sections * (2*old_last*delta + delta*delta)
           - 2*sum_hours[day]*delta
           - delta*delta;
  } else {
    old_last = 0; new_last = 0;
    for (int j = rbound - 1; j >= lbound; j--) {
      if (Constraint::schedule_->GetSubjectOf(section, j) != -1) break;
      old_last++;
    }
    for (int j = rbound - 1; j >= lbound; j--) {
      if (timeslot <= j && j < timeslot + length) new_last++;
      else if (Constraint::schedule_->GetSubjectOf(section, j) != -1) break;
      else new_last++;
    }
    delta = new_last - old_last;
    day = lbound / Constraint::schedule_->GetNumSlotsPerDay();
    result += num_sections * (2*old_last*delta + delta*delta)
            - 2*sum_hours[day]*delta
            - delta*delta;

    old_last = 0; new_last = 0;
    for (int j = open_rbound - 1; j >= open_lbound; j--) {
      if (Constraint::schedule_->GetSubjectOf(section, j) != -1) break;
      old_last++;
    }
    for (int j = open_rbound - 1; j >= open_lbound; j--) {
      if (Constraint::schedule_->GetSubjectOf(section, j) != -1) break;
      if (open_timeslot <= j && j < open_timeslot + length) break;
      new_last++;
    }
    delta = new_last - old_last;
    day = open_lbound / Constraint::schedule_->GetNumSlotsPerDay();
    result += num_sections * (2*old_last*delta + delta*delta)
            - 2*sum_hours[day]*delta
            - delta*delta;
  }

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}

int EvenDismissal::CountAll() {
  int num_sections = std::distance(Constraint::schedule_->GetSectionsBegin(),
                                   Constraint::schedule_->GetSectionsEnd());
  std::vector<int> squared_sum(Constraint::schedule_->GetNumDays(), 0);
  std::vector<int> sum_hours(Constraint::schedule_->GetNumDays(), 0);
  int result = 0;

  for (auto it = Constraint::schedule_->GetSectionsBegin();
       it != Constraint::schedule_->GetSectionsEnd(); it++) {
    for (int i = 0; i < Constraint::schedule_->GetNumDays(); i++) {
      int j = (i + 1)*Constraint::schedule_->GetNumSlotsPerDay() - 1;
      for (; j >= i*Constraint::schedule_->GetNumSlotsPerDay(); j--)
        if (Constraint::schedule_->GetSubjectOf((*it)->GetId(), j) != -1)
          break;
      j = (i + 1)*Constraint::schedule_->GetNumSlotsPerDay() - 1 - j;
      squared_sum[i] += j*j;
      sum_hours[i] += j;
    }
  }

  for (int i = 0; i < Constraint::schedule_->GetNumDays(); i++)
    result += num_sections*squared_sum[i] - sum_hours[i]*sum_hours[i];

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}