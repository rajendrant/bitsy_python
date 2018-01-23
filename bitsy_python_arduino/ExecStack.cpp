#include "ExecStack.h"

namespace bitsy_python {

void ExecStack::push(const Variable &v) {
  hdr.pushThreeBits(v.type);
  data.pushBytes((uint8_t *)&v.val, v.size());
}

Variable ExecStack::pop() {
  Variable v;
  v.type = hdr.popThreeBits();
  data.popBytes((uint8_t *)&v.val, v.size());
  return v;
}

uint32_t ExecStack::getCustomHeapVariableMap(uint8_t start_id) const {
  uint32_t map = 0;
  uint8_t bits, *bytes, bytelen=0, byteind=0;
  uint32_t p1=INVALID_ITERATOR, p2=INVALID_ITERATOR;
  while(hdr.getNextThreeBits(&bits, &p1)) {
    Variable var;
    var.type = bits;
    for(int i=0; i<var.size(); i++) {
      if (bytelen==byteind) {
        data.getNextByte(&p2, &bytes, &bytelen);
        byteind = 0;
      }
      ((uint8_t *)&var.val)[i] = bytes[byteind];
      byteind++;
    }
    if (bits == Variable::CUSTOM && var.is_custom_heap_type() &&
        var.val.custom_type.val>=start_id &&
        var.val.custom_type.val<start_id+32) {
      map |= 0x1<<(var.val.custom_type.val-start_id);
    }
  }
  return map;
}

}
