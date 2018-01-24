#include "subject.h"

Subject::Subject(const int &id, const int &num_slots, const int &teacher,
                 const std::string &name)
    : id_(id),
      num_slots_(num_slots),
      teacher_(teacher),
      name_(name) {}

inline int Subject::GetId() { return id_; }

inline int Subject::GetNumSlots() { return num_slots_; }

inline int Subject::GetTeacher() { return teacher_; }

inline std::string Subject::GetName() { return name_; }