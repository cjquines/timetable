#include "maxconsecutive.h"
#include "../schedule.h"
#include "../teacher.h"

#include <cassert>
#include <iostream>

MaxConsecutive::MaxConsecutive(Schedule* schedule, int priority,
                               int max_consecutive)
    : Constraint(schedule, priority), max_consecutive_(max_consecutive) {}

int MaxConsecutive::Consecutive(int teacher, int lbound, int rbound,
                                int section, int timeslot,
                                int open_timeslot) {
  bool seen_filled = false;
  int num_filled = 0, result = 0;
  for (int i = lbound; i < rbound; i++) {
    int count;
    if (section == -1) count = schedule_->CountSectionsOf(teacher, i);
    else count = schedule_->CountSectionsTranslate(teacher, i, section,
                                                   timeslot, open_timeslot);
    if (seen_filled) {
      if (count == 0) {
        if (num_filled > max_consecutive_)
          result += num_filled - max_consecutive_;
        seen_filled = false;
      } else num_filled++;
    } else if (count > 0) {
      seen_filled = true;
      num_filled = 1;
    }
  }
  if (seen_filled && num_filled > max_consecutive_)
    result += num_filled - max_consecutive_;
  return result;
}

int MaxConsecutive::CountTranslate(int section, int timeslot,
                                   int open_timeslot) {
  int teacher = schedule_->GetTeacherOf(section, timeslot);
  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = schedule_->ClampDay(open_timeslot);

  int result = Consecutive(teacher, lbound, rbound, section, timeslot,
                           open_timeslot)
             - Consecutive(teacher, lbound, rbound)
             + Consecutive(teacher, open_lbound, open_rbound, section, timeslot,
                           open_timeslot)
             - Consecutive(teacher, open_lbound, open_rbound);

  if (lbound == open_lbound) result /= 2;
  if (priority_ > 0) return result*priority_;
  return result;
}

int MaxConsecutive::CountSwapTimeslot(int section, int lhs_timeslot,
                                      int rhs_timeslot) {
  return CountAdjSwap(section, lhs_timeslot, rhs_timeslot);
}

int MaxConsecutive::CountAdjSwap(int section, int lhs_timeslot,
                                 int rhs_timeslot) {
  int new_rhs_slot = schedule_->NewRHSSlot(section, lhs_timeslot, rhs_timeslot);

  return CountTranslate(section, lhs_timeslot, new_rhs_slot)
       + CountTranslate(section, rhs_timeslot, lhs_timeslot);
}

int MaxConsecutive::CountAll() {
  int result = 0;
  for (const auto &it : schedule_->GetTeachers()) {
    for (int i = 0; i < schedule_->GetNumDays(); i++) {
      result += Consecutive(it->GetId(), i*schedule_->GetNumSlotsPerDay(),
                            (i+1)*schedule_->GetNumSlotsPerDay());
    }
  }

  if (priority_ > 0) return result*priority_;
  return result;
}