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

  for (auto it = to_swap.begin(); it != to_swap.end(); it++) {
    int section = it->first;
    int lhs_timeslot = it->second;
    if (schedule_->GetSubjectOf(section, lhs_timeslot) < 0) continue;
    int flag = 0, result = 0;

    for (auto jt = it+1; jt != to_swap.end(); jt++) {
      if (jt->first != section) continue;
      int rhs_timeslot = jt->second;
      if (schedule_->IsFree(section, rhs_timeslot))
        std::tie(flag, result) = translate(section, lhs_timeslot, rhs_timeslot);
      else if (schedule_->GetSubjectOf(section, rhs_timeslot) >= 0) 
        std::tie(flag, result) = swap(section, lhs_timeslot, rhs_timeslot);
      if (flag >= 1) break;
    }

    if (flag == 0) {
      int rhs_timeslot = schedule_->NextSubject(section, lhs_timeslot);
      if (rhs_timeslot == schedule_->ClampDay(lhs_timeslot).second) break;
      std::tie(flag, result) = adjswap(section, lhs_timeslot, rhs_timeslot);
    }

    if (flag == 1) return result;
    if (flag == 2) break;
  }

  return 0;
}

template <typename T, typename U, typename V>
int Solver::HardSearchTemplate(T translate, U swap, V adjswap) {
  auto new_translate = [this, &translate]
      (int section, int timeslot, int open_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidHardTranslate(section, timeslot, open_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->HardCountTranslate(section, timeslot, open_timeslot);
    return translate(section, timeslot, open_timeslot, delta);
  };

  auto new_swap = [this, &swap]
      (int section, int lhs_timeslot, int rhs_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidHardSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->HardCountSwap(section, lhs_timeslot, rhs_timeslot);
    return swap(section, lhs_timeslot, rhs_timeslot, delta);
  };

  auto new_adjswap = [this, &adjswap]
      (int section, int lhs_timeslot, int rhs_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidHardAdjSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->HardCountAdjSwap(section, lhs_timeslot,
                                            rhs_timeslot);
    return adjswap(section, lhs_timeslot, rhs_timeslot, delta);
  };

  return SearchTemplate(new_translate, new_swap, new_adjswap);
}

template <typename T, typename U, typename V>
int Solver::SoftSearchTemplate(T translate, U swap, V adjswap) {
  auto new_translate = [this, &translate]
      (int section, int timeslot, int open_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidSoftTranslate(section, timeslot, open_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->SoftCountTranslate(section, timeslot, open_timeslot);
    return translate(section, timeslot, open_timeslot, delta);
  };

  auto new_swap = [this, &swap]
      (int section, int lhs_timeslot, int rhs_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidSoftSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->SoftCountSwap(section, lhs_timeslot, rhs_timeslot);
    return swap(section, lhs_timeslot, rhs_timeslot, delta);
  };

  auto new_adjswap = [this, &adjswap]
      (int section, int lhs_timeslot, int rhs_timeslot) -> std::pair<int, int> {
    if (!schedule_->IsValidSoftAdjSwap(section, lhs_timeslot, rhs_timeslot))
      return std::make_pair(0, 0);
    int delta = schedule_->SoftCountAdjSwap(section, lhs_timeslot,
                                            rhs_timeslot);
    return adjswap(section, lhs_timeslot, rhs_timeslot, delta);
  };

  return SearchTemplate(new_translate, new_swap, new_adjswap);
}

#endif