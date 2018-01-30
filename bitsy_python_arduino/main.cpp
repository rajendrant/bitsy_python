#include "bitsylimit.h"

#ifdef DESKTOP
#include "bitsy_python_vm.h"

#define ENABLE_BITSY_USERLIB_TESTUSERLIB
#include "bitsy_python_userlibs.h"

int main(int argc, char **argv) {
  if (argc != 2) return -1;

  bitsy_python::BitsyPythonVM::init(argv[1]);
  bitsy_python::BitsyPythonVM::execute();
  return 0;
}

#endif
