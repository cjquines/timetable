#ifndef _TIMETABLE_SUBJECT_H
#define _TIMETABLE_SUBJECT_H

#include <string>
#include <vector>

class Subject {
public:
  Subject(const int &id, const std::vector<int> &slots, const int &teacher,
          const std::string &name);
  int GetId();
  std::vector<int>::iterator GetSlotsBegin();
  std::vector<int>::iterator GetSlotsEnd();
  int GetTeacher();
  std::string GetName();

private:
  int id_;
  std::vector<int> slots_;
  int teacher_;
  std::string name_;
};

#endif