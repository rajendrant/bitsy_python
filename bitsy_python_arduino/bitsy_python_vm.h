#ifndef BITSY_PYTHON_VM_H_
#define BITSY_PYTHON_VM_H_

#include "bitsylimit.h"
#include "variable.h"

namespace bitsy_python {

namespace BitsyPythonVM {

#ifdef DESKTOP
void init(const char *fname);
#elif defined(ARDUINO)
void init();
#endif

void initExecution();
void execute();
bool executeOneStep();
void callUserFunction(uint16_t f, Variable arg);

}
}

#endif /* BITSY_PYTHON_VM_H_ */
