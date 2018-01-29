#ifndef FUNCTIONSTACK_H_
#define FUNCTIONSTACK_H_

#include <stdint.h>
#include "bitsylimit.h"
#include "variable.h"

namespace bitsy_python {

bool FunctionStack_is_empty();
void FunctionStack_setup_function_call(uint8_t args, uint8_t vars, uint16_t old_ins_ptr);
bool FunctionStack_return_function(uint16_t *old_ins_ptr);

Variable FunctionStack_getNthVariable(uint8_t n);
void FunctionStack_setNthVariable(uint8_t n, const Variable& v);

uint32_t FunctionStack_getCustomHeapVariableMap(uint8_t start_id);

}

#endif /* FUNCTIONSTACK_H_ */
