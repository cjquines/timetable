#ifndef _TIMETABLE_SUBJECT_H
#define _TIMETABLE_SUBJECT_H

#include <string>
#include <vector>

class Subject {
public:
  Subject(int id, const std::vector<int> &slots, int teacher,
          const std::string &name);
  int GetId();
  const std::vector<int>& GetSlots() const;
  int GetTeacher();
  std::string GetName();

private:
  int id_;
  std::vector<int> slots_;
  int teacher_;
  std::string name_;
};

#endif