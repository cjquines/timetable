#ifndef _TIMETABLE_SUBJECT_H
#define _TIMETABLE_SUBJECT_H

#include <string>

class Subject {
public:
  Subject(const int &id, const int &num_slots, const int &teacher,
          const std::string &name);
  int GetId();
  int GetNumSlots();
  int GetTeacher();
  std::string GetName();

private:
  int id_;
  int num_slots_;
  int teacher_;
  std::string name_;
};

#endif