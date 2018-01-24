#ifndef _TIMETABLING_SUBJECT_H
#define _TIMETABLING_SUBJECT_H

#include <string>

class Subject {
public:
  Subject(const int &id, const int &num_slots, const int &teacher,
    const std::string &name);
  inline int GetId();
  inline int GetNumSlots();
  inline int GetTeacher();
  inline std::string GetName();
private:
  int id_;
  int num_slots_;
  int teacher_;
  std::string name_;
};

inline int Subject::GetId() { return id_; }

inline int Subject::GetNumSlots() { return num_slots_; }

inline int Subject::GetTeacher() { return teacher_; }

inline std::string Subject::GetName() { return name_; }

#endif