#ifndef BITSY_PYTHON_VM_H_
#define BITSY_PYTHON_VM_H_

#include "BitsyHeap.h"
#include "ExecStack.h"
#include "FunctionStack.h"
#include "Program.h"
#include "bitsylimit.h"

namespace bitsy_python {

extern BitsyHeap bitsy_heap;
extern ExecStack exec_stack;
extern FunctionStack function_stack;

class BitsyPythonVM {
 public:
#ifdef DESKTOP
  BitsyPythonVM(const char *fname);
#elif defined(ARDUINO)
  BitsyPythonVM();
#endif

  void initExecution();
  void execute();
  bool executeOneStep();

 private:
  void binary_arithmetic(uint8_t ins);

  Program prog;
};
}

#endif /* BITSY_PYTHON_VM_H_ */
