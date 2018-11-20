#ifndef _TIMETABLE_PARSER_H
#define _TIMETABLE_PARSER_H

#include <map>
#include <string>

#include "schedule.h"

#include "yaml-cpp/yaml.h"

class Parser {
public:
  Parser(const char* filename);

  Schedule* GetSchedule();
  int GetSeconds();
  int GetAttempts();

  void ReadTeachers();
  void ReadGroups();
  void ReadConstraints();
  void ParseFile();

private:
  YAML::Node input_;
  Schedule* schedule_;

  int num_groups_;
  int num_teachers_;
  int num_sections_;
  int num_subjects_;

  std::map<std::string, int> sections_dict_;
  std::map<std::string, int> teachers_dict_;
};

#endif