#ifndef _TIMETABLE_CONSTRAINTS_MINSUBJECTS_H
#define _TIMETABLE_CONSTRAINTS_MINSUBJECTS_H

#include "constraint.h"

class MinSubjects : public Constraint {
public:
  MinSubjects(Schedule* schedule, const int &priority,
              const int &min_subjects);
  int CountTranslate(const int &section, const int &timeslot,
                     const int &open_timeslot) override;
  int CountAll() override;
private:
  int min_subjects_;

  int CountSubjects(const int &section, const int &lbound, const int &rbound);
};

#endif