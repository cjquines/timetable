#ifndef _TIMETABLE_SCHEDULE_TPP
#define _TIMETABLE_SCHEDULE_TPP

template <typename T, typename... Args>
void Schedule::AddConstraint(int priority, Args... args) {
  std::unique_ptr<Constraint> ptr = std::make_unique<T>(this, priority,
                                                        args...);
  if (priority <= 0) hard_constraints_.push_back(std::move(ptr));
  else soft_constraints_.push_back(std::move(ptr));
}

#endif