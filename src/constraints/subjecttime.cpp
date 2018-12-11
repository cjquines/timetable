#include <vector>

#include "subjecttime.h"
#include "../schedule.h"
#include "../section.h"

SubjectTime::SubjectTime(Schedule *schedule, int priority, int subject,
                         const std::vector<int> &slots, bool daily,
                         bool unassignable)
    : Constraint(schedule, priority), subject_(subject) {
  std::vector<int> new_slots(slots);
  if (daily)
    for (int k = 1; k < schedule_->GetNumDays(); k++)
      for (auto i : slots)
        new_slots.push_back(k*schedule_->GetNumSlotsPerDay() + i);
  if (unassignable) {
    unassignable_.assign(schedule_->GetNumSlots(), 0);
    for (auto i : new_slots) unassignable_[i] = 1;
  } else {
    unassignable_.assign(schedule_->GetNumSlots(), 1);
    for (auto i : new_slots) unassignable_[i] = 0;
  }
}

int SubjectTime::HalfCount(int section, int lhs_timeslot, int rhs_timeslot) {
  int length = schedule_->GetLengthOf(section, lhs_timeslot);
  int result = 0;
  for (int i = 0; i < length; i++) {
    result += unassignable_[rhs_timeslot + i] - unassignable_[lhs_timeslot + i];
  }
  return result;
}

int SubjectTime::CountTranslate(int section, int timeslot, int open_timeslot) {
  if (schedule_->GetSubjectOf(section, timeslot) != subject_) return 0;
  int result = HalfCount(section, timeslot, open_timeslot);
  
  if (priority_ > 0) return result*priority_;
  return result;
}

int SubjectTime::CountSwapTimeslot(int section, int lhs_timeslot,
                                   int rhs_timeslot) {
  int result = 0;
  if (schedule_->GetSubjectOf(section, lhs_timeslot) == subject_)
    result += HalfCount(section, lhs_timeslot, rhs_timeslot);
  if (schedule_->GetSubjectOf(section, rhs_timeslot) == subject_)
    result += HalfCount(section, rhs_timeslot, lhs_timeslot);

  if (priority_ > 0) return result*priority_;
  return result;
}

int SubjectTime::CountAll() {
  int result = 0;
  for (const auto &it : schedule_->GetSections()) {
    for (int i = 0; i < schedule_->GetNumSlots(); i++) {
      if (unassignable_[i] && schedule_->GetSubjectOf(it->GetId(),
          schedule_->GetHeadOf(it->GetId(), i)) == subject_) result++;
    }
  }

  if (priority_ > 0) return result*priority_;
  return result;
}