#ifndef _TIMETABLE_SCHEDULE_TPP
#define _TIMETABLE_SCHEDULE_TPP

template <typename T, typename... Args>
void Schedule::AddConstraint(int priority, Args... args) {
  std::unique_ptr<Constraint> ptr = std::make_unique<T>(this, priority,
                                                        args...);
  if (priority <= 0) hard_constraints_.push_back(std::move(ptr));
  else soft_constraints_.push_back(std::move(ptr));
}

template <typename T, typename U>
int Schedule::SearchTemplate(T translate, U swap) {
  std::vector< std::pair<int, int> > to_swap;
  for (const auto &it : GetSections()) {
    for (int i = 0; i < GetNumSlots(); i++) {
      to_swap.emplace_back(it->GetId(), i);
    }
  }
  std::shuffle(to_swap.begin(), to_swap.end(), rand_generator_);

  bool break_flag = false;
  for (auto it = to_swap.begin(); it != to_swap.end(); it++) {
    int section = it->first;
    int lhs_timeslot = it->second;
    if (GetSubjectOf(section, lhs_timeslot) < 0) continue;
    for (auto jt = it+1; jt != to_swap.end(); jt++) {
      if (jt->first != section) continue;
      int rhs_timeslot = jt->second;
      int flag = 0, result;

      if (IsFree(section, rhs_timeslot))
        std::tie(flag, result) = translate(section, lhs_timeslot, rhs_timeslot);
      else if (GetSubjectOf(section, rhs_timeslot) >= 0)
        std::tie(flag, result) = swap(section, lhs_timeslot, rhs_timeslot);

      if (flag == 0) continue;
      else if (flag == 1) return result;
      else if (flag == 2) break_flag = true;
      if (break_flag) break;
    }
    if (break_flag) break;
  }

  return 0;
}

#endif