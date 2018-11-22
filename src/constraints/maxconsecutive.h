#ifndef _TIMETABLE_CONSTRAINTS_MAXCONSECUTIVE_H
#define _TIMETABLE_CONSTRAINTS_MAXCONSECUTIVE_H

#include "constraint.h"

class MaxConsecutive : public Constraint {
public:
  MaxConsecutive(Schedule* schedule, const int &priority,
                 const int &max_consecutive);
  int CountTranslate(const int &section, const int &timeslot,
                     const int &open_timeslot) override;
  int CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                        const int &rhs_timeslot) override;
  int CountAll() override;

private:
  int max_consecutive_;
};

#endif