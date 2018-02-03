#ifndef _TIMETABLE_CONSTRAINTS_TEACHERTIME_H
#define _TIMETABLE_CONSTRAINTS_TEACHERTIME_H

#include <vector>

#include "constraint.h"

class TeacherTime : public Constraint {
public:
  TeacherTime(Schedule* schedule, const int &priority, const int &teacher,
              const std::vector<int> &unassignable);
  int CountTranslate(const int &section, const int &timeslot,
                     const int &open_timeslot) override;
  int CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                        const int &rhs_timeslot) override;
  int CountAll() override;

private:
  int teacher_;
  std::vector<bool> unassignable_;
};

#endif