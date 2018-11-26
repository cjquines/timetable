#ifndef _TIMETABLE_CONSTRAINTS_DISTINCTPERDAY_H
#define _TIMETABLE_CONSTRAINTS_DISTINCTPERDAY_H

#include "constraint.h"

class DistinctPerDay : public Constraint {
public:
  DistinctPerDay(Schedule* schedule, int priority);
  int CountTranslate(int section, int timeslot, int open_timeslot) override;
  int CountSwapTimeslot(int section, int lhs_timeslot,
                        int rhs_timeslot) override;
  int CountAll() override;

private:
  bool FindSubject(int subject, int section, int lbound,
                   int rbound, int skip_timeslot = -1);
};

#endif