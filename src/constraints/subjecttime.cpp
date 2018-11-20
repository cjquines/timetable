#include <vector>

#include "subjecttime.h"
#include "../schedule.h"
#include "../section.h"

SubjectTime::SubjectTime(Schedule *schedule, const int &priority,
                         const int &subject, const std::vector<int> &unassignable)
    : Constraint(schedule, priority), subject_(subject),
      unassignable_(Constraint::schedule_->GetNumSlots(), 0) {
  for (auto i : unassignable) unassignable_[i] = 1;
}

int SubjectTime::CountTranslate(const int &section, const int &timeslot,
                                const int &open_timeslot) {
  if (Constraint::schedule_->GetSubjectOf(section, timeslot) != subject_)
    return 0;
  int num_slots = Constraint::schedule_->GetLengthOf(section, timeslot);
  int result = 0;
  for (int i = 0; i < num_slots; i++)
    result += unassignable_[open_timeslot+i] - unassignable_[timeslot+i];
  
  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}

int SubjectTime::CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                                   const int &rhs_timeslot) {
  int num_slots = Constraint::schedule_->GetLengthOf(section, lhs_timeslot);
  int result = 0;
  if (Constraint::schedule_->GetSubjectOf(section, lhs_timeslot) == subject_)
    for (int i = 0; i < num_slots; i++)
      result += unassignable_[rhs_timeslot+i] - unassignable_[lhs_timeslot+i];
  if (Constraint::schedule_->GetSubjectOf(section, rhs_timeslot) == subject_)
    for (int i = 0; i < num_slots; i++)
      result += unassignable_[lhs_timeslot+i] - unassignable_[rhs_timeslot+i];

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}

int SubjectTime::CountAll() {
  int result = 0;
  for (auto it = Constraint::schedule_->GetSectionsBegin();
       it != Constraint::schedule_->GetSectionsEnd(); it++)
    for (int i = 0; i < Constraint::schedule_->GetNumSlots(); i++)
      if (unassignable_[i] &&
          Constraint::schedule_->GetSubjectOf((*it)->GetId(),
            Constraint::schedule_->GetHeadOf((*it)->GetId(), i)) == subject_)
        result++;

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}