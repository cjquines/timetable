#include "teacherbreak.h"
#include "../schedule.h"
#include "../teacher.h"

TeacherBreak::TeacherBreak(Schedule* schedule, int priority, int lbound,
                           int rbound, int length)
    : Constraint(schedule, priority), lbound_(lbound), rbound_(rbound),
      length_(length) {}

int TeacherBreak::Score(int teacher, int lbound) {
  auto countfunc = [this] (int teacher, int i) -> int {
    return schedule_->CountSectionsOf(teacher, i);
  };
  return ScoreTemplate(teacher, lbound, countfunc);
}

int TeacherBreak::ScoreTranslate(int teacher, int lbound, int section,
                                 int timeslot, int open_timeslot) {
  auto countfunc = [this, section, timeslot, open_timeslot]
      (int teacher, int i) -> int {
    return schedule_->CountSectionsTranslate(teacher, i, section, timeslot,
                                             open_timeslot);
  };
  return ScoreTemplate(teacher, lbound, countfunc);
}

int TeacherBreak::ScoreAdjSwap(int teacher, int lbound, int section,
                               int lhs_timeslot, int rhs_timeslot) {
  auto countfunc = [this, section, lhs_timeslot, rhs_timeslot]
      (int teacher, int i) -> int {
    return schedule_->CountSectionsAdjSwap(teacher, i, section, lhs_timeslot,
                                           rhs_timeslot);
  };
  return ScoreTemplate(teacher, lbound, countfunc);
}

int TeacherBreak::CountTranslate(int section, int timeslot,
                                   int open_timeslot) {
  int teacher = schedule_->GetTeacherOf(section, timeslot);
  int lbound = schedule_->ClampDay(timeslot).first;
  int open_lbound = schedule_->ClampDay(open_timeslot).first;
  int result = ScoreTranslate(teacher, lbound, section, timeslot,
                              open_timeslot)
             - Score(teacher, lbound)
             + ScoreTranslate(teacher, open_lbound, section, timeslot,
                              open_timeslot)
             - Score(teacher, open_lbound);

  if (lbound == open_lbound) result /= 2;
  if (priority_ > 0) return result*priority_;
  return result;
}

int TeacherBreak::CountSwapTimeslot(int section, int lhs_timeslot,
                                    int rhs_timeslot) {
  return CountAdjSwap(section, lhs_timeslot, rhs_timeslot);
}

int TeacherBreak::CountAdjSwap(int section, int lhs_timeslot,
                               int rhs_timeslot) {
  int new_rhs_slot = schedule_->NewRHSSlot(section, lhs_timeslot, rhs_timeslot);
  if (schedule_->GetTeacherOf(section, lhs_timeslot)
   != schedule_->GetTeacherOf(section, rhs_timeslot))
    return CountTranslate(section, lhs_timeslot, new_rhs_slot)
         + CountTranslate(section, rhs_timeslot, lhs_timeslot);

  int teacher = schedule_->GetTeacherOf(section, lhs_timeslot);
  int lhs_lbound = schedule_->ClampDay(lhs_timeslot).first;
  int rhs_lbound = schedule_->ClampDay(rhs_timeslot).first;
  int result = ScoreAdjSwap(teacher, lhs_lbound, section, lhs_timeslot,
                            rhs_timeslot)
             - Score(teacher, lhs_lbound)
             + ScoreAdjSwap(teacher, rhs_lbound, section, lhs_timeslot,
                            rhs_timeslot)
             - Score(teacher, rhs_lbound);

  if (lhs_lbound == rhs_lbound) result /= 2;
  if (priority_ > 0) return result*priority_;
  return result;
}

int TeacherBreak::CountAll() {
  int result = 0;
  for (const auto &it : schedule_->GetTeachers()) {
    if (it->GetId() == 0) continue;
    for (int i = 0; i < schedule_->GetNumDays(); i++) {
      result += Score(it->GetId(), i*schedule_->GetNumSlotsPerDay());
    }
  }

  if (priority_ > 0) return result*priority_;
  return result;
}