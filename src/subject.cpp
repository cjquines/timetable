#include "subject.h"

Subject::Subject(const int &id, const std::vector<int> &slots,
                 const int &teacher, const std::string &name)
    : id_(id),
      slots_(slots),
      teacher_(teacher),
      name_(name) {}

int Subject::GetId() { return id_; }

std::vector<int>::iterator Subject::GetSlotsBegin() { return slots_.begin(); }

std::vector<int>::iterator Subject::GetSlotsEnd() { return slots_.end(); }

int Subject::GetTeacher() { return teacher_; }

std::string Subject::GetName() { return name_; }