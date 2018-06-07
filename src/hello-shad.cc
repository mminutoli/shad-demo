#include <iostream>
#include <sstream>

#include "shad/shad.h"
#include "shad/runtime/runtime.h"

void helloTask(const size_t &v) {
  std::stringstream SS;
  SS << shad::rt::thisLocality() << "-> Hello, SHAD : " << v << std::endl;
  std::cout << SS.str();
}

namespace shad {

int main(int argc, char *argv[])
{
  for (auto l : rt::allLocalities())
    rt::executeAt(l, helloTask, size_t(42));
      
  rt::executeOnAll(
      [](const size_t &v) {
        std::stringstream SS;
        SS << rt::thisLocality() << "-> Hello, SHAD : " << v << std::endl;
        std::cout << SS.str();
      }, size_t(123));

  rt::forEachOnAll(
      [](const size_t &v, size_t i) {
        std::stringstream SS;
        SS << rt::thisLocality() << "-> Hello, SHAD : at " << i << "/"  << v << std::endl;
        std::cout << SS.str();
      }, size_t(100), 100);
  return 0;
}

}
