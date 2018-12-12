#ifndef _TIMETABLE_CONSTRAINTS_SLOTSBETWEEN_H
#define _TIMETABLE_CONSTRAINTS_SLOTSBETWEEN_H

#include <utility>

#include "constraint.h"

class SlotsBetween : public Constraint {
public:
  SlotsBetween(Schedule* schedule, int priority, int min_slots, int max_slots);
  int CountTranslate(int section, int timeslot, int open_timeslot) override;
  int CountSwapTimeslot(int section, int lhs_timeslot,
                        int rhs_timeslot) override;
  int CountAdjSwap(int section, int lhs_timeslot, int rhs_timeslot) override;
  int CountAll() override;

private:
  int min_slots_, max_slots_;

  int Score(int section, int lbound, int rbound,
            std::pair<int, int> add_break = {-1, 0},
            std::pair<int, int> rm_break = {-1, 0},
            std::pair<int, int> add_subj = {-1, 0},
            std::pair<int, int> rm_subj = {-1, 0});
};

#endif