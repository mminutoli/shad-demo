#include <iostream>
#include <sstream>

#include "shad/shad.h"
#include "shad/runtime/runtime.h"


void helloTask(shad::rt::Handle &h, const size_t &v) {
  std::stringstream SS;
  SS << shad::rt::thisLocality() << "-> Hello, SHAD : " << v << std::endl;
  std::cout << SS.str();
}


namespace shad {

int main(int argc, char *argv[])
{
  rt::Handle h;

  for (auto l : rt::allLocalities())
    rt::asyncExecuteAt(h, l, helloTask, size_t(42));
      
  rt::asyncExecuteOnAll(h,
      [](rt::Handle &h, const size_t&v) {
        std::stringstream SS;
        SS << rt::thisLocality() << "-> Hello, SHAD : " << v << std::endl;
        std::cout << SS.str();
      }, size_t(123));

  rt::asyncForEachOnAll(h,
      [](rt::Handle &h, const size_t&v, size_t i) {
        std::stringstream SS;
        SS << rt::thisLocality() << "-> Hello, SHAD : at " << i << "/"  << v << std::endl;
        std::cout << SS.str();
      }, size_t(100), 100);

  rt::waitForCompletion(h);
  return 0;
}

}
