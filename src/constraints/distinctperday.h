#ifndef _TIMETABLE_CONSTRAINTS_DISTINCTPERDAY_H
#define _TIMETABLE_CONSTRAINTS_DISTINCTPERDAY_H

#include "constraint.h"

class DistinctPerDay : public Constraint {
public:
  DistinctPerDay(Schedule* schedule, const int &priority);
  int CountTranslate(const int &section, const int &timeslot,
                     const int &open_timeslot) override;
  int CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                        const int &rhs_timeslot) override;
  int CountAll() override;

private:
  bool FindSubject(const int &subject, const int &section, const int &lbound,
                   const int &rbound, const int &skip_timeslot = -1);
};

#endif