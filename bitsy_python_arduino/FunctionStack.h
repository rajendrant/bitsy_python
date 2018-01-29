#ifndef FunctionStack_H_
#define FunctionStack_H_

#include <stdint.h>
#include "bitsylimit.h"
#include "variable.h"

namespace bitsy_python {

namespace FunctionStack {

bool is_empty();
void setup_function_call(uint8_t args, uint8_t vars, uint16_t old_ins_ptr);
bool return_function(uint16_t *old_ins_ptr);

Variable getNthVariable(uint8_t n);
void setNthVariable(uint8_t n, const Variable& v);

uint32_t getCustomHeapVariableMap(uint8_t start_id);

}
}

#endif /* FunctionStack_H_ */
