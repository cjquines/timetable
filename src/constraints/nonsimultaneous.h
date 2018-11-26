#ifndef _TIMETABLE_CONSTRAINTS_NONSIMULTANEOUS_H
#define _TIMETABLE_CONSTRAINTS_NONSIMULTANEOUS_H

#include "constraint.h"

class NonSimultaneous : public Constraint {
public:
  NonSimultaneous(Schedule* schedule, int priority);
  int CountTranslate(int section, int timeslot, int open_timeslot) override;
  int CountSwapTimeslot(int section, int lhs_timeslot,
                        int rhs_timeslot) override;
  int CountAll() override;
};

#endif