#include <ctime>

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "parser.h"
#include "group.h"
#include "schedule.h"

#include "yaml-cpp/yaml.h"

Parser::Parser(const char* filename)
    : num_groups_(0), num_teachers_(0), num_sections_(0), num_subjects_(0) {
  input_ = YAML::LoadFile(filename);

  int seed;
  if (input_["seed"]) seed = input_["seed"].as<int>();
  else seed = std::time(nullptr);

  std::ofstream log;
  log.open("log.txt");
  log << "Seed: " << seed << std::endl;
  log.close();

  schedule_ = new Schedule(input_["days"].as<int>(), input_["slots"].as<int>(),
                           seed);
}

Schedule* Parser::GetSchedule() { return schedule_; }

int Parser::GetSeconds() { return input_["seconds"].as<int>(); }

int Parser::GetAttempts() { return input_["attempts"].as<int>(); }

void Parser::ReadTeachers() {
  for (auto it : input_["teachers"]) {
    std::string name = it["name"].as<std::string>();
    schedule_->AddTeacher(num_teachers_, name);

    if (it["unassignable"])
      schedule_->AddTeacherTime(it["unassignable"]["priority"].as<int>(),
                                num_teachers_,
                                it["unassignable"]["times"].as< std::vector<int> >());

    teachers_dict_[name] = num_teachers_++;
  }
}

void Parser::ReadGroups() {
  for (auto it : input_["groups"]) {
    schedule_->AddGroup(num_groups_);
    auto grp = schedule_->GetGroup(num_groups_);

    for (auto jt : it["sections"]) {
      std::string name = jt["name"].as<std::string>();
      grp->AddSection(num_sections_, name);
      sections_dict_[name] = num_sections_++;
    }

    for (auto jt : it["subjects"]) {
      std::string name = jt["name"].as<std::string>();
      int teacher = teachers_dict_[jt["teacher"].as<std::string>()];
      std::vector<int> times = jt["times"].as< std::vector<int> >();
      grp->AddSubject(num_subjects_, times, teacher, name);

      if (jt["unassignable"])
        schedule_->AddSubjectTime(jt["unassignable"]["priority"].as<int>(),
                                  num_subjects_,
                                  jt["unassignable"]["times"].as< std::vector<int> >());

      num_subjects_++;
    }

    num_groups_++;
  }
}

void Parser::ReadConstraints() {
  for (auto it : input_["constraints"]) {
    std::string type = it["type"].as<std::string>();
    int priority = it["priority"].as<int>();

    if (type == "distinctPerDay") {
      schedule_->AddDistinctPerDay(priority);
    } else if (type == "evenDismissal") {
      std::vector<std::string> sections
        = it["sections"].as< std::vector<std::string> >();
      std::vector<int> section_ids;

      for (auto jt : sections)
        section_ids.push_back(sections_dict_[jt]);

      schedule_->AddEvenDismissal(priority, section_ids);
    } else if (type == "minSubjects") {
      schedule_->AddMinSubjects(priority, it["minSubjects"].as<int>());
    } else if (type == "nonSimultaneous") {
      schedule_->AddNonSimultaneous(priority);
    } else if (type == "reqFirstSubject") {
      schedule_->AddReqFirstSubject(priority);
    }
  }
}

void Parser::ParseFile() {
  ReadTeachers();
  ReadGroups();
  ReadConstraints();
}