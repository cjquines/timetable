#ifndef _TIMETABLE_CONSTRAINTS_SUBJECTTIME_H
#define _TIMETABLE_CONSTRAINTS_SUBJECTTIME_H

#include <vector>

#include "constraint.h"

class SubjectTime : public Constraint {
public:
  SubjectTime(Schedule* schedule, int priority, int subject,
              const std::vector<int> &slots, bool daily = false,
              bool unassignable = true);
  int CountTranslate(int section, int timeslot, int open_timeslot) override;
  int CountSwapTimeslot(int section, int lhs_timeslot,
                        int rhs_timeslot) override;
  int CountAll() override;

private:
  int subject_;
  std::vector<bool> unassignable_;

  int HalfCount(int section, int lhs_timeslot, int rhs_timeslot);
};

#endif