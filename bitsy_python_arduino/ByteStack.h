#ifndef BYTESTACK_H_
#define BYTESTACK_H_

#include "BlockStack.h"

namespace bitsy_python {

class ByteStack : BlockStack {
 public:
  ByteStack();
  void pushBytes(const uint8_t *src, uint8_t len);
  void popBytes(uint8_t *dst, uint8_t len);
  void pushByte(uint8_t src);
  uint8_t popByte();

 private:
  uint8_t _top_byte = 0;
};
}
#endif /* BYTESTACK_H_ */
