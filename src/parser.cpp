#include <ctime>

#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

#include "parser.h"
#include "group.h"
#include "schedule.h"

#include "yaml-cpp/yaml.h"

Parser::Parser(const char* filename) : num_groups_(0), num_teachers_(0),
      num_sections_(0), num_subjects_(0), priority_factor_(1) {
  std::ifstream file(filename);
  if (!file.good())
    throw std::runtime_error("the file " + std::string(filename) + " doesn't exist.");

  input_ = YAML::LoadFile(filename);

  int seed;
  if (input_["seed"]) seed = input_["seed"].as<int>();
  else seed = std::time(nullptr);

  std::ofstream log;
  log.open("log.txt");
  log << "Seed: " << seed << std::endl;
  log.close();

  if (!input_["days"])
    throw std::runtime_error("property 'days' not found in input file.");
  if (!input_["days"].IsScalar())
    throw std::runtime_error("property 'days' doesn't look like an integer.");
  if (!input_["slots"])
    throw std::runtime_error("property 'slots' not found in input file.");
  if (!input_["slots"].IsScalar())
    throw std::runtime_error("property 'slots' doesn't look like an integer.");

  schedule_ = new Schedule(input_["days"].as<int>(), input_["slots"].as<int>(),
                           seed);
}

Schedule* Parser::GetSchedule() { return schedule_; }

int Parser::GetSeconds() {
  if (!input_["seconds"])
    throw std::runtime_error("property 'seconds' not found in input file.");
  if (!input_["seconds"].IsScalar())
    throw std::runtime_error("property 'seconds' doesn't look like an integer.");
  return input_["seconds"].as<int>();
}

int Parser::GetAttempts() {
  if (!input_["attempts"])
    throw std::runtime_error("property 'attempts' not found in input file.");
  if (!input_["attempts"].IsScalar())
    throw std::runtime_error("property 'attempts' doesn't look like an integer.");
  return input_["attempts"].as<int>();
}

int Parser::GetTop() {
  if (!input_["top"])
    throw std::runtime_error("property 'top' not found in input file.");
  if (!input_["top"].IsScalar())
    throw std::runtime_error("property 'top' doesn't look like an integer.");
  return input_["top"].as<int>();
}

void Parser::ReadTeachers() {
  if (!input_["teachers"])
    throw std::runtime_error("property 'teachers' not found in input file.");
  if (!input_["teachers"].IsSequence())
    throw std::runtime_error("property 'teachers' isn't a sequence of teachers.");

  for (auto it : input_["teachers"]) {
    if (!it["name"])
      throw std::runtime_error("teacher " + std::to_string(num_teachers_)
                             + " doesn't have a name.");
    if (!it["name"].IsScalar())
      throw std::runtime_error("teacher " + std::to_string(num_teachers_)
                             + "'s name doesn't look like a name.");

    std::string name = it["name"].as<std::string>();

    if (teachers_dict_.find(name) != teachers_dict_.end())
      throw std::runtime_error(name + " is listed more than once among the teachers.");

    schedule_->AddTeacher(num_teachers_, name);

    if (it["unassignable"]) {
      if (!it["unassignable"]["priority"])
        throw std::runtime_error(
          name + "'s unassignable constraint doesn't have a priority.");
      if (!it["unassignable"]["priority"].IsScalar())
        throw std::runtime_error(
          name + "'s unassignable priority doesn't look like an integer.");
      if (!it["unassignable"]["times"])
        throw std::runtime_error(
          name + "'s unassignable constraint doesn't have a list of times.");
      if (!it["unassignable"]["times"].IsSequence())
        throw std::runtime_error(
          name + "'s unassignable times doesn't look like a list.");

      int priority = it["unassignable"]["priority"].as<int>();
      if (priority > 0) priority *= priority_factor_;

      schedule_->AddTeacherTime(priority, num_teachers_,
                                it["unassignable"]["times"].as< std::vector<int> >());
    }

    teachers_dict_[name] = num_teachers_++;
  }
}

void Parser::ReadGroups() {
  if (!input_["groups"])
    throw std::runtime_error("property 'groups' not found in input file.");
  if (!input_["groups"].IsSequence())
    throw std::runtime_error("property 'groups' isn't a sequence of teachers.");

  for (auto it : input_["groups"]) {
    if (!it["sections"])
      throw std::runtime_error("group " + std::to_string(num_groups_)
                             + " doesn't have a list of sections.");
    if (!it["sections"].IsSequence())
      throw std::runtime_error("group " + std::to_string(num_groups_)
                             + "'s sections isn't a sequence of sections.");
    if (!it["subjects"])
      throw std::runtime_error("group " + std::to_string(num_groups_)
                             + " doesn't have a list of subjects.");
    if (!it["subjects"].IsSequence())
      throw std::runtime_error("group " + std::to_string(num_groups_)
                             + "'s subjects isn't a sequence of subjects.");

    schedule_->AddGroup(num_groups_);
    auto grp = schedule_->GetGroup(num_groups_);

    for (auto jt : it["sections"]) {
      if (!jt["name"])
        throw std::runtime_error("one of group " + std::to_string(num_groups_)
                               + "' sections doesn't have a name.");
      if (!jt["name"].IsScalar())
        throw std::runtime_error("one of group " + std::to_string(num_groups_)
                               + "' sections' names doesn't look like a name.");

      std::string name = jt["name"].as<std::string>();

      if (sections_dict_.find(name) != sections_dict_.end())
        throw std::runtime_error(name + " is listed more than once among the sections.");

      grp->AddSection(num_sections_, name);
      sections_dict_[name] = num_sections_++;
    }

    for (auto jt : it["subjects"]) {
      if (!jt["name"])
        throw std::runtime_error("one of group " + std::to_string(num_groups_)
                               + "' subjects doesn't have a name.");
      if (!jt["name"].IsScalar())
        throw std::runtime_error("one of group " + std::to_string(num_groups_)
                               + "' subjects' names doesn't look like a name.");

      std::string name = jt["name"].as<std::string>();

      if (!jt["teacher"])
        throw std::runtime_error(name + " of group " + std::to_string(num_groups_)
                               + " doesn't have a teacher.");
      if (!jt["teacher"].IsScalar())
        throw std::runtime_error(name + " of group " + std::to_string(num_groups_)
                               + "'s teacher doesn't look like a name.");

      auto kt = teachers_dict_.find(jt["teacher"].as<std::string>());
      if (kt == teachers_dict_.end())
        throw std::runtime_error(jt["teacher"].as<std::string>() + " of subject " + name
                               + " of group " + std::to_string(num_groups_)
                               + " isn't mentioned earlier.");

      int teacher = kt->second;
      std::vector<int> times = jt["times"].as< std::vector<int> >();
      grp->AddSubject(num_subjects_, times, teacher, name);

      if (jt["unassignable"]) {
        if (!jt["unassignable"]["priority"])
          throw std::runtime_error(name + " of group " + std::to_string(num_groups_)
                                 + "'s unassignable constraint doesn't have a priority.");
        if (!jt["unassignable"]["priority"].IsScalar())
          throw std::runtime_error(name + " of group " + std::to_string(num_groups_)
                                 + "'s unassignable priority doesn't look like an integer.");
        if (!jt["unassignable"]["times"])
          throw std::runtime_error(name + " of group " + std::to_string(num_groups_)
                                 + "'s unassignable constraint doesn't have a list of times.");
        if (!jt["unassignable"]["times"].IsSequence())
          throw std::runtime_error(name + " of group " + std::to_string(num_groups_)
                                 + "'s unassignable times doesn't look like a list.");

        int priority = jt["unassignable"]["priority"].as<int>();
        if (priority > 0) priority *= priority_factor_;

        schedule_->AddSubjectTime(priority, num_subjects_,
                                  jt["unassignable"]["times"].as< std::vector<int> >());
      }

      num_subjects_++;
    }

    num_groups_++;
  }
}

void Parser::ReadConstraints() {
  if (!input_["constraints"])
    throw std::runtime_error("property 'constraints' not found in input file.");
  if (!input_["constraints"].IsSequence())
    throw std::runtime_error("property 'constraints' isn't a sequence of constraints.");

  for (auto it : input_["constraints"]) {
    if (!it["type"])
      throw std::runtime_error("one of the constraints doesn't have a type.");
    if (!it["type"].IsScalar())
      throw std::runtime_error("one of the constraint's names doesn't look like a name.");

    std::string type = it["type"].as<std::string>();

    if (!it["priority"])
      throw std::runtime_error("one of the " + type
                             + " constraints doesn't have a priority.");
    if (!it["priority"].IsScalar())
      throw std::runtime_error("one of the " + type
                             + "'s priorities doesn't look like a priority.");

    int priority = it["priority"].as<int>();
    if (priority > 0) priority *= priority_factor_;

    if (type == "distinctPerDay") {
      schedule_->AddDistinctPerDay(priority);
    } else if (type == "evenDismissal") {
      if (!it["sections"])
        throw std::runtime_error("one of the " + type
                               + " constraints doesn't have a list of sections.");
      if (!it["sections"].IsSequence())
        throw std::runtime_error("one of the " + type
                               + "'s sections isn't a list of sections.");

      std::vector<std::string> sections
        = it["sections"].as< std::vector<std::string> >();
      std::vector<int> section_ids;

      for (auto jt : sections) {
        auto kt = sections_dict_.find(jt);

        if (kt == sections_dict_.end())
          throw std::runtime_error("section " + jt + " of the " + type
                                 + " constraint isn't mentioned earlier.");

        section_ids.push_back(kt->second);
      }

      priority /= sections.size() * sections.size();
      schedule_->AddEvenDismissal(priority, section_ids);
    } else if (type == "minSubjects") {
      if (!it["minSubjects"])
        throw std::runtime_error("one of the " + type
                               + " constraints doesn't have a minSubjects.");
      if (!it["minSubjects"].IsScalar())
        throw std::runtime_error("one of the " + type
                               + "'s minSubjects doesn't look like an integer.");

      schedule_->AddMinSubjects(priority, it["minSubjects"].as<int>());
    } else if (type == "nonSimultaneous") {
      schedule_->AddNonSimultaneous(priority);
    } else if (type == "reqFirstSubject") {
      schedule_->AddReqFirstSubject(priority);
    } else if (type == "subjectGaps") {
      schedule_->AddSubjectGaps(priority);
    } else {
      throw std::runtime_error("type " + type + " isn't recognized.");
    }
  }
}

void Parser::ParseFile() {
  if (input_["constraints"].IsSequence()) {
    for (auto it : input_["constraints"]) {
      if (it["type"] && it["type"].IsScalar()
          && it["type"].as<std::string>() == "evenDismissal"
          && it["sections"].IsSequence()) {
        if (it["priority"].as<int>() <= 0) continue;
        priority_factor_ *= it["sections"].size() * it["sections"].size();
      }
    }
  }

  ReadTeachers();
  ReadGroups();
  ReadConstraints();
}