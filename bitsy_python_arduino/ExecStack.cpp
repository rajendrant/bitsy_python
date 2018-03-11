#include "ExecStack.h"

#include "ByteStack.h"
#include "gc.h"

#include <stdio.h>

namespace bitsy_python {
namespace ExecStack {

uint8_t pos;
uint8_t top;

void init() {
  ByteStack::init();
}

void push(const Variable &v) {
  if (pos==8) {
    ByteStack::pushByte(top);
    pos = 0;
  }
  top = (top & ~(0x3<<pos)) | (v.type<<pos);
  ByteStack::pushBytes((uint8_t *)&v.val, v.size());
  pos+=2;
}

Variable pop() {
  if (pos==0) {
    top = ByteStack::popByte();
    pos = 8;
  }
  pos-=2;
  Variable v;
  v.type = (top>>pos)&0x3;
  ByteStack::popBytes((uint8_t *)&v.val, v.size());
  return v;
}

uint32_t getCustomHeapVariableMap(uint8_t start_id) {
  uint32_t map = 0;
  uint8_t *bytes, bytelen=0;
  uint8_t p1=0xFF;
  uint8_t pos2=pos;
  uint8_t top2=top;
  ByteStack::getPrevByte(&p1, &bytes, &bytelen);
  while(1) {
    if (pos2==0) {
      if (bytelen==0 && !ByteStack::getPrevByte(&p1, &bytes, &bytelen)) break;
      top2 = bytes[--bytelen];
      pos2 = 8;
    }
    pos2-=2;
    Variable v;
    v.type = (top2>>pos2)&0x3;
    for(uint8_t i=v.size(); i!=0; i--) {
      if (bytelen==0 && !ByteStack::getPrevByte(&p1, &bytes, &bytelen)) break;
      ((uint8_t *)&v.val)[i-1] = bytes[--bytelen];
    }
    if (v.type == Variable::CUSTOM)
      updateCustomHeapVariableMap(start_id, v.val.custom_type, &map);
  }
  return map;
}

}
}
