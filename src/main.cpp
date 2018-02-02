#include <ctime>

#include <iostream>
#include <string>

#include "schedule.h"
#include "group.h"

int main() {
  // std::cout << std::time(nullptr) << std::endl;
  Schedule config(5, 4, std::time(nullptr));
  // std::cout << std::time(nullptr) << std::endl;
  // Schedule config(5, 3, 1517567544);
  config.AddTeacher(0, "Darilag");
  config.AddTeacher(1, "Catimbang");
  config.AddTeacher(2, "Salvador");
  config.AddTeacher(3, "De Jesus");
  config.AddTeacher(4, "Villareal");
  config.AddGroup(0);
  auto grp = config.GetGroup(0);
  grp->AddSection(0, "Locke");
  grp->AddSection(1, "Hobbes");
  grp->AddSection(2, "Rousseau");
  grp->AddSubject(0, 2, 0, "DRR");
  grp->AddSubject(1, 1, 0, "PE4");
  grp->AddSubject(2, 2, 1, "MIL");
  grp->AddSubject(3, 2, 1, "PPL");
  grp->AddSubject(4, 2, 2, "BIO");
  grp->AddSubject(5, 2, 2, "III");
  grp->AddSubject(6, 2, 3, "PHY");
  grp->AddSubject(7, 2, 4, "CAP");
  config.Initialize();
  config.InitialSchedule();
  config.TestPrint();
  config.HardSolver(config.HardCount());
  if (config.HardCount() == 0) config.SoftInitialize();
  config.TestPrint();
  return 0;
}