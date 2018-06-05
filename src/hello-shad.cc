#include <iostream>
#include <sstream>

#include "shad/shad.h"
#include "shad/runtime/runtime.h"

namespace shad {

int main(int argc, char *argv[])
{
  rt::executeOnAll(
      [](const size_t&) {
        std::cout << "Hello, SHAD" << std::endl;
      }, size_t(0));

  rt::forEachOnAll(
      [](const size_t&, size_t i) {
        std::stringstream str;
        str << "Hello, SHAD < " << i << std::endl;
        std::cout << str.str();
      }, size_t(0), 100);
  return 0;
}

}
