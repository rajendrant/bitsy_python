#ifndef EXECSTACK_H_
#define EXECSTACK_H_

#include "BitStack.h"
#include "ByteStack.h"
#include "bitsylimit.h"
#include "variable.h"

#include <stdint.h>

namespace bitsy_python {

class ExecStack {
 public:
  void push(const Variable &v);
  Variable pop();
  uint32_t getCustomHeapVariableMap(uint8_t start_id) const;

 private:
  BitStack hdr;
  ByteStack data;
};
}

#endif /* EXECSTACK_H_ */
