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
  // TODO(rajendrant): Implement this
  return map;
}

}
