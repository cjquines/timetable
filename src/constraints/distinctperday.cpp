#include <set>
#include <tuple>

#include "distinctperday.h"
#include "../schedule.h"
#include "../section.h"

DistinctPerDay::DistinctPerDay(Schedule* schedule)
    : Constraint(schedule, 0) {}

int DistinctPerDay::CountAssign(const int &subject, const int &section,
                                 const int &timeslot) {
  int lbound, rbound, result = 0;
  std::tie(lbound, rbound) = Constraint::schedule_->ClampDay(timeslot);
  for (int i = lbound; i < rbound; i++)
    if (Constraint::schedule_->GetSubjectOf(section, i) == subject)
      result++;
  return result;
}

int DistinctPerDay::CountTranslate(const int &section, const int &timeslot,
                                   const int &open_timeslot) {
  int lbound, rbound, open_lbound, open_rbound;
  std::tie(lbound, rbound) = Constraint::schedule_->ClampDay(timeslot);
  std::tie(open_lbound, open_rbound) = Constraint::schedule_->ClampDay(open_timeslot);
  int subject = Constraint::schedule_->GetSubjectOf(section, timeslot);
  if (lbound == open_lbound) return 0;

  int result = 1;
  for (int i = lbound; i < rbound; i++) {
    int this_subject = Constraint::schedule_->GetSubjectOf(section, i);
    if (this_subject == subject) result--;
  }
  for (int i = open_lbound; i < open_rbound; i++) {
    int this_subject = Constraint::schedule_->GetSubjectOf(section, i);
    if (this_subject == subject) result++;
  }
  return result;
}

int DistinctPerDay::CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                                      const int &rhs_timeslot) {
  int lhs_lbound, lhs_rbound, rhs_lbound, rhs_rbound;
  std::tie(lhs_lbound, lhs_rbound) = Constraint::schedule_->ClampDay(lhs_timeslot);
  std::tie(rhs_lbound, rhs_rbound) = Constraint::schedule_->ClampDay(rhs_timeslot);
  int lhs_subject = Constraint::schedule_->GetSubjectOf(section, lhs_timeslot);
  int rhs_subject = Constraint::schedule_->GetSubjectOf(section, rhs_timeslot);
  if (lhs_lbound == rhs_lbound || lhs_subject == rhs_subject) return 0;

  int result = 2;
  for (int i = lhs_lbound; i < lhs_rbound; i++) {
    int subject = Constraint::schedule_->GetSubjectOf(section, i);
    if (subject == lhs_subject) result--;
    if (subject == rhs_subject) result++;
  }
  for (int i = rhs_lbound; i < rhs_rbound; i++) {
    int subject = Constraint::schedule_->GetSubjectOf(section, i);
    if (subject == rhs_subject) result--;
    if (subject == lhs_subject) result++;
  }
  return result;
}

int DistinctPerDay::CountAll() {
  int result = 0;
  for (auto it = Constraint::schedule_->GetSectionsBegin();
       it != Constraint::schedule_->GetSectionsEnd(); it++) {
    for (int i = 0; i < Constraint::schedule_->GetNumDays(); i++) {
      std::set<int> seen_subjects;
      for (int j = i*Constraint::schedule_->GetNumSlotsPerDay();
           j < (i+1)*Constraint::schedule_->GetNumSlotsPerDay(); j++) {
        if (Constraint::schedule_->GetSubjectOf((*it)->GetId(), j) < 0)
          continue;
        if (!seen_subjects.insert(
            Constraint::schedule_->GetSubjectOf((*it)->GetId(), j)).second)
          result++;
      }
    }
  }
  return result;
}