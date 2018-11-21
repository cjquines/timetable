#include <iostream>
#include <stdexcept>
#include <string>

#include "emitter.h"
#include "parser.h"
#include "schedule.h"

#include "yaml-cpp/yaml.h"

int main(int argc, char** argv) {
  try {
    if (argc <= 1)
      throw std::runtime_error("program run without input.");

    Parser parser(argv[1]);
    parser.ParseFile();
    Schedule* config = parser.GetSchedule();

    config->Solve(parser.GetSeconds(), parser.GetAttempts());
    config->TestPrint();

    Emitter emitter(config);
    emitter.OutputSchedule("output.xml");
  } catch (const std::runtime_error &error) {
    std::cerr << "ERROR: " << error.what() << std::endl;
  }

  return 0;
}