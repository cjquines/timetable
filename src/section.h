#ifndef _TIMETABLE_SECTION_H
#define _TIMETABLE_SECTION_H

#include <string>

class Section {
public:
  Section(const int &id, const std::string &name);
  int GetId();
  std::string GetName();

private:
  int id_;
  std::string name_;
};

#endif