#ifndef _TIMETABLE_GROUP_H
#define _TIMETABLE_GROUP_H

#include <string>
#include <vector>

#include "section.h"
#include "subject.h"

class Group {
public:
  Group(int id);
  
  int GetId();
  int GetNumSections();
  int GetNumSubjects();

  Section* GetSection(int idx);
  std::vector<Section*>::iterator GetSectionsBegin();
  std::vector<Section*>::iterator GetSectionsEnd();

  Subject* GetSubject(int idx);
  std::vector<Subject*>::iterator GetSubjectsBegin();
  std::vector<Subject*>::iterator GetSubjectsEnd();

  void AddSection(int id, const std::string &name);
  void AddSubject(int id, const std::vector<int> &slots, int teacher,
                  const std::string &name);

private:
  int id_;
  std::vector<Section*> sections_;
  std::vector<Subject*> subjects_;
};

#endif