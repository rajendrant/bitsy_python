#include "Program.h"

#include <stdio.h>

#include "test_common.h"

class ProgramTest {
public:
  void test1() {
    //bitsy_python::Program p = bitsy_python::Program::FromFile("/usr/local/google/home/rajendrant/IdeaProjects/bitsypython/test.bitsy");
    //p.dump();
  }
};

int main() {
  ProgramTest t;
  t.test1();
  printf(__FILE__ " passed\n");
  return 0;
}
