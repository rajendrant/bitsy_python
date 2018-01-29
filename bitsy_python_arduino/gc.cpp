#include "gc.h"

#include "bitsy_python_vm.h"
#include "datatypes/datatype.h"

namespace bitsy_python {
void gc() {
  for(uint8_t start_id=0; start_id < bitsy_heap.lastID(); start_id+=32) {
    uint32_t heap_free = bitsy_heap.getFreeIDMap(start_id);
    if (heap_free==0xFFFFFFFF) continue;
    heap_free |= ExecStack_getCustomHeapVariableMap(start_id) |
                 FunctionStack_getCustomHeapVariableMap(start_id);
    for(uint8_t i=0; i<32; i++) {
      if (!(heap_free & 0x1<<i))
        bitsy_heap.FreeVar(start_id+i);
    }
  }
}

void updateCustomHeapVariableMap(uint8_t start_id, const Variable &v, uint32_t *map) {
  if (v.is_custom_heap_type() &&
        v.val.custom_type.val>=start_id &&
        v.val.custom_type.val<start_id+32) {
      *map |= 0x1<<(v.val.custom_type.val-start_id);
  DataType::updateUsedContainers(start_id, v, map);
}
}
}
