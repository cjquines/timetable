#include "group.h"

Group::Group(const int &id) : id_(id) {}

int Group::GetId() { return id_; }

int Group::GetNumSections() { return sections_.size(); }

int Group::GetNumSubjects() { return subjects_.size(); }

std::vector<Section*>::iterator Group::GetSectionsBegin() {
  return sections_.begin();
}

std::vector<Section*>::iterator Group::GetSectionsEnd() {
  return sections_.end();
}

std::vector<Subject*>::iterator Group::GetSubjectsBegin() {
  return subjects_.begin();
}

std::vector<Subject*>::iterator Group::GetSubjectsEnd() {
  return subjects_.end();
}

void Group::AddSection(const int &id, const std::string &name) {
  Section* ptr = new Section(id, name);
  sections_.push_back(ptr);
}

void Group::AddSubject(const int &id, const int &num_slots, const int &teacher,
                       const std::string &name) {
  Subject* ptr = new Subject(id, num_slots, teacher, name);
  subjects_.push_back(ptr);
}