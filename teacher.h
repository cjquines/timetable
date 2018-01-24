#ifndef _TIMETABLING_TEACHER_H
#define _TIMETABLING_TEACHER_H

#include <string>

class Teacher {
public:
  Teacher(const int &id, const std::string &name);
  inline int GetId();
  inline std::string GetName();
private:
  int id_;
  std::string name_;
};

inline int Teacher::GetId() { return id_; }

inline std::string Teacher::GetName() { return name_; }

#endif