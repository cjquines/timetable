#include "group.h"

Group::Group(int id) : id_(id) {}

int Group::GetId() { return id_; }

Section* Group::GetSection(int idx) { return sections_[idx]; }

const std::vector<Section*>& Group::GetSections() const { return sections_; }

Subject* Group::GetSubject(int idx) { return subjects_[idx]; }

const std::vector<Subject*>& Group::GetSubjects() const { return subjects_; }

void Group::AddSection(int id, const std::string &name) {
  Section* ptr = new Section(id, name);
  sections_.push_back(ptr);
}

void Group::AddSubject(int id, const std::vector<int> &slots,
                       int teacher, const std::string &name) {
  Subject* ptr = new Subject(id, slots, teacher, name);
  subjects_.push_back(ptr);
}