#ifndef _TIMETABLE_SCHEDULE_H
#define _TIMETABLE_SCHEDULE_H

#include <algorithm>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

class Group;
class Subject;
class Teacher;

#include "section.h"
#include "constraints/constraint.h"

class Schedule {
public:
  Schedule(int num_days, int num_slots_per_day, int seed);

  int GetNumDays();
  int GetNumSlots();
  int GetNumSlotsPerDay();

  Group* GetGroup(int idx);
  const std::vector<Group*>& GetGroups() const;
  std::vector<Group*>::iterator GetGroupsBegin();
  std::vector<Group*>::iterator GetGroupsEnd();

  Section* GetSection(int idx);
  const std::vector<Section*>& GetSections() const;
  std::vector<Section*>::iterator GetSectionsBegin();
  std::vector<Section*>::iterator GetSectionsEnd();

  Subject* GetSubject(int idx);
  const std::vector<Subject*>& GetSubjects() const;
  std::vector<Subject*>::iterator GetSubjectsBegin();
  std::vector<Subject*>::iterator GetSubjectsEnd();

  Teacher* GetTeacher(int idx);
  const std::vector<Teacher*>& GetTeachers() const;
  std::vector<Teacher*>::iterator GetTeachersBegin();
  std::vector<Teacher*>::iterator GetTeachersEnd();

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
  bool IsValidSoftTranslate(int section, int timeslot, int open_timeslot);
  bool IsValidSoftSwap(int section, int lhs_timeslot, int rhs_timeslot);

  std::pair<int, int> ClampDay(int timeslot);

  void HardAssign(int subject, int section, int timeslot, int num_slots);
  int HardCountTranslate(int section, int timeslot, int open_timeslot);
  void HardTranslate(int section, int timeslot, int open_timeslot);
  int HardCountSwap(int section, int lhs_timeslot, int rhs_timeslot);
  void HardSwap(int section, int lhs_timeslot, int rhs_timeslot);

  int SoftCountTranslate(int section, int timeslot, int open_timeslot);
  void SoftTranslate(int section, int timeslot, int open_timeslot);
  int SoftCountSwap(int section, int lhs_timeslot, int rhs_timeslot);
  void SoftSwap(int section, int lhs_timeslot, int rhs_timeslot);

  int HardCount();
  int SoftCount();

  bool InitialSchedule();

  template <typename T, typename U>
  int SearchTemplate(T translate, U swap);

  int HardSolver(int time_limit);
  int HardLocalSearch();
  int HardTabuSearch();

  int SoftSolver(int time_limit, int num_samples, double kappa, int tau,
                 double alpha);
  int SoftLocalSearch(bool accept_side, int threshold);
  int SoftSimulatedAnnealing(int time_limit, int num_samples, double kappa,
                             int tau, double alpha);
  double SimulatedAnnealingSample(int num_samples);
  int SimulatedAnnealingSearch(double temperature);

  int Solve(int time_limit, int attempts, int max_best, int num_samples,
            double kappa, int tau, double alpha);
  void SwitchScheduleTo(int schedule);
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

  std::map< int, std::pair< std::vector< std::vector<int> >,
            std::vector< std::vector<int> > > > best_tables_;

  std::vector< std::vector<bool> > subject_tabus_;
};

#include "schedule.tpp"

#endif