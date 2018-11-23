#include "reqfirstsubject.h"
#include "../schedule.h"
#include "../section.h"

ReqFirstSubject::ReqFirstSubject(Schedule* schedule, const int &priority)
    : Constraint(schedule, priority) {}

int ReqFirstSubject::CountTranslate(const int &section, const int &timeslot,
                                   const int &open_timeslot) {
  int result = 0;

  if (timeslot % schedule_->GetNumSlotsPerDay() == 0) result++;
  if (open_timeslot % schedule_->GetNumSlotsPerDay() == 0) result--;

  if (priority_ > 0) return result*priority_;
  return result;
}

int ReqFirstSubject::CountAll() {
  int result = 0;
  
  for (auto it = schedule_->GetSectionsBegin();
       it != schedule_->GetSectionsEnd(); it++)
    for (int i = 0; i < schedule_->GetNumDays(); i++)
      if (schedule_->GetSubjectOf((*it)->GetId(),
          i*schedule_->GetNumSlotsPerDay()) == -1)
        result++;

  if (priority_ > 0) return result*priority_;
  return result;
}