#ifndef BITSY_PYTHON_GC_H_
#define BITSY_PYTHON_GC_H_

#include <stdint.h>

#include "variable.h"

namespace bitsy_python {
void gc();
void updateCustomHeapVariableMap(uint8_t start_id, const Variable &v, uint32_t *map);
}

#endif /* BITSY_PYTHON_GC_H_ */
