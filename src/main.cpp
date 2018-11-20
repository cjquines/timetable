#include <cassert>

#include <iostream>
#include <string>

#include "parser.h"
#include "schedule.h"

#include "yaml-cpp/yaml.h"

int main(int argc, char** argv) {
  Parser parser(argv[1]);
  parser.ParseFile();
  Schedule* config = parser.GetSchedule();

  config->Solve(parser.GetSeconds(), parser.GetAttempts());
  config->TestPrint();

  return 0;
}