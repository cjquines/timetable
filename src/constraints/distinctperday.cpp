#include <set>
#include <tuple>

#include "distinctperday.h"
#include "../schedule.h"
#include "../section.h"

DistinctPerDay::DistinctPerDay(Schedule* schedule, const int &priority)
    : Constraint(schedule, priority) {}

bool DistinctPerDay::FindSubject(const int &subject, const int &section,
                                 const int &lbound, const int &rbound,
                                 const int &skip_timeslot) {
  for (int i = lbound; i < rbound; i++) {
    if (i == skip_timeslot) continue;
    if (subject == schedule_->GetSubjectOf(section, i)) return true;
  }
  return false;
}

int DistinctPerDay::CountTranslate(const int &section, const int &timeslot,
                                   const int &open_timeslot) {
  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = schedule_->ClampDay(open_timeslot);
  int subject = schedule_->GetSubjectOf(section, timeslot);
  if (lbound == open_lbound) return 0;

  int result = FindSubject(subject, section, open_lbound, open_rbound)
             - FindSubject(subject, section, lbound, rbound, timeslot);

  if (priority_ > 0) return result*priority_;
  return result;
}

int DistinctPerDay::CountSwapTimeslot(const int &section,
                                      const int &lhs_timeslot,
                                      const int &rhs_timeslot) {
  int lhs_lbound, lhs_rbound, rhs_lbound, rhs_rbound;
  std::tie(lhs_lbound, lhs_rbound) = schedule_->ClampDay(lhs_timeslot);
  std::tie(rhs_lbound, rhs_rbound) = schedule_->ClampDay(rhs_timeslot);
  int lhs_subject = schedule_->GetSubjectOf(section, lhs_timeslot);
  int rhs_subject = schedule_->GetSubjectOf(section, rhs_timeslot);
  if (lhs_lbound == rhs_lbound || lhs_subject == rhs_subject) return 0;

  int result =
      FindSubject(lhs_subject, section, rhs_lbound, rhs_rbound)
    - FindSubject(lhs_subject, section, lhs_lbound, lhs_rbound, lhs_timeslot)
    + FindSubject(rhs_subject, section, lhs_lbound, lhs_rbound)
    - FindSubject(rhs_subject, section, rhs_lbound, rhs_rbound, rhs_timeslot);

  if (priority_ > 0) return result*priority_;
  return result;
}

int DistinctPerDay::CountAll() {
  int result = 0;
  for (const auto &it : schedule_->GetSections()) {
    for (int i = 0; i < schedule_->GetNumDays(); i++) {
      std::set<int> seen_subjects;
      for (int j = i*schedule_->GetNumSlotsPerDay();
           j < (i+1)*schedule_->GetNumSlotsPerDay(); j++) {
        if (schedule_->GetSubjectOf(it->GetId(), j) < 0) continue;
        if (!seen_subjects.insert(
            schedule_->GetSubjectOf(it->GetId(), j)).second) result++;
      }
    }
  }

  if (priority_ > 0) return result*priority_;
  return result;
}