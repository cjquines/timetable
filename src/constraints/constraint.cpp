#include "constraint.h"

Constraint::Constraint(Schedule *schedule, const int &priority)
    : schedule_(schedule), priority_(priority) {}

int Constraint::GetPriority() { return priority_; }

int Constraint::CountAssign(const int &subject, const int &section,
                            const int &timeslot, const int &num_slots) {
  return 0;
}

int Constraint::CountTranslate(const int &section, const int &timeslot,
                               const int &open_timeslot) {
  return 0;
}

int Constraint::CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                                  const int &rhs_timeslot) {
  return 0;
}

int Constraint::CountAll() { return 0; }