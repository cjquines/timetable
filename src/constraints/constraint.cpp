#include "constraint.h"

Constraint::Constraint(Schedule *schedule, int priority)
    : schedule_(schedule), priority_(priority) {}

int Constraint::CountTranslate(int section, int timeslot, int open_timeslot) {
  return 0;
}

int Constraint::CountSwapTimeslot(int section, int lhs_timeslot,
                                  int rhs_timeslot) {
  return 0;
}

int Constraint::CountAll() {
  return 0;
}