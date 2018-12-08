#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>

#include "emitter.h"
#include "parser.h"
#include "schedule.h"
#include "solver.h"

int main(int argc, char** argv) {
  try {
    if (argc <= 1)
      throw std::runtime_error("program run without input.");

    Parser parser(argv[1]);
    parser.ParseFile();
    Schedule* config = parser.GetSchedule();
    Solver solver(config, parser.GetSeed());

    int num_schedules = solver.Solve(parser.GetSeconds(), parser.GetAttempts(),
                                     parser.GetTop(), parser.GetNumSamples(),
                                     parser.GetKappa(), parser.GetTau(),
                                     parser.GetAlpha());

    for (int i = 0; i < num_schedules; i++) {
      int soft_count;
      Schedule* schedule;
      std::tie(soft_count, schedule) = solver.GetBestSchedule(i);
      Emitter emitter(schedule);
      emitter.OutputSchedule("output-" + std::to_string(soft_count) + ".xml");
    }
  } catch (const std::runtime_error &error) {
    std::cerr << "ERROR: " << error.what() << std::endl;
  }

  return 0;
}