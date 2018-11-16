#include "subjectgaps.h"
#include "../schedule.h"
#include "../section.h"

#include <iostream>

SubjectGaps::SubjectGaps(Schedule* schedule, const int &priority)
    : Constraint(schedule, priority) {}

int SubjectGaps::CountTranslate(const int &section, const int &timeslot,
                                const int &open_timeslot) {
  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = Constraint::schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = Constraint::schedule_->ClampDay(open_timeslot);
  int length = Constraint::schedule_->GetLengthOf(section, timeslot);
  int result = 0;

  if (lbound == open_lbound && Constraint::schedule_->IsFree(section,
    std::min(timeslot, open_timeslot) + length, std::abs(timeslot - open_timeslot) - length)) {
    int small = timeslot, large = open_timeslot;
    if (small > large) std::swap(small, large);
    bool open_left = Constraint::schedule_->IsFree(section, lbound, small - lbound);
    bool open_right = Constraint::schedule_->IsFree(section, large + length, rbound - large - length);
    if (open_left) {
      if (open_right) return 0;
      return timeslot - open_timeslot;
    } else {
      if (!open_right) return 0;
      return open_timeslot - timeslot;
    }
  }

  if (!Constraint::schedule_->IsFree(section, lbound, timeslot - lbound)) {
    if (Constraint::schedule_->IsFree(section, timeslot + length,
                                      rbound - timeslot - length)) {
      int num_empty = 0;
      for (int j = timeslot - 1; j >= lbound; j--) {
        if (Constraint::schedule_->GetSubjectOf(section, j) != -1) {
          result -= num_empty;
          break;
        } else num_empty++;
      }
    } else result += length;
  } else {
    int num_empty = 0;
    for (int j = timeslot + length; j < rbound; j++) {
      if (Constraint::schedule_->GetSubjectOf(section, j) != -1) {
        result -= num_empty;
        break;
      } else num_empty++;
    }
  }

  if (!Constraint::schedule_->IsFree(section, open_lbound,
                                     open_timeslot - open_lbound)) {
    if (Constraint::schedule_->IsFree(section, open_timeslot + length,
                                      open_rbound - open_timeslot - length)) {
      int num_empty = 0;
      for (int j = open_timeslot - 1; j >= open_lbound; j--) {
        if (Constraint::schedule_->GetSubjectOf(section, j) != -1) {
          result += num_empty;
          break;
        } else num_empty++;
      }
    } else result -= length;
  } else {
    int num_empty = 0;
    for (int j = open_timeslot + length; j < open_rbound; j++) {
      if (Constraint::schedule_->GetSubjectOf(section, j) != -1) {
        result += num_empty;
        break;
      } else num_empty++;
    }
  }

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  else return result;
}

int SubjectGaps::CountAll() {
  int result = 0;
  for (auto it = Constraint::schedule_->GetSectionsBegin();
       it != Constraint::schedule_->GetSectionsEnd(); it++)
    for (int i = 0; i < Constraint::schedule_->GetNumDays(); i++) {
      int j = i*Constraint::schedule_->GetNumSlotsPerDay();
      for (; j < (i+1)*Constraint::schedule_->GetNumSlotsPerDay(); j++)
        if (Constraint::schedule_->GetSubjectOf((*it)->GetId(), j) != -1)
          break;
      bool seen_empty = false;
      int num_empty = 0;
      for (; j < (i+1)*Constraint::schedule_->GetNumSlotsPerDay(); j++) {
        if (seen_empty) {
          if (Constraint::schedule_->GetSubjectOf((*it)->GetId(), j) != -1) {
            result += num_empty;
            seen_empty = false;
          } else num_empty++;
        } else if (Constraint::schedule_->GetSubjectOf((*it)->GetId(), j) == -1)
            seen_empty = true, num_empty = 1;
      }
    }
    
  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  else return result;
}