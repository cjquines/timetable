#ifndef _TIMETABLE_CONSTRAINTS_EVENDISMISSAL_H
#define _TIMETABLE_CONSTRAINTS_EVENDISMISSAL_H

#include "constraint.h"

class EvenDismissal : public Constraint {
public:
  EvenDismissal(Schedule* schedule, const int &priority);
  int CountTranslate(const int &section, const int &timeslot,
                     const int &open_timeslot) override;
  int CountAll() override;
};

#endif