#ifndef _TIMETABLE_CONSTRAINTS_TEACHERBREAK_H
#define _TIMETABLE_CONSTRAINTS_TEACHERBREAK_H

#include "constraint.h"

class TeacherBreak : public Constraint {
public:
  TeacherBreak(Schedule* schedule, int priority, int lbound, int rbound,
               int length);
  int CountTranslate(int section, int timeslot, int open_timeslot) override;
  int CountSwapTimeslot(int section, int lhs_timeslot,
                        int rhs_timeslot) override;
  int CountAdjSwap(int section, int lhs_timeslot, int rhs_timeslot) override;
  int CountAll() override;

private:
  int lbound_, rbound_, length_;

  template <typename T>
  int ScoreTemplate(int teacher, int lbound, T countfunc);
  int Score(int teacher, int lbound);
  int ScoreTranslate(int teacher, int lbound, int section, int timeslot,
                     int open_timeslot);
  int ScoreAdjSwap(int teacher, int lbound, int section, int lhs_timeslot,
                   int rhs_timeslot);
};

#include "teacherbreak.tpp"

#endif