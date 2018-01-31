#ifndef FunctionStack_H_
#define FunctionStack_H_

#include <stdint.h>
#include "bitsylimit.h"
#include "Program.h"
#include "variable.h"

namespace bitsy_python {

namespace FunctionStack {

void init();

bool is_empty();
void setup_function_call(const Program::FunctionParams& p);
Program::FunctionParams return_function();

Variable getNthVariable(uint8_t n);
void setNthVariable(uint8_t n, const Variable& v);

uint32_t getCustomHeapVariableMap(uint8_t start_id);

}
}

#endif /* FunctionStack_H_ */
