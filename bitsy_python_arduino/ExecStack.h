#ifndef ExecStack_H_
#define ExecStack_H_

#include "bitsylimit.h"
#include "variable.h"

#include <stdint.h>

namespace bitsy_python {

namespace ExecStack {

void push(const Variable &v);
Variable pop();
uint32_t getCustomHeapVariableMap(uint8_t start_id);

}
}

#endif /* ExecStack_H_ */
