#ifndef _TIMETABLE_CONSTRAINTS_BREAKCOLLISIONS_H
#define _TIMETABLE_CONSTRAINTS_BREAKCOLLISIONS_H

#include "constraint.h"

class BreakCollisions : public Constraint {
public:
  BreakCollisions(Schedule* schedule, int priority, int max_collisions);
  int CountTranslate(int section, int timeslot, int open_timeslot) override;
  int CountSwapTimeslot(int section, int lhs_timeslot,
                        int rhs_timeslot) override;
  int CountAdjSwap(int section, int lhs_timeslot, int rhs_timeslot) override;
  int CountAll() override;

private:
  int max_collisions_;

  int Score(int lbound, int rbound, int section = -1, int timeslot = 0,
            int open_timeslot = 0);
};

#endif