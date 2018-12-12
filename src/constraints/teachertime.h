#ifndef _TIMETABLE_CONSTRAINTS_TEACHERTIME_H
#define _TIMETABLE_CONSTRAINTS_TEACHERTIME_H

#include <vector>

#include "constraint.h"

class TeacherTime : public Constraint {
public:
  TeacherTime(Schedule* schedule, int priority, int teacher,
              const std::vector<int> &unassignable, bool daily = false);
  int CountTranslate(int section, int timeslot, int open_timeslot) override;
  int CountSwapTimeslot(int section, int lhs_timeslot,
                        int rhs_timeslot) override;
  int CountAdjSwap(int section, int lhs_timeslot, int rhs_timeslot) override;
  int CountAll() override;

private:
  int teacher_;
  std::vector<bool> unassignable_;

  int HalfCount(int section, int lhs_timeslot, int rhs_timeslot);
};

#endif