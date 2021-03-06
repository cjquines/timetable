#include <cassert>
#include <ctime>

#include <iostream>
#include <string>
#include <chrono>

#include "schedule.h"
#include "group.h"

int main() {
  std::ios_base::sync_with_stdio(0);
  std::chrono::time_point<std::chrono::system_clock> start, end, last_ping;
  
  for (int seed = 7700; seed < 10100; seed += 100) {
    std::clog << seed/100 << std::endl;

    std::time_t start_max_5 = std::time(NULL);
    start = std::chrono::system_clock::now();

    Schedule config(5, 8, seed);
    config.AddTeacher(0, "Darilag");
    config.AddTeacher(1, "Catimbang");
    config.AddTeacher(2, "Salvador");
    config.AddTeacher(3, "De Jesus");
    config.AddTeacher(4, "Villareal");
    config.AddSubjectGaps(0);
    config.AddEvenDismissal(1);
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
    grp->AddSubject(0, {2, 2}, 0, "DRR");
    grp->AddSubject(1, {1}, 0, "PE4");
    config.AddSubjectTime(64, 1, {0, 1, 2, 3, 4, 5, 6, 7});
    grp->AddSubject(2, {2, 2}, 1, "MIL");
    grp->AddSubject(3, {2, 2}, 1, "PPL");
    grp->AddSubject(4, {2, 2}, 2, "BIO");
    grp->AddSubject(5, {2, 2}, 2, "III");
    grp->AddSubject(6, {2, 2}, 3, "PHY");
    grp->AddSubject(7, {2, 2}, 4, "CAP");

    config.AddGroup(1);
    grp = config.GetGroup(1);
    grp->AddSection(2, "Hobbes");
    grp->AddSubject(8, {2, 2}, 0, "DRR");
    grp->AddSubject(9, {1}, 0, "PE4");
    config.AddSubjectTime(64, 9, {0, 1, 2, 3, 4, 5, 6, 7});
    grp->AddSubject(10, {2, 2}, 1, "MIL");
    grp->AddSubject(11, {2, 2}, 1, "PPL");
    grp->AddSubject(12, {2, 2}, 2, "BIO");
    grp->AddSubject(13, {2, 2}, 3, "III");
    grp->AddSubject(14, {2, 2}, 3, "PHY");
    grp->AddSubject(15, {2, 2}, 4, "CAP");

    config.AddGroup(2);
    grp = config.GetGroup(2);
    grp->AddSection(3, "11STEM1");
    grp->AddSection(4, "11STEM2");
    grp->AddSection(5, "11STEM3");
    grp->AddSection(6, "11STEM4");
    grp->AddSubject(16, {2, 2}, 5, "RWS");
    grp->AddSubject(17, {2, 2}, 6, "CHM");
    grp->AddSubject(18, {2, 2}, 7, "PPP");
    grp->AddSubject(19, {2, 2}, 8, "PR1");
    grp->AddSubject(20, {1}, 0, "PE2");
    config.AddSubjectTime(64, 20, {0, 1, 2, 3, 4, 5, 6, 7});
    grp->AddSubject(21, {2, 2}, 9, "STA");
    grp->AddSubject(22, {2, 2}, 10, "EAP");
    grp->AddSubject(23, {2, 2}, 11, "EMT");
    grp->AddSubject(24, {2, 2}, 12, "CAL");

    config.AddGroup(3);
    grp = config.GetGroup(3);
    grp->AddSection(7, "11STEM5");
    grp->AddSubject(25, {2, 2}, 5, "RWS");
    grp->AddSubject(26, {2, 2}, 6, "CHM");
    grp->AddSubject(27, {2, 2}, 7, "PPP");
    grp->AddSubject(28, {2, 2}, 4, "PR1");
    grp->AddSubject(29, {1}, 0, "PE2");
    config.AddSubjectTime(64, 29, {0, 1, 2, 3, 4, 5, 6, 7});
    grp->AddSubject(30, {2, 2}, 9, "STA");
    grp->AddSubject(31, {2, 2}, 10, "EAP");
    grp->AddSubject(32, {2, 2}, 11, "EMT");
    grp->AddSubject(33, {2, 2}, 12, "CAL");

    config.Initialize();
    config.InitialSchedule();
    std::cout << config.HardSolver(5) << ": ";

    // if (std::difftime(std::time(NULL), start_max_5) >= 5) {
    //   std::cout << "FAIL HARD " << config.HardCount() << std::endl;
    //   continue;
    // }

    config.SoftInitialize();
    int soft_count = config.SoftCount();

    last_ping = std::chrono::system_clock::now();
    double pinged = 1;

    int loops = 0;
    while (soft_count > 0 && std::difftime(std::time(NULL), start_max_5) < 10) {
      if (loops >= 1000) {
        std::cout << "SA ";
        // output-2: 0.2, 50, 0.99
        soft_count = config.SoftSimulatedAnnealing(10, 0.2, 50, 0.95);
        // soft_count += config.SoftLocalSearch(true, 10);
        // loops = 0;
      } else {
        int delta = config.SoftLocalSearch(true, 0);
        if (delta == 0) loops++;
        else loops = 0;
        soft_count += delta;
      }
      if (std::chrono::duration<double>(std::chrono::system_clock::now() - last_ping).count() > 0.01) {
        std::cout << soft_count << ' ';
        pinged++;
        last_ping = std::chrono::system_clock::now();
      }
    }
    std::cout << std::endl;

    if (std::difftime(std::time(NULL), start_max_5) >= 10) {
      assert(soft_count == config.SoftCount());
      std::cout << "FAIL SOFT " << soft_count << std::endl;
      continue;
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "PASS " << elapsed_seconds.count() << std::endl;
  }
  return 0;
}