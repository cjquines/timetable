#include "schedule.h"
#include "group.h"

Schedule::Schedule(const int &num_days, const int &num_slots_per_day)
    : num_days_(num_days), num_slots_per_day_(num_slots_per_day) {
  num_slots_ = num_days_ * num_slots_per_day_;
}

int Schedule::GetNumDays() { return num_days_; }

int Schedule::GetNumSlots() { return num_slots_; }

int Schedule::GetNumSlotsPerDay() { return num_slots_per_day_; }

Group* Schedule::GetGroup(const int &idx) { return groups_[idx]; }

std::vector<Group*>::iterator Schedule::GetGroupsBegin() {
  return groups_.begin();
}

std::vector<Group*>::iterator Schedule::GetGroupsEnd() {
  return groups_.end();
}

Section* Schedule::GetSection(const int &idx) { return sections_[idx]; }

std::vector<Section*>::iterator Schedule::GetSectionsBegin() {
  return sections_.begin();
}

std::vector<Section*>::iterator Schedule::GetSectionsEnd() {
  return sections_.end();
}

// std::unique_ptr<Constraint> Schedule::GetConstraint(const int &idx) {
//   return constraints_[idx];
// }

// std::vector< std::unique_ptr<Constraint> >::iterator
//   Schedule::GetConstraintsBegin() {
//   return constraints_.begin();
// }

// std::vector< std::unique_ptr<Constraint> >::iterator
//   Schedule::GetConstraintsEnd(){
//   return constraints_.end();
// }

void Schedule::AddGroup(const int &id) {
  Group* ptr = new Group(id);
  groups_.push_back(ptr);
}

void Schedule::Initialize() {

}