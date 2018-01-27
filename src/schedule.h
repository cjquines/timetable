#ifndef _TIMETABLE_SCHEDULE_H
#define _TIMETABLE_SCHEDULE_H

#include <memory>
#include <utility>
#include <vector>

class Group;
class Section;
class Subject;
class Teacher;

#include "constraints/constraint.h"

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

  Subject* GetSubject(const int &idx);
  std::vector<Subject*>::iterator GetSubjectsBegin();
  std::vector<Subject*>::iterator GetSubjectsEnd();

  // std::unique_ptr<Constraint> GetConstraint(const int &idx);
  // std::vector< std::unique_ptr<Constraint> >::iterator GetConstraintsBegin();
  // std::vector< std::unique_ptr<Constraint> >::iterator GetConstraintsEnd();

  void AddGroup(const int &id);
  void Initialize();

  int GetSubjectOf(const int &section, const int &timeslot);
  int GetTeacherOf(const int &section, const int &timeslot);
  int CountSectionsOf(const int &teacher, const int &timeslot);
  int GetSectionOf(const int &teacher, const int &timeslot);

  std::pair<int, int> ClampDay(const int &timeslot);

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
  std::vector<Subject*> subjects_;
  std::vector<Teacher*> teachers_;
  std::vector< std::unique_ptr<Constraint> > constraints_;

  bool hard_satisfied_;

  std::vector< std::vector<Subject*> > timetable_;
  std::vector< std::vector<Section*> > teacher_table_;

  void RandomTimeslots(std::vector< std::pair<int, int> >::iterator begin,
                       std::vector< std::pair<int, int> >::iterator end);
};

#endif