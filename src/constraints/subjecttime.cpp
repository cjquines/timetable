#include <vector>

#include "subjecttime.h"
#include "../schedule.h"
#include "../section.h"

SubjectTime::SubjectTime(Schedule *schedule, const int &priority,
                         const int &subject, const std::vector<int> &unassignable)
    : Constraint(schedule, priority), subject_(subject),
      unassignable_(schedule_->GetNumSlots(), 0) {
  for (auto i : unassignable) unassignable_[i] = 1;
}

int SubjectTime::CountTranslate(const int &section, const int &timeslot,
                                const int &open_timeslot) {
  if (schedule_->GetSubjectOf(section, timeslot) != subject_)
    return 0;
  int num_slots = schedule_->GetLengthOf(section, timeslot);
  int result = 0;
  for (int i = 0; i < num_slots; i++)
    result += unassignable_[open_timeslot+i] - unassignable_[timeslot+i];
  
  if (priority_ > 0) return result*priority_;
  return result;
}

int SubjectTime::CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                                   const int &rhs_timeslot) {
  int num_slots = schedule_->GetLengthOf(section, lhs_timeslot);
  int result = 0;
  if (schedule_->GetSubjectOf(section, lhs_timeslot) == subject_)
    for (int i = 0; i < num_slots; i++)
      result += unassignable_[rhs_timeslot+i] - unassignable_[lhs_timeslot+i];
  if (schedule_->GetSubjectOf(section, rhs_timeslot) == subject_)
    for (int i = 0; i < num_slots; i++)
      result += unassignable_[lhs_timeslot+i] - unassignable_[rhs_timeslot+i];

  if (priority_ > 0) return result*priority_;
  return result;
}

int SubjectTime::CountAll() {
  int result = 0;
  for (auto it = schedule_->GetSectionsBegin();
       it != schedule_->GetSectionsEnd(); it++)
    for (int i = 0; i < schedule_->GetNumSlots(); i++)
      if (unassignable_[i] &&
          schedule_->GetSubjectOf((*it)->GetId(),
            schedule_->GetHeadOf((*it)->GetId(), i)) == subject_)
        result++;

  if (priority_ > 0) return result*priority_;
  return result;
}