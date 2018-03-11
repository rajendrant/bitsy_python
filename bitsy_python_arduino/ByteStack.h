#ifndef BYTESTACK_H_
#define BYTESTACK_H_

#include "BlockStack.h"

namespace bitsy_python {
namespace ByteStack {

void init();
void pushBytes(const uint8_t *src, uint8_t len);
void popBytes(uint8_t *dst, uint8_t len);
void pushByte(uint8_t src);
uint8_t popByte();
bool getPrevByte(uint8_t *p, uint8_t **bytes, uint8_t *len);

}
}
#endif /* BYTESTACK_H_ */
