#ifndef _TIMETABLE_CONSTRAINTS_CONSTRAINT_H
#define _TIMETABLE_CONSTRAINTS_CONSTRAINT_H

class Schedule;

class Constraint {
public:
  Constraint(Schedule *schedule, const int &priority);
  virtual int CountTranslate(const int &section, const int &timeslot,
                             const int &open_timeslot);
  virtual int CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                                const int &rhs_timeslot);
  virtual int CountAll();

protected:
  Schedule* schedule_;
  int priority_;
};

#endif