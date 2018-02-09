#include <ctime>

#include <iostream>
#include <string>

#include "schedule.h"
#include "group.h"

int main() {
  std::ios_base::sync_with_stdio(0);

  Schedule config(5, 4, std::time(nullptr));
  std::cout << std::time(nullptr) << std::endl;
  // Schedule config(5, 4, 1518139471);
  config.AddTeacher(0, "Darilag");
  config.AddTeacher(1, "Catimbang");
  config.AddTeacher(2, "Salvador");
  config.AddTeacher(3, "De Jesus");
  config.AddTeacher(4, "Villareal");
  config.AddTeacher(5, "Kath");
  config.AddTeacher(6, "Dayrit");
  config.AddTeacher(7, "Fernandez");
  config.AddTeacher(8, "Edz");
  config.AddTeacher(9, "Josh");
  config.AddTeacher(10, "Pheobe");
  config.AddTeacher(11, "Dogma");
  config.AddTeacher(12, "Agustin");
  // config.AddTeacherTime(1, 0, {3, 7, 11, 15, 19});
  config.AddTeacherTime(1, 1, {3, 7, 11, 15, 19});
  config.AddTeacherTime(1, 2, {3, 7, 11, 15, 19});
  config.AddTeacherTime(1, 3, {3, 7, 11, 15, 19});
  config.AddTeacherTime(1, 4, {3, 7, 11, 15, 19});
  config.AddTeacherTime(1, 5, {7, 11, 15, 19});
  config.AddTeacherTime(1, 6, {7, 11, 15, 19});
  config.AddTeacherTime(1, 7, {7, 11, 15, 19});
  config.AddTeacherTime(1, 8, {7, 11, 15, 19});
  config.AddTeacherTime(1, 9, {7, 11, 15, 19});
  config.AddTeacherTime(1, 10, {7, 11, 15, 19});
  config.AddTeacherTime(1, 11, {7, 11, 15, 19});
  config.AddTeacherTime(1, 12, {7, 11, 15, 19});

  config.AddGroup(0);
  auto grp = config.GetGroup(0);
  grp->AddSection(0, "Locke");
  grp->AddSection(1, "Rousseau");
  grp->AddSubject(0, 2, 0, "DRR");
  grp->AddSubject(1, 1, 0, "PE4");
  grp->AddSubject(2, 2, 1, "MIL");
  grp->AddSubject(3, 2, 1, "PPL");
  grp->AddSubject(4, 2, 2, "BIO");
  grp->AddSubject(5, 2, 2, "III");
  grp->AddSubject(6, 2, 3, "PHY");
  grp->AddSubject(7, 2, 4, "CAP");

  config.AddGroup(1);
  grp = config.GetGroup(1);
  grp->AddSection(2, "Hobbes");
  grp->AddSubject(8, 2, 0, "DRR");
  grp->AddSubject(9, 1, 0, "PE4");
  grp->AddSubject(10, 2, 1, "MIL");
  grp->AddSubject(11, 2, 1, "PPL");
  grp->AddSubject(12, 2, 2, "BIO");
  grp->AddSubject(13, 2, 3, "III");
  grp->AddSubject(14, 2, 3, "PHY");
  grp->AddSubject(15, 2, 4, "CAP");

  config.AddGroup(2);
  grp = config.GetGroup(2);
  grp->AddSection(3, "11STEM1");
  grp->AddSection(4, "11STEM2");
  grp->AddSection(5, "11STEM3");
  grp->AddSection(6, "11STEM4");
  grp->AddSubject(16, 2, 5, "RWS");
  grp->AddSubject(17, 1, 6, "CHM");
  grp->AddSubject(18, 2, 7, "PPP");
  grp->AddSubject(19, 2, 8, "PR1");
  grp->AddSubject(20, 2, 0, "PE2");
  grp->AddSubject(21, 2, 9, "STA");
  grp->AddSubject(22, 2, 10, "EAP");
  grp->AddSubject(23, 2, 11, "EMT");
  grp->AddSubject(24, 2, 12, "CAL");

  config.AddGroup(3);
  grp = config.GetGroup(3);
  grp->AddSection(7, "11STEM5");
  grp->AddSubject(25, 2, 5, "RWS");
  grp->AddSubject(26, 1, 6, "CHM");
  grp->AddSubject(27, 2, 7, "PPP");
  grp->AddSubject(28, 2, 4, "PR1");
  grp->AddSubject(29, 2, 0, "PE2");
  grp->AddSubject(30, 2, 9, "STA");
  grp->AddSubject(31, 2, 10, "EAP");
  grp->AddSubject(32, 2, 11, "EMT");
  grp->AddSubject(33, 2, 12, "CAL");

  config.Initialize();
  config.InitialSchedule();
  config.TestPrint();
  int hard_count = config.HardCount();
  while (hard_count > 0) hard_count += config.HardSolver();
  config.SoftInitialize();
  config.TestPrint();
  int soft_count = config.SoftCount();
  while (soft_count > 1) soft_count += config.SoftSolver();
  config.TestPrint();
  return 0;
}