#ifndef _TIMETABLE_SOLVER_TPP
#define _TIMETABLE_SOLVER_TPP

template <typename T, typename U, typename V>
int Solver::SearchTemplate(T translate, U swap, V adjswap) {
  std::vector< std::pair<int, int> > to_swap;
  for (const auto &it : schedule_->GetSections()) {
    for (int i = 0; i < schedule_->GetNumSlots(); i++) {
      to_swap.emplace_back(it->GetId(), i);
    }
  }
  std::shuffle(to_swap.begin(), to_swap.end(), rand_generator_);

  bool break_flag = false;
  for (auto it = to_swap.begin(); it != to_swap.end(); it++) {
    int section = it->first;
    int lhs_timeslot = it->second;
    if (schedule_->GetSubjectOf(section, lhs_timeslot) < 0) continue;
    for (auto jt = it+1; jt != to_swap.end(); jt++) {
      if (jt->first != section) continue;
      int rhs_timeslot = jt->second;
      int flag = 0, result;

      if (schedule_->IsFree(section, rhs_timeslot))
        std::tie(flag, result) = translate(section, lhs_timeslot, rhs_timeslot);
      else if (schedule_->GetSubjectOf(section, rhs_timeslot) >= 0) {
        if (schedule_->GetLengthOf(section, lhs_timeslot)
         == schedule_->GetLengthOf(section, rhs_timeslot))
          std::tie(flag, result) = swap(section, lhs_timeslot, rhs_timeslot);
        else
          std::tie(flag, result) = adjswap(section, lhs_timeslot, rhs_timeslot);
      }

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