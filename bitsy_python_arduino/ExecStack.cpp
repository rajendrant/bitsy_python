#include "ExecStack.h"

#include "BitStack.h"
#include "ByteStack.h"
#include "gc.h"

#include <stdio.h>

namespace bitsy_python {

BitStack hdr;
ByteStack data;

void ExecStack::init() {
  hdr.init();
  data.init();
}

void ExecStack::push(const Variable &v) {
  hdr.pushTwoBits(v.type);
  data.pushBytes((uint8_t *)&v.val, v.size());
}

Variable ExecStack::pop() {
  Variable v;
  v.type = hdr.popTwoBits();
  data.popBytes((uint8_t *)&v.val, v.size());
  return v;
}

uint32_t ExecStack::getCustomHeapVariableMap(uint8_t start_id) {
  uint32_t map = 0;
  uint8_t bits, *bytes, bytelen=0, byteind=0;
  uint32_t p1=INVALID_ITERATOR, p2=INVALID_ITERATOR;
  while(hdr.getNextTwoBits(&bits, &p1)) {
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
    if (bits == Variable::CUSTOM)
      updateCustomHeapVariableMap(start_id, var.val.custom_type, &map);
  }
  return map;
}

}
