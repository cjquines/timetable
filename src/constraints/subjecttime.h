#ifndef _TIMETABLE_CONSTRAINTS_SUBJECTTIME_H
#define _TIMETABLE_CONSTRAINTS_SUBJECTTIME_H

#include <vector>

#include "constraint.h"

class SubjectTime : public Constraint {
public:
  SubjectTime(Schedule* schedule, const int &priority, const int &subject,
              const std::vector<int> &unassignable);
  int CountTranslate(const int &section, const int &timeslot,
                     const int &open_timeslot) override;
  int CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                        const int &rhs_timeslot) override;
  int CountAll() override;

private:
  int subject_;
  std::vector<bool> unassignable_;

  int HalfCount(const int &section, const int &lhs_timeslot,
                const int &rhs_timeslot);
};

#endif