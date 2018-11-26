#include "teacher.h"

Teacher::Teacher(int id, const std::string &name)
  : id_(id), name_(name) {}

int Teacher::GetId() { return id_; }

std::string Teacher::GetName() { return name_; }