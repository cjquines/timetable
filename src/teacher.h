#ifndef _TIMETABLE_TEACHER_H
#define _TIMETABLE_TEACHER_H

#include <string>

class Teacher {
public:
  Teacher(const int &id, const std::string &name);
  int GetId();
  std::string GetName();

private:
  int id_;
  std::string name_;
};

#endif