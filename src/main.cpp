#include <cassert>
#include <ctime>

#include <iostream>
#include <string>

#include "schedule.h"
#include "group.h"

#include "yaml-cpp/yaml.h"

int num_groups;
int num_teachers;
int num_sections;
int num_subjects;
std::map<std::string, int> sections_dict;
std::map<std::string, int> teachers_dict;

int main(int argc, char** argv) {
  YAML::Node input = YAML::LoadFile(argv[1]);

  Schedule config(input["days"].as<int>(), input["slots"].as<int>(),
                  std::time(nullptr));

  for (auto it : input["teachers"]) {
    std::string name = it["name"].as<std::string>();
    config.AddTeacher(num_teachers, name);

    if (it["unassignable"])
      config.AddTeacherTime(it["unassignable"]["priority"].as<int>(),
                            num_teachers,
                            it["unassignable"]["times"].as< std::vector<int> >());

    teachers_dict[name] = num_teachers++;
  }

  for (auto it : input["groups"]) {
    config.AddGroup(num_groups);
    auto grp = config.GetGroup(num_groups);

    for (auto jt : it["sections"]) {
      std::string name = jt["name"].as<std::string>();
      grp->AddSection(num_sections, name);
      sections_dict[name] = num_sections++;
    }

    for (auto jt : it["subjects"]) {
      std::string name = jt["name"].as<std::string>();
      int teacher = teachers_dict[jt["teacher"].as<std::string>()];
      std::vector<int> times = jt["times"].as< std::vector<int> >();
      grp->AddSubject(num_subjects, times, teacher, name);

      if (jt["unassignable"])
        config.AddSubjectTime(jt["unassignable"]["priority"].as<int>(),
                              num_subjects,
                              jt["unassignable"]["times"].as< std::vector<int> >());

      num_subjects++;
    }

    num_groups++;
  }

  for (auto it : input["constraints"]) {
    std::string type = it["type"].as<std::string>();
    int priority = it["priority"].as<int>();

    if (type == "distinctPerDay") {
      config.AddDistinctPerDay(priority);
    } else if (type == "evenDismissal") {
      std::vector<std::string> sections
        = it["sections"].as< std::vector<std::string> >();
      std::vector<int> section_ids;

      for (auto jt : sections)
        section_ids.push_back(sections_dict[jt]);

      config.AddEvenDismissal(priority, section_ids);
    } else if (type == "minSubjects") {
      config.AddMinSubjects(priority, it["minSubjects"].as<int>());
    } else if (type == "nonSimultaneous") {
      config.AddNonSimultaneous(priority);
    } else if (type == "reqFirstSubject") {
      config.AddReqFirstSubject(priority);
    }
  }

  config.Solve(input["seconds"].as<int>(), input["attempts"].as<int>());
  config.TestPrint();

  return 0;
}