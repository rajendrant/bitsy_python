#include "BitStack.h"

#include <stdlib.h>

#define GET_2BIT_AT_POS(buf, n) (((buf) >> (n)) & 0x3)
#define SET_2BIT_AT_POS(buf, n, val) \
  ((buf) = ((buf) & ~(0x3 << (n))) | (((val)&0x3) << (n)))

namespace bitsy_python {

BitStack::BitStack() {
  bytes = NULL;
  pos = 0;
  len = 0;
}

void BitStack::pushTwoBits(uint8_t bits) {
  if(pos/4==len) {
    len += 16;
    bytes = (uint8_t*) realloc(bytes, len);
    BITSY_ASSERT(bytes);
  }
  SET_2BIT_AT_POS(bytes[pos/4], pos%4, bits);
  pos += 2;
}

uint8_t BitStack::popTwoBits() {
  pos -= 2;
  return GET_2BIT_AT_POS(bytes[pos/4], pos%4);
}

bool BitStack::getNextTwoBits(uint8_t *ret, uint32_t *p) const {
  if (*p==INVALID_ITERATOR)
    *p=0;
  if (*p >= pos)
    return false;
  *ret = GET_2BIT_AT_POS(bytes[*p/4], *p%4);
  *p += 2;
  return true;
}

}
