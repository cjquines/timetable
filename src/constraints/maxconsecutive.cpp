#include "maxconsecutive.h"
#include "../schedule.h"
#include "../teacher.h"

MaxConsecutive::MaxConsecutive(Schedule* schedule, const int &priority,
                               const int &max_consecutive)
    : Constraint(schedule, priority), max_consecutive_(max_consecutive) {}

int MaxConsecutive::CountTranslate(const int &section, const int &timeslot,
                                   const int &open_timeslot) {
  int teacher = schedule_->GetTeacherOf(section, timeslot);
  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = schedule_->ClampDay(open_timeslot);
  int length = schedule_->GetLengthOf(section, timeslot);
  int result = 0;

  bool seen_filled = false;
  int num_filled = 0;
  for (int i = lbound; i < rbound; i++) {
    if (seen_filled) {
      if (schedule_->CountSectionsOf(teacher, i) == 0) {
        if (num_filled > max_consecutive_)
          result += max_consecutive_ - num_filled;
        seen_filled = false;
      } else num_filled++;
    } else if (schedule_->CountSectionsOf(teacher, i) > 0) {
      seen_filled = true;
      num_filled = 1;
    }
  }
  if (seen_filled && num_filled > max_consecutive_)
    result += max_consecutive_ - num_filled;

  seen_filled = false;
  num_filled = 0;
  for (int i = open_lbound; i < open_rbound; i++) {
    if (seen_filled) {
      if (schedule_->CountSectionsOf(teacher, i) == 0) {
        if (num_filled > max_consecutive_)
          result += max_consecutive_ - num_filled;
        seen_filled = false;
      } else num_filled++;
    } else if (schedule_->CountSectionsOf(teacher, i) > 0) {
      seen_filled = true;
      num_filled = 1;
    }
  }
  if (seen_filled && num_filled > max_consecutive_)
    result += max_consecutive_ - num_filled;

  seen_filled = false;
  num_filled = 0;
  for (int i = lbound; i < rbound; i++) {
    bool empty = (timeslot <= i && i < timeslot + length
                  && schedule_->CountSectionsOf(teacher, i) == 1)
                 || (schedule_->CountSectionsOf(teacher, i) == 0
                     && !(open_timeslot <= i && i < open_timeslot + length));
    if (seen_filled) {
      if (empty) {
        if (num_filled > max_consecutive_)
          result += num_filled - max_consecutive_;
        seen_filled = false;
      } else num_filled++;
    } else if (!empty) {
      seen_filled = true;
      num_filled = 1;
    }
  }
  if (seen_filled && num_filled > max_consecutive_)
    result += num_filled - max_consecutive_;

  seen_filled = false;
  num_filled = 0;
  for (int i = open_lbound; i < open_rbound; i++) {
    bool empty = (timeslot <= i && i < timeslot + length
                  && schedule_->CountSectionsOf(teacher, i) == 1)
                 || (schedule_->CountSectionsOf(teacher, i) == 0
                     && !(open_timeslot <= i && i < open_timeslot + length));
    if (seen_filled) {
      if (empty) {
        if (num_filled > max_consecutive_)
          result += num_filled - max_consecutive_;
        seen_filled = false;
      } else num_filled++;
    } else if (!empty) {
      seen_filled = true;
      num_filled = 1;
    }
  }
  if (seen_filled && num_filled > max_consecutive_)
    result += num_filled - max_consecutive_;

  if (lbound == open_lbound) result /= 2;
  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}

int MaxConsecutive::CountSwapTimeslot(const int &section,
                                      const int &lhs_timeslot,
                                      const int &rhs_timeslot) {
  int lhs_teacher = schedule_->GetTeacherOf(section, lhs_timeslot);
  int rhs_teacher = schedule_->GetTeacherOf(section, rhs_timeslot);
  if (lhs_teacher == rhs_teacher) return 0;

  return CountTranslate(section, lhs_timeslot, rhs_timeslot)
       + CountTranslate(section, rhs_timeslot, lhs_timeslot);
}

int MaxConsecutive::CountAll() {
  int result = 0;
  for (auto it = schedule_->GetTeachersBegin();
       it != schedule_->GetTeachersEnd(); it++) {
    for (int i = 0; i < schedule_->GetNumDays(); i++) {
      bool seen_filled = false;
      int num_filled = 0;
      for (int j = i*schedule_->GetNumSlotsPerDay();
           j < (i+1)*schedule_->GetNumSlotsPerDay(); j++) {
        if (seen_filled) {
          if (schedule_->CountSectionsOf((*it)->GetId(), j) == 0) {
            if (num_filled > max_consecutive_)
              result += num_filled - max_consecutive_;
            seen_filled = false;
          } else num_filled++;
        } else if (schedule_->CountSectionsOf((*it)->GetId(), j) > 0) {
          seen_filled = true;
          num_filled = 1;
        }
      }
      if (seen_filled && num_filled > max_consecutive_)
        result += num_filled - max_consecutive_;
    }
  }

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}