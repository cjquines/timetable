#include "group.h"

Group::Group(int id) : id_(id) {}

int Group::GetId() { return id_; }

int Group::GetNumSections() { return sections_.size(); }

int Group::GetNumSubjects() { return subjects_.size(); }

Section* Group::GetSection(int idx) { return sections_[idx]; }

std::vector<Section*>::iterator Group::GetSectionsBegin() {
  return sections_.begin();
}

std::vector<Section*>::iterator Group::GetSectionsEnd() {
  return sections_.end();
}

Subject* Group::GetSubject(int idx) { return subjects_[idx]; }

std::vector<Subject*>::iterator Group::GetSubjectsBegin() {
  return subjects_.begin();
}

std::vector<Subject*>::iterator Group::GetSubjectsEnd() {
  return subjects_.end();
}

void Group::AddSection(int id, const std::string &name) {
  Section* ptr = new Section(id, name);
  sections_.push_back(ptr);
}

void Group::AddSubject(int id, const std::vector<int> &slots,
                       int teacher, const std::string &name) {
  Subject* ptr = new Subject(id, slots, teacher, name);
  subjects_.push_back(ptr);
}