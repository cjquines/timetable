#ifndef _TIMETABLE_CONSTRAINTS_NONSIMULTANEOUS_H
#define _TIMETABLE_CONSTRAINTS_NONSIMULTANEOUS_H

#include "constraint.h"

class NonSimultaneous : public Constraint {
public:
  NonSimultaneous(Schedule* schedule);
  int CountAssign(const int &subject, const int &section, const int &timeslot)
    override;
  int CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                        const int &rhs_timeslot) override;
};

#endif