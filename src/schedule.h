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
  void AddSubjectGaps(const int &priority);
  void AddSubjectTime(const int &priority, const int &subject,
                      const std::vector<int> &unassignable);
  void AddTeacherTime(const int &priority, const int &teacher,
                      const std::vector<int> &unassignable);
  void Reset();
  void Initialize();
  void SoftInitialize();

  int GetSubjectOf(const int &section, const int &timeslot);
  int GetHeadOf(const int &section, const int &timeslot);
  int GetTeacherOf(const int &section, const int &timeslot);
  int GetLengthOf(const int &section, const int &timeslot);
  int CountSectionsOf(const int &teacher, const int &timeslot);
  int GetSectionOf(const int &teacher, const int &timeslot);

  bool IsFree(const int &section, const int &timeslot, const int &num_slots);
  bool IsValidHardTranslate(const int &section, const int &timeslot,
                            const int &open_timeslot);
  bool IsValidHardSwap(const int &section, const int &lhs_timeslot,
                       const int &rhs_timeslot);
  bool IsValidSoftTranslate(const int &section, const int &timeslot,
                            const int &open_timeslot);
  bool IsValidSoftSwap(const int &section, const int &lhs_timeslot,
                       const int &rhs_timeslot);

  std::pair<int, int> ClampDay(const int &timeslot);

  void HardAssign(const int &subject, const int &section, const int &timeslot,
                  const int &num_slots);
  int HardCountTranslate(const int &section, const int &timeslot,
                         const int &open_timeslot);
  void HardTranslate(const int &section, const int &timeslot,
                     const int &open_timeslot);
  int HardCountSwap(const int &section, const int &lhs_timeslot,
                    const int &rhs_timeslot);
  void HardSwap(const int &section, const int &lhs_timeslot,
                const int &rhs_timeslot);

  int SoftCountTranslate(const int &section, const int &timeslot,
                         const int &open_timeslot);
  void SoftTranslate(const int &section, const int &timeslot,
                     const int &open_timeslot);
  int SoftCountSwap(const int &section, const int &lhs_timeslot,
                    const int &rhs_timeslot);
  void SoftSwap(const int &section, const int &lhs_timeslot,
                const int &rhs_timeslot);

  int HardCount();
  int SoftCount();

  void InitialSchedule();
  int HardSolver(const int &time_limit);
  int HardLocalSearch();
  int HardTabuSearch();
  int SoftLocalSearch(const bool &accept_side, const int &threshold);
  int SoftSimulatedAnnealing(const int &time_limit, const double &kappa,
                             const int &tau, const double &alpha);
  double SimulatedAnnealingSample(const int &num_samples);
  int SimulatedAnnealingSearch(const double &temperature);

  void TestPrint();

private:
  int num_days_;
  int num_slots_;
  int num_slots_per_day_;

  std::vector<Group*> groups_;
  std::vector<Section*> sections_;
  std::vector<Subject*> subjects_;
  std::vector<Teacher*> teachers_;
  std::vector< std::unique_ptr<Constraint> > hard_constraints_;
  std::vector< std::unique_ptr<Constraint> > soft_constraints_;

  bool hard_satisfied_;
  std::mt19937 rand_generator_;

  std::vector< std::vector<int> > timetable_;
  std::vector< std::vector<int> > teacher_table_;

  std::vector< std::vector<bool> > subject_tabus_;
};

#endif