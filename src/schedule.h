#ifndef _TIMETABLE_SCHEDULE_H
#define _TIMETABLE_SCHEDULE_H

#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

class Group;
class Section;
class Subject;
class Teacher;

#include "constraints/constraint.h"

class Schedule {
public:
  Schedule(const int &num_days, const int &num_slots_per_day, const int &seed);

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

  Teacher* GetTeacher(const int &idx);
  std::vector<Teacher*>::iterator GetTeachersBegin();
  std::vector<Teacher*>::iterator GetTeachersEnd();

  void AddGroup(const int &id);
  void AddTeacher(const int &id, const std::string &name);
  void Initialize();

  int GetSubjectOf(const int &section, const int &timeslot);
  int GetTeacherOf(const int &section, const int &timeslot);
  int CountSectionsOf(const int &teacher, const int &timeslot);
  int GetSectionOf(const int &teacher, const int &timeslot);

  std::pair<int, int> ClampDay(const int &timeslot);

  int HardCountAssign(const int &subject, const int &section,
                      const int &timeslot);
  void HardAssign(const int &subject, const int &section, const int &timeslot);
  int HardCountTranslate(const int &section, const int &timeslot,
                         const int &open_timeslot);
  void HardTranslate(const int &section, const int &timeslot,
                     const int &open_timeslot);
  int HardCountSwap(const int &section, const int &lhs_timeslot,
                    const int &rhs_timeslot);
  void HardSwap(const int &section, const int &lhs_timeslot,
                const int &rhs_timeslot);
  int SoftCountSwap(const int &section, const int &lhs_timeslot,
                    const int &rhs_timeslot);
  void SoftSwap(const int &section, const int &lhs_timeslot,
                const int &rhs_timeslot);

  int HardCount();

  void InitialSchedule();
  void HardSolver(int current);

  void TestPrint();

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
  std::mt19937 rand_generator_;

  std::vector< std::vector<int> > timetable_;
  std::vector< std::vector<int> > teacher_table_;

  void RandomTimeslots(Group* group,
                       std::vector< std::pair<int, int> >::iterator begin,
                       std::vector< std::pair<int, int> >::iterator end);
};

#endif