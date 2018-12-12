#include <utility>

#include "slotsbetween.h"
#include "../schedule.h"
#include "../section.h"

SlotsBetween::SlotsBetween(Schedule* schedule, int priority, int min_slots,
                           int max_slots)
    : Constraint(schedule, priority), min_slots_(min_slots),
      max_slots_(max_slots) {}

int SlotsBetween::Score(int section, int lbound, int rbound,
                        std::pair<int, int> add_break,
                        std::pair<int, int> rm_break,
                        std::pair<int, int> add_subj,
                        std::pair<int, int> rm_subj) {
  bool seen_break = false;
  int result = 0, count = 0, i = lbound;
  while (i < rbound) {
    if (i == add_break.first) {
      if (count < min_slots_) result += min_slots_ - count;
      else if (count > max_slots_) result += count - max_slots_;
      count = 0;
      seen_break = true;
      i += add_break.second;
    } else if (i == add_subj.first) {
      count += add_subj.second;
      i += add_subj.second;
    } else if (i == rm_break.first) {
      i += rm_break.second;
    } else if (i == rm_subj.first) {
      i += rm_subj.second;
    } else if (schedule_->GetSubjectOf(section, i) >= 0) {
      if (schedule_->GetTeacherOf(section, i) == 0) {
        if (count < min_slots_) result += min_slots_ - count;
        else if (count > max_slots_) result += count - max_slots_;
        count = 0;
        seen_break = true;
        i += schedule_->GetLengthOf(section, i);
      } else {
        count += schedule_->GetLengthOf(section, i);
        i += schedule_->GetLengthOf(section, i);
      }
    } else i++;
  }
  if (seen_break) {
    if (count < min_slots_) result += min_slots_ - count;
    else if (count > max_slots_) result += count - max_slots_;
  }
  return result;
}

int SlotsBetween::CountTranslate(int section, int timeslot, int open_timeslot) {
  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = schedule_->ClampDay(open_timeslot);
  int length = schedule_->GetLengthOf(section, timeslot);
  int result = - Score(section, lbound, rbound)
               - Score(section, open_lbound, open_rbound);

  if (schedule_->GetTeacherOf(section, timeslot) == 0) {
    result += Score(section, lbound, rbound,
                    {open_timeslot, length}, {timeslot, length})
            + Score(section, open_lbound, open_rbound,
                    {open_timeslot, length}, {timeslot, length});
  } else {
    result += Score(section, lbound, rbound, {-1, 0}, {-1, 0},
                    {open_timeslot, length}, {timeslot, length})
            + Score(section, open_lbound, open_rbound, {-1, 0}, {-1, 0},
                    {open_timeslot, length}, {timeslot, length});
  }

  if (lbound == open_lbound) result /= 2;
  if (priority_ > 0) return result*priority_;
  return result;
}

int SlotsBetween::CountSwapTimeslot(int section, int lhs_timeslot,
                                    int rhs_timeslot) {
  int lhs_teacher = schedule_->GetTeacherOf(section, lhs_timeslot);
  int rhs_teacher = schedule_->GetTeacherOf(section, rhs_timeslot);
  if ((lhs_teacher != 0 && rhs_teacher != 0) || (lhs_teacher == rhs_teacher))
    return 0;

  int lhs_lbound, lhs_rbound, rhs_lbound, rhs_rbound;
  std::tie(lhs_lbound, lhs_rbound) = schedule_->ClampDay(lhs_timeslot);
  std::tie(rhs_lbound, rhs_rbound) = schedule_->ClampDay(rhs_timeslot);
  int length = schedule_->GetLengthOf(section, lhs_timeslot);
  if (rhs_teacher == 0) std::swap(lhs_timeslot, rhs_timeslot);

  int result = Score(section, lhs_lbound, lhs_rbound,
                     {rhs_timeslot, length}, {lhs_timeslot, length},
                     {lhs_timeslot, length}, {rhs_timeslot, length})
             - Score(section, lhs_lbound, lhs_rbound)
             + Score(section, rhs_lbound, rhs_rbound,
                     {rhs_timeslot, length}, {lhs_timeslot, length},
                     {lhs_timeslot, length}, {rhs_timeslot, length})
             - Score(section, rhs_lbound, rhs_rbound);

  if (lhs_lbound == rhs_lbound) result /= 2;
  if (priority_ > 0) return result*priority_;
  return result;
}

int SlotsBetween::CountAdjSwap(int section, int lhs_timeslot,
                               int rhs_timeslot) {
  int lhs_teacher = schedule_->GetTeacherOf(section, lhs_timeslot);
  int rhs_teacher = schedule_->GetTeacherOf(section, rhs_timeslot);
  if ((lhs_teacher != 0 && rhs_teacher != 0) || (lhs_teacher == rhs_teacher))
    return 0;

  int lhs_lbound, lhs_rbound, rhs_lbound, rhs_rbound;
  std::tie(lhs_lbound, lhs_rbound) = schedule_->ClampDay(lhs_timeslot);
  std::tie(rhs_lbound, rhs_rbound) = schedule_->ClampDay(rhs_timeslot);
  int lhs_length = schedule_->GetLengthOf(section, lhs_timeslot);
  int rhs_length = schedule_->GetLengthOf(section, lhs_timeslot);
  int new_rhs_slot = schedule_->NewRHSSlot(section, lhs_timeslot, rhs_timeslot);
  int result = - Score(section, lhs_lbound, lhs_rbound)
               - Score(section, rhs_lbound, rhs_rbound);

  if (lhs_teacher == 0) {
    result += Score(section, lhs_lbound, lhs_rbound,
                    {new_rhs_slot, lhs_length}, {lhs_timeslot, lhs_length},
                    {lhs_timeslot, rhs_length}, {rhs_timeslot, rhs_length})
            + Score(section, rhs_lbound, rhs_rbound,
                    {new_rhs_slot, lhs_length}, {lhs_timeslot, lhs_length},
                    {lhs_timeslot, rhs_length}, {rhs_timeslot, rhs_length});
  } else {
    result += Score(section, lhs_lbound, lhs_rbound,
                    {lhs_timeslot, rhs_length}, {rhs_timeslot, rhs_length},
                    {new_rhs_slot, lhs_length}, {lhs_timeslot, lhs_length})
            + Score(section, rhs_lbound, rhs_rbound,
                    {lhs_timeslot, rhs_length}, {rhs_timeslot, rhs_length},
                    {new_rhs_slot, lhs_length}, {lhs_timeslot, lhs_length});
  }

  if (lhs_lbound == rhs_lbound) result /= 2;
  if (priority_ > 0) return result*priority_;
  return result;
}

int SlotsBetween::CountAll() {
  int result = 0;
  for (const auto &it : schedule_->GetSections())
    for (int i = 0; i < schedule_->GetNumDays(); i++)
      result += Score(it->GetId(), i*schedule_->GetNumSlotsPerDay(),
                      (i + 1)*schedule_->GetNumSlotsPerDay());

  if (priority_ > 0) return result*priority_;
  return result;
}