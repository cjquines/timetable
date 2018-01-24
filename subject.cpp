#include "subject.h"

Subject::Subject(const int &id, const int &num_slots, const int &teacher,
  const std::string &name) : id_(id), num_slots_(num_slots), teacher_(teacher),
  name_(name) {}