#ifndef _TIMETABLE_CONSTRAINTS_REQFIRSTSUBJECT_H
#define _TIMETABLE_CONSTRAINTS_REQFIRSTSUBJECT_H

#include "constraint.h"

class ReqFirstSubject : public Constraint {
public:
  ReqFirstSubject(Schedule* schedule);
  int CountTranslate(const int &section, const int &timeslot,
                     const int &open_timeslot) override;
  int CountAll() override;
};

#endif