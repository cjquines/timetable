#include "subject.h"

Subject::Subject(const int &id, const int &num_slots, const int &teacher,
                 const std::string &name)
    : id_(id),
      num_slots_(num_slots),
      teacher_(teacher),
      name_(name) {}

int Subject::GetId() { return id_; }

int Subject::GetNumSlots() { return num_slots_; }

int Subject::GetTeacher() { return teacher_; }

std::string Subject::GetName() { return name_; }