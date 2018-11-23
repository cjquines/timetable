#include "subjectgaps.h"
#include "../schedule.h"
#include "../section.h"

SubjectGaps::SubjectGaps(Schedule* schedule, const int &priority)
    : Constraint(schedule, priority) {}

int SubjectGaps::CountTranslate(const int &section, const int &timeslot,
                                const int &open_timeslot) {
  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = schedule_->ClampDay(open_timeslot);
  int length = schedule_->GetLengthOf(section, timeslot);
  int result = 0;

  if (lbound == open_lbound && schedule_->IsFree(section,
    std::min(timeslot, open_timeslot) + length, std::abs(timeslot - open_timeslot) - length)) {
    int small = timeslot, large = open_timeslot;
    if (small > large) std::swap(small, large);
    bool open_left = schedule_->IsFree(section, lbound, small - lbound);
    bool open_right = schedule_->IsFree(section, large + length, rbound - large - length);
    if (open_left) {
      if (open_right) return 0;
      if (priority_ > 0)
        return (timeslot - open_timeslot)*priority_;
      return timeslot - open_timeslot;
    } else {
      if (!open_right) return 0;
      if (priority_ > 0)
        return (open_timeslot - timeslot)*priority_;
      return open_timeslot - timeslot;
    }
  }

  if (!schedule_->IsFree(section, lbound, timeslot - lbound)) {
    if (schedule_->IsFree(section, timeslot + length,
                                      rbound - timeslot - length)) {
      int num_empty = 0;
      for (int j = timeslot - 1; j >= lbound; j--) {
        if (schedule_->GetSubjectOf(section, j) != -1) {
          result -= num_empty;
          break;
        } else num_empty++;
      }
    } else result += length;
  } else {
    int num_empty = 0;
    for (int j = timeslot + length; j < rbound; j++) {
      if (schedule_->GetSubjectOf(section, j) != -1) {
        result -= num_empty;
        break;
      } else num_empty++;
    }
  }

  if (!schedule_->IsFree(section, open_lbound,
                                     open_timeslot - open_lbound)) {
    if (schedule_->IsFree(section, open_timeslot + length,
                                      open_rbound - open_timeslot - length)) {
      int num_empty = 0;
      for (int j = open_timeslot - 1; j >= open_lbound; j--) {
        if (schedule_->GetSubjectOf(section, j) != -1) {
          result += num_empty;
          break;
        } else num_empty++;
      }
    } else result -= length;
  } else {
    int num_empty = 0;
    for (int j = open_timeslot + length; j < open_rbound; j++) {
      if (schedule_->GetSubjectOf(section, j) != -1) {
        result += num_empty;
        break;
      } else num_empty++;
    }
  }

  if (priority_ > 0) return result*priority_;
  return result;
}

int SubjectGaps::CountAll() {
  int result = 0;
  for (auto it = schedule_->GetSectionsBegin();
       it != schedule_->GetSectionsEnd(); it++) {
    for (int i = 0; i < schedule_->GetNumDays(); i++) {
      int j = i*schedule_->GetNumSlotsPerDay();
      for (; j < (i+1)*schedule_->GetNumSlotsPerDay(); j++)
        if (schedule_->GetSubjectOf((*it)->GetId(), j) != -1)
          break;
      bool seen_empty = false;
      int num_empty = 0;
      for (; j < (i+1)*schedule_->GetNumSlotsPerDay(); j++) {
        if (seen_empty) {
          if (schedule_->GetSubjectOf((*it)->GetId(), j) != -1) {
            result += num_empty;
            seen_empty = false;
          } else num_empty++;
        } else if (schedule_->GetSubjectOf((*it)->GetId(), j) == -1)
            seen_empty = true, num_empty = 1;
      }
    }
  }

  if (priority_ > 0) return result*priority_;
  return result;
}