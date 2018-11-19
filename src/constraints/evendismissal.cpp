#include <vector>

#include "evendismissal.h"
#include "../schedule.h"
#include "../section.h"

EvenDismissal::EvenDismissal(Schedule* schedule, const int &priority)
    : Constraint(schedule, priority) {
      num_sections_ = std::distance(Constraint::schedule_->GetSectionsBegin(),
                                    Constraint::schedule_->GetSectionsEnd());
      sum_hours_.assign(Constraint::schedule_->GetNumDays(), 0);
    }

int EvenDismissal::CountTranslate(const int &section, const int &timeslot,
                                  const int &open_timeslot) {
  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = Constraint::schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = Constraint::schedule_->ClampDay(open_timeslot);
  int length = Constraint::schedule_->GetLengthOf(section, timeslot);
  int result = 0;

  if (lbound == open_lbound) {
    int old_last = 0, new_last = 0;
    for (int j = rbound - 1; j >= lbound; j--) {
      if (Constraint::schedule_->GetSubjectOf(section, j) != -1) break;
      old_last++;
    }
    for (int j = rbound - 1; j >= lbound; j--) {
      if (timeslot <= j && j <= timeslot + length) continue;
      if (Constraint::schedule_->GetSubjectOf(section, j) != -1) break;
      if (open_timeslot <= j && j <= open_timeslot + length) break;
      new_last++;
    }
    int delta = new_last - old_last;
    return (num_sections_ * (2*old_last*delta + delta*delta)
         - 2*sum_hours_[lbound / Constraint::schedule_->GetNumSlotsPerDay()]*delta
         - delta*delta)*Constraint::priority_;
  }

  // find lbound delta
  int old_last = 0, new_last = 0;
  for (int j = rbound - 1; j >= lbound; j--) {
    if (Constraint::schedule_->GetSubjectOf(section, j) != -1) break;
    old_last++;
  }
  for (int j = rbound - 1; j >= lbound; j--) {
    if (timeslot <= j && j <= timeslot + length) continue;
    if (Constraint::schedule_->GetSubjectOf(section, j) != -1) break;
    new_last++;
  }
  int delta = new_last - old_last;
  result += num_sections_ * (2*old_last*delta + delta*delta)
          - 2*sum_hours_[lbound / Constraint::schedule_->GetNumSlotsPerDay()]*delta
          - delta*delta;

  // find open_lbound delta
  old_last = 0; new_last = 0;
  for (int j = open_rbound - 1; j >= open_lbound; j--) {
    if (Constraint::schedule_->GetSubjectOf(section, j) != -1) break;
    old_last++;
  }
  for (int j = open_rbound - 1; j >= open_lbound; j--) {
    if (Constraint::schedule_->GetSubjectOf(section, j) != -1) break;
    if (open_timeslot <= j && j <= open_timeslot + length) break;
    new_last++;
  }
  delta = new_last - old_last;
  result += num_sections_ * (2*old_last*delta + delta*delta)
          - 2*sum_hours_[open_lbound / Constraint::schedule_->GetNumSlotsPerDay()]*delta
          - delta*delta;

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  else return result;
}

int EvenDismissal::CountAll() {
  std::vector<int> squared_sum(Constraint::schedule_->GetNumDays(), 0);
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
      sum_hours_[i] += j;
    }
  }

  for (int i = 0; i < Constraint::schedule_->GetNumDays(); i++) {
    result += num_sections_*squared_sum[i] - sum_hours_[i];
  }

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  else return result;
}