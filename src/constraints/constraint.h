#ifndef _TIMETABLE_CONSTRAINTS_CONSTRAINT_H
#define _TIMETABLE_CONSTRAINTS_CONSTRAINT_H

class Schedule;

class Constraint {
public:
  Constraint(Schedule *schedule, int priority);
  virtual int CountTranslate(int section, int timeslot, int open_timeslot);
  virtual int CountSwapTimeslot(int section, int lhs_timeslot,
                                int rhs_timeslot);
  virtual int CountAll();

protected:
  Schedule* schedule_;
  int priority_;
};

#endif