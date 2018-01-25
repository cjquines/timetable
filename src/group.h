#ifndef _TIMETABLING_GROUP_H
#define _TIMETABLING_GROUP_H

#include <iterator>
#include <string>
#include <vector>

#include "section.h"
#include "subject.h"

class Group {
public:
  Group(const int &id);
  int GetId();
  int GetNumSections();
  int GetNumSubjects();
  std::vector<Section*>::iterator GetSectionsBegin();
  std::vector<Section*>::iterator GetSectionsEnd();
  std::vector<Subject*>::iterator GetSubjectsBegin();
  std::vector<Subject*>::iterator GetSubjectsEnd();
  void AddSection(const int &id, const std::string &name);
  void AddSubject(const int &id, const int &num_slots, const int &teacher,
                  const std::string &name);

private:
  int id_;
  std::vector<Section*> sections_;
  std::vector<Subject*> subjects_;
};

#endif