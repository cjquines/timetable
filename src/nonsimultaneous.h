#ifndef _TIMETABLING_NONSIMULTANEOUS_H
#define _TIMETABLING_NONSIMULTANEOUS_H

#include "constraint.h"

class NonSimultaneous : public Constraint {
public:
  NonSimultaneous(Schedule* schedule);
  int CountAssign(const int &subject, const int &section, const int &timeslot)
    override;
};

#endif