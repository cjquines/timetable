#ifndef _TIMETABLE_CONSTRAINTS_EVENDISMISSAL_H
#define _TIMETABLE_CONSTRAINTS_EVENDISMISSAL_H

#include <vector>

#include "constraint.h"

class EvenDismissal : public Constraint {
public:
  EvenDismissal(Schedule* schedule, int priority,
                const std::vector<int> &sections);
  int CountTranslate(int section, int timeslot, int open_timeslot) override;
  int CountAll() override;

private:
  std::vector<int> sections_;

  int Dismissal(int section, int lbound, int rbound);
  int DismissalTranslate(int section, int lbound, int rbound, int timeslot,
                         int open_timeslot);
  int HalfCountTranslate(int section, int lbound, int rbound, int timeslot,
                         int open_timeslot);
};

#endif