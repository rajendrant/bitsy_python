#include "gc.h"

#include "bitsy_python_vm.h"

namespace bitsy_python {
void gc() {
  for(uint8_t start_id=0; start_id < bitsy_heap.max(); start_id+=32) {
    uint32_t heap_free = bitsy_heap.getFreeIDMap(start_id);
    if (heap_free==0xFFFFFFFF) continue;
    heap_free |= exec_stack.getCustomHeapVariableMap(start_id) |
                 function_stack.getCustomHeapVariableMap(start_id);
    for(uint8_t i=0; i<32; i++) {
      if (!(heap_free & 0x1<<i))
        bitsy_heap.FreeVar(start_id+i);
    }
  }
}
}
