#include "subject.h"

Subject::Subject(int id, const std::vector<int> &slots,
                 int teacher, const std::string &name)
    : id_(id), slots_(slots), teacher_(teacher), name_(name) {}

int Subject::GetId() { return id_; }

const std::vector<int>& Subject::GetSlots() const { return slots_; }

int Subject::GetTeacher() { return teacher_; }

std::string Subject::GetName() { return name_; }