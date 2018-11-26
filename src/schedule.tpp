#ifndef _TIMETABLE_SCHEDULE_TPP
#define _TIMETABLE_SCHEDULE_TPP

template <typename T, typename... Args>
void Schedule::AddConstraint(int priority, Args... args) {
  Constraint* ptr = new T(this, priority, args...);
  if (priority <= 0) hard_constraints_.push_back(ptr);
  else soft_constraints_.push_back(ptr);
}

#endif