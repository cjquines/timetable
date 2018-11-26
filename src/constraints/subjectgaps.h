#ifndef _TIMETABLE_CONSTRAINTS_SUBJECTGAPS_H
#define _TIMETABLE_CONSTRAINTS_SUBJECTGAPS_H

#include "constraint.h"

class SubjectGaps : public Constraint {
public:
  SubjectGaps(Schedule* schedule, int priority);
  int CountTranslate(int section, int timeslot, int open_timeslot) override;
  int CountAll() override;
};

#endif