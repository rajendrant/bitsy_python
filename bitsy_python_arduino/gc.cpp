#include "gc.h"

#include "bitsy_python_vm.h"
#include "datatypes/datatype.h"
#include "ExecStack.h"
#include "FunctionStack.h"

namespace bitsy_python {
void gc() {
  for(uint8_t start_id=0; start_id < BitsyHeap::lastID(); start_id+=32) {
    uint32_t heap_free = BitsyHeap::getFreeIDMap(start_id);
    if (heap_free==0xFFFFFFFF) continue;
    heap_free |= ExecStack::getCustomHeapVariableMap(start_id) |
                 FunctionStack::getCustomHeapVariableMap(start_id);
    for(uint8_t i=0; i<32; i++) {
      if (!(heap_free & 0x1L<<i))
        BitsyHeap::FreeVar(start_id+i);
    }
  }
}

void updateCustomHeapVariableMap(uint8_t start_id, Variable::CustomType v, uint32_t *map) {
  if (Variable::is_custom_heap_type(v) && v.val>=start_id && v.val<start_id+32) {
    *map |= 0x1L<<(v.val-start_id);
    DataType::updateUsedContainers(start_id, v, map);
  }
}
}
