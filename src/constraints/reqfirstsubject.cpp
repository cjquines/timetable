#include "reqfirstsubject.h"
#include "../schedule.h"
#include "../section.h"

ReqFirstSubject::ReqFirstSubject(Schedule* schedule, const int &priority)
    : Constraint(schedule, priority) {}

int ReqFirstSubject::CountTranslate(const int &section, const int &timeslot,
                                   const int &open_timeslot) {
  int result = 0;

  if (timeslot % Constraint::schedule_->GetNumSlotsPerDay() == 0) result++;
  if (open_timeslot % Constraint::schedule_->GetNumSlotsPerDay() == 0) result--;

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}

int ReqFirstSubject::CountAll() {
  int result = 0;
  
  for (auto it = Constraint::schedule_->GetSectionsBegin();
       it != Constraint::schedule_->GetSectionsEnd(); it++)
    for (int i = 0; i < Constraint::schedule_->GetNumDays(); i++)
      if (Constraint::schedule_->GetSubjectOf((*it)->GetId(),
          i*Constraint::schedule_->GetNumSlotsPerDay()) == -1)
        result++;

  if (Constraint::priority_ > 0) return result*Constraint::priority_;
  return result;
}