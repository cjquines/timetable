#ifndef _TIMETABLE_CONSTRAINTS_MAXCONSECUTIVE_H
#define _TIMETABLE_CONSTRAINTS_MAXCONSECUTIVE_H

#include "constraint.h"

class MaxConsecutive : public Constraint {
public:
  MaxConsecutive(Schedule* schedule, int priority, int max_consecutive);
  int CountTranslate(int section, int timeslot, int open_timeslot) override;
  int CountSwapTimeslot(int section, int lhs_timeslot,
                        int rhs_timeslot) override;
  int CountAll() override;

private:
  int max_consecutive_;

  int Consecutive(int teacher, int lbound, int rbound, int section = -1,
                  int timeslot = 0, int open_timeslot = 0);
};

#endif