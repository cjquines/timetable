#include <cassert>
#include <ctime>

#include <iostream>
#include <string>

#include "schedule.h"
#include "group.h"

int main() {
  // std::time_t start_max_5 = std::time(NULL);

  std::ios_base::sync_with_stdio(0);
  // Schedule config(5, 14, std::time(nullptr));
  // std::cout << std::time(nullptr) << std::endl;
  Schedule config(5, 14, 1542636971);
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
  // config.AddTeacherTime(1, 0, {6, 7, 14, 15, 22, 23, 30, 31, 38, 39});
  // config.AddTeacherTime(1, 1, {6, 7, 14, 15, 22, 23, 30, 31, 38, 39});
  // config.AddTeacherTime(1, 2, {6, 7, 14, 15, 22, 23, 30, 31, 38, 39});
  // config.AddTeacherTime(1, 3, {6, 7, 14, 15, 22, 23, 30, 31, 38, 39});
  // config.AddTeacherTime(1, 4, {6, 7, 14, 15, 22, 23, 30, 31, 38, 39});
  // config.AddTeacherTime(1,  5, {14, 15, 22, 23, 30, 31, 38, 39});
  // config.AddTeacherTime(1,  6, {14, 15, 22, 23, 30, 31, 38, 39});
  // config.AddTeacherTime(1,  7, {14, 15, 22, 23, 30, 31, 38, 39});
  // config.AddTeacherTime(1,  8, {14, 15, 22, 23, 30, 31, 38, 39});
  // config.AddTeacherTime(1,  9, {14, 15, 22, 23, 30, 31, 38, 39});
  // config.AddTeacherTime(1, 10, {14, 15, 22, 23, 30, 31, 38, 39});
  // config.AddTeacherTime(1, 11, {14, 15, 22, 23, 30, 31, 38, 39});
  // config.AddTeacherTime(1, 12, {14, 15, 22, 23, 30, 31, 38, 39});

  config.AddGroup(0);
  auto grp = config.GetGroup(0);
  grp->AddSection(0, "Locke");
  grp->AddSection(1, "Rousseau");
  grp->AddSubject(0, {3, 3, 2}, 0, "DRR");
  grp->AddSubject(1, {2}, 0, "PE4");
  config.AddSubjectTime(225, 1, {0, 1, 2, 3, 4, 5, 6, 7});
  grp->AddSubject(2, {3, 3, 2}, 1, "MIL");
  grp->AddSubject(3, {3, 3, 2}, 1, "PPL");
  grp->AddSubject(4, {3, 3, 2}, 2, "BIO");
  grp->AddSubject(5, {3, 3, 2}, 2, "III");
  grp->AddSubject(6, {3, 3, 2}, 3, "PHY");
  grp->AddSubject(7, {3, 3, 2}, 4, "CAP");

  config.AddGroup(1);
  grp = config.GetGroup(1);
  grp->AddSection(2, "Hobbes");
  grp->AddSubject(8, {3, 3, 2}, 0, "DRR");
  grp->AddSubject(9, {2}, 0, "PE4");
  config.AddSubjectTime(225, 9, {0, 1, 2, 3, 4, 5, 6, 7});
  grp->AddSubject(10, {3, 3, 2}, 1, "MIL");
  grp->AddSubject(11, {3, 3, 2}, 1, "PPL");
  grp->AddSubject(12, {3, 3, 2}, 2, "BIO");
  grp->AddSubject(13, {3, 3, 2}, 3, "III");
  grp->AddSubject(14, {3, 3, 2}, 3, "PHY");
  grp->AddSubject(15, {3, 3, 2}, 4, "CAP");

  config.AddGroup(2);
  grp = config.GetGroup(2);
  grp->AddSection(3, "11STEM1");
  grp->AddSection(4, "11STEM2");
  grp->AddSection(5, "11STEM3");
  grp->AddSection(6, "11STEM4");
  grp->AddSubject(16, {3, 3, 2}, 5, "RWS");
  grp->AddSubject(17, {3, 3, 2}, 6, "CHM");
  grp->AddSubject(18, {3, 3, 2}, 7, "PPP");
  grp->AddSubject(19, {3, 3, 2}, 8, "PR1");
  grp->AddSubject(20, {2}, 0, "PE2");
  config.AddSubjectTime(225, 20, {0, 1, 2, 3, 4, 5, 6, 7});
  grp->AddSubject(21, {3, 3, 2}, 9, "STA");
  grp->AddSubject(22, {3, 3, 2}, 10, "EAP");
  grp->AddSubject(23, {3, 3, 2}, 11, "EMT");
  grp->AddSubject(24, {3, 3, 2}, 12, "CAL");

  config.AddGroup(3);
  grp = config.GetGroup(3);
  grp->AddSection(7, "11STEM5");
  grp->AddSubject(25, {3, 3, 2}, 5, "RWS");
  grp->AddSubject(26, {3, 3, 2}, 6, "CHM");
  grp->AddSubject(27, {3, 3, 2}, 7, "PPP");
  grp->AddSubject(28, {3, 3, 2}, 4, "PR1");
  grp->AddSubject(29, {2}, 0, "PE2");
  config.AddSubjectTime(225, 29, {0, 1, 2, 3, 4, 5, 6, 7});
  grp->AddSubject(30, {3, 3, 2}, 9, "STA");
  grp->AddSubject(31, {3, 3, 2}, 10, "EAP");
  grp->AddSubject(32, {3, 3, 2}, 11, "EMT");
  grp->AddSubject(33, {3, 3, 2}, 12, "CAL");

  config.AddSubjectGaps(0);
  config.AddMinSubjects(0, 2);
  config.AddEvenDismissal(25, {0, 1, 2});
  config.AddEvenDismissal(9, {3, 4, 5, 6, 7});
  config.AddDistinctPerDay(0);
  config.AddNonSimultaneous(0);
  config.AddReqFirstSubject(0);
  config.Solve(5, 1);
  config.TestPrint();

  return 0;
}