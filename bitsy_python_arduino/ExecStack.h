#ifndef EXECSTACK_H_
#define EXECSTACK_H_

#include "bitsylimit.h"
#include "variable.h"

#include <stdint.h>

namespace bitsy_python {

void ExecStack_push(const Variable &v);
Variable ExecStack_pop();
uint32_t ExecStack_getCustomHeapVariableMap(uint8_t start_id);

}

#endif /* EXECSTACK_H_ */
