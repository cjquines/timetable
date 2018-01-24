#include <iostream>
#include <string>
#include "section.h"

int main() {
  Section a(0, "1");
  std::cout << a.GetId() << std::endl;
  return 0;
}