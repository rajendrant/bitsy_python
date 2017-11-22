#ifndef EXECSTACK_H_
#define EXECSTACK_H_

#include "bitsylimit.h"
#include "variable.h"
#include "ByteStack.h"
#include "BitStack.h"

#include <stdint.h>

namespace bitsy_python {

class ExecStack {
public:
  void push(const Variable &v);
  Variable pop();
private:
  BitStack hdr;
  ByteStack data;
};

}

#endif /* EXECSTACK_H_ */
