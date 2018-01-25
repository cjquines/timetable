#include "constraint.h"

Constraint::Constraint(Schedule *schedule, const int &priority)
    : schedule_(schedule), priority_(priority) {}