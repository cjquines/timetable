#include "constraint.h"

Constraint::Constraint(Schedule *schedule, const int &priority)
    : schedule_(schedule), priority_(priority) {}

int Constraint::GetPriority() { return priority_; }

int Constraint::CountAssign(const int &subject, const int &section,
                            const int &timeslot) { return 0; }

int Constraint::CountSwapTimeslot(const int &section, const int &lhs_timeslot,
                                  const int &rhs_timeslot) { return 0; }