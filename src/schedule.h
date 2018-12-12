#ifndef _TIMETABLE_SCHEDULE_H
#define _TIMETABLE_SCHEDULE_H

#include <memory>
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
  Schedule(int num_days, int num_slots_per_day);

  int GetNumDays();
  int GetNumSlots();
  int GetNumSlotsPerDay();

  Group* GetGroup(int idx);
  const std::vector<Group*>& GetGroups() const;

  Section* GetSection(int idx);
  const std::vector<Section*>& GetSections() const;

  Subject* GetSubject(int idx);
  const std::vector<Subject*>& GetSubjects() const;

  Teacher* GetTeacher(int idx);
  const std::vector<Teacher*>& GetTeachers() const;

  void AddGroup(int id);
  void AddTeacher(int id, const std::string &name);

  template <typename T, typename... Args>
  void AddConstraint(int priority, Args... args);

  void ResetTimetable();
  void Initialize();
  void SoftInitialize();

  int GetSubjectOf(int section, int timeslot);
  int GetHeadOf(int section, int timeslot);
  int GetTeacherOf(int section, int timeslot);
  int GetLengthOf(int section, int timeslot);
  int CountSectionsOf(int teacher, int timeslot);
  int CountSectionsTranslate(int teacher, int timeslot, int section,
                             int tr_timeslot, int open_timeslot);
  int GetSectionOf(int teacher, int timeslot);

  bool IsFree(int section, int timeslot, int num_slots = 1);
  bool IsFreeTranslate(int timeslot, int section, int tr_timeslot,
                       int open_timeslot);
  bool IsValidHardTranslate(int section, int timeslot, int open_timeslot);
  bool IsValidHardSwap(int section, int lhs_timeslot, int rhs_timeslot);
  bool IsValidHardAdjSwap(int section, int lhs_timeslot, int rhs_timeslot);
  bool IsValidSoftTranslate(int section, int timeslot, int open_timeslot);
  bool IsValidSoftSwap(int section, int lhs_timeslot, int rhs_timeslot);
  bool IsValidSoftAdjSwap(int section, int lhs_timeslot, int rhs_timeslot);

  std::pair<int, int> ClampDay(int timeslot);
  int NewRHSSlot(int section, int lhs_timeslot, int rhs_timeslot);
  int NextSubject(int section, int timeslot);

  void HardAssign(int subject, int section, int timeslot, int num_slots);

  int HardCountTranslate(int section, int timeslot, int open_timeslot);
  void HardTranslate(int section, int timeslot, int open_timeslot);
  int HardCountSwap(int section, int lhs_timeslot, int rhs_timeslot);
  void HardSwap(int section, int lhs_timeslot, int rhs_timeslot);
  int HardCountAdjSwap(int section, int lhs_timeslot, int rhs_timeslot);
  void HardAdjSwap(int section, int lhs_timeslot, int rhs_timeslot);

  int SoftCountTranslate(int section, int timeslot, int open_timeslot);
  void SoftTranslate(int section, int timeslot, int open_timeslot);
  int SoftCountSwap(int section, int lhs_timeslot, int rhs_timeslot);
  void SoftSwap(int section, int lhs_timeslot, int rhs_timeslot);
  int SoftCountAdjSwap(int section, int lhs_timeslot, int rhs_timeslot);
  void SoftAdjSwap(int section, int lhs_timeslot, int rhs_timeslot);

  int HardCount();
  int SoftCount();

  void TestPrint();

private:
  int num_days_;
  int num_slots_;
  int num_slots_per_day_;

  std::vector<Group*> groups_;
  std::vector<Section*> sections_;
  std::vector<Subject*> subjects_;
  std::vector<Teacher*> teachers_;
  std::vector<Constraint*> hard_constraints_;
  std::vector<Constraint*> soft_constraints_;

  bool hard_satisfied_;

  std::vector< std::vector<int> > timetable_;
  std::vector< std::vector<int> > teacher_table_;
};

#include "schedule.tpp"

#endif