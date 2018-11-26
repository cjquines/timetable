#include "section.h"

Section::Section(int id, const std::string &name)
  : id_(id), name_(name) {}

int Section::GetId() { return id_; }

std::string Section::GetName() { return name_; }