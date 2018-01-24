#ifndef _TIMETABLING_SECTION_H
#define _TIMETABLING_SECTION_H

#include <string>

class Section {
public:
  Section(const int &id, const std::string &name);
  inline int GetId();
  inline std::string GetName();
private:
  int id_;
  std::string name_;
};

inline int Section::GetId() { return id_; }

inline std::string Section::GetName() { return name_; }

#endif