#ifndef _TIMETABLE_CONSTRAINTS_SUBJECTGAPS_H
#define _TIMETABLE_CONSTRAINTS_SUBJECTGAPS_H

#include "constraint.h"

class SubjectGaps : public Constraint {
public:
  SubjectGaps(Schedule* schedule, const int &priority);
  int CountTranslate(const int &section, const int &timeslot,
                     const int &open_timeslot) override;
  int CountAll() override;
};

#endif