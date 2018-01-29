#include "bitsylimit.h"

#ifdef DESKTOP
#include "bitsy_python_vm.h"

#define ENABLE_BITSY_USERLIB_TESTUSERLIB
#include "bitsy_python_userlibs.h"

bitsy_python::BitsyPythonVM *vm_global;

int main(int argc, char **argv) {
  if (argc != 2) return -1;

  bitsy_python::BitsyPythonVM vm(argv[1]);
  vm_global = &vm;
  vm.initExecution();
  vm.execute();
  return 0;
}

#endif
