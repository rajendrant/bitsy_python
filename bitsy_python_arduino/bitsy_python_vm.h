#ifndef BITSY_PYTHON_VM_H_
#define BITSY_PYTHON_VM_H_

#include "bitsylimit.h"
#include "BitsyHeap.h"
#include "Program.h"
#include "ExecStack.h"
#include "FunctionStack.h"

namespace bitsy_python {

extern BitsyHeap bitsy_heap;

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

    ExecStack exec_stack;
    FunctionStack function_stack;
    Program prog;
};
}

#endif /* BITSY_PYTHON_VM_H_ */
