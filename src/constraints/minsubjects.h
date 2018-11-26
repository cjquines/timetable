#ifndef _TIMETABLE_CONSTRAINTS_MINSUBJECTS_H
#define _TIMETABLE_CONSTRAINTS_MINSUBJECTS_H

#include "constraint.h"

class MinSubjects : public Constraint {
public:
  MinSubjects(Schedule* schedule, int priority, int min_subjects);
  int CountTranslate(int section, int timeslot, int open_timeslot) override;
  int CountAll() override;
private:
  int min_subjects_;

  int CountSubjects(int section, int lbound, int rbound);
};

#endif