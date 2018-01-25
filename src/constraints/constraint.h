#ifndef _TIMETABLE_CONSTRAINTS_CONSTRAINT_H
#define _TIMETABLE_CONSTRAINTS_CONSTRAINT_H

class Schedule;

class Constraint {
public:
  Constraint(Schedule *schedule, const int &priority);
  int GetPriority();
  virtual int CountAssign(const int &subject, const int &section,
                          const int &timeslot);
  virtual int CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                                const int &rhs_timeslot);

protected:
  Schedule* schedule_;
  int priority_;
};

#endif