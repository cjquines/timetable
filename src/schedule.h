#ifndef _TIMETABLING_SCHEDULE_H
#define _TIMETABLING_SCHEDULE_H

#include <memory>
#include <vector>

class Group;
class Section;
class Teacher;

#include "constraint.h"

class Schedule {
public:
  Schedule(const int &num_days, const int &num_slots_per_day);

  int GetNumDays();
  int GetNumSlots();
  int GetNumSlotsPerDay();

  Group* GetGroup(const int &idx);
  std::vector<Group*>::iterator GetGroupsBegin();
  std::vector<Group*>::iterator GetGroupsEnd();

  Section* GetSection(const int &idx);
  std::vector<Section*>::iterator GetSectionsBegin();
  std::vector<Section*>::iterator GetSectionsEnd();

  // std::unique_ptr<Constraint> GetConstraint(const int &idx);
  // std::vector< std::unique_ptr<Constraint> >::iterator GetConstraintsBegin();
  // std::vector< std::unique_ptr<Constraint> >::iterator GetConstraintsEnd();

  void AddGroup(const int &id);
  void Initialize();

  int HardCountAssign(const int &subject, const int &section,
                      const int &timeslot);
  void HardAssign(const int &subject, const int &section, const int &timeslot);
  int HardCountSwap(const int &section, const int &lhs_timeslot,
                    const int &rhs_timeslot);
  void HardSwap(const int &section, const int &lhs_timeslot,
                const int &rhs_timeslot);
  int SoftCountSwap(const int &section, const int &lhs_timeslot,
                    const int &rhs_timeslot);
  void SoftSwap(const int &section, const int &lhs_timeslot,
                const int &rhs_timeslot);

private:
  int num_days_;
  int num_slots_;
  int num_slots_per_day_;

  std::vector<Group*> groups_;
  std::vector<Section*> sections_;
  std::vector<Teacher*> teachers_;
  std::vector< std::unique_ptr<Constraint> > constraints_;

  std::vector< std::vector<int> > timetable_;
  std::vector< std::vector<int> > teacher_table_;

  void RandomTimeslots(std::vector< std::pair<int, int> >::iterator begin,
                       std::vector< std::pair<int, int> >::iterator end);
};

#endif