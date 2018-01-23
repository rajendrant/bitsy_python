#include "BitStack.h"

#include <assert.h>
#include <stdlib.h>

#define GET_2BIT_AT_POS(buf, n) (((buf) >> (n)) & 0x3)
#define SET_2BIT_AT_POS(buf, n, val) \
  ((buf) = ((buf) & ~(0x3 << (n))) | (((val)&0x3) << (n)))

#define GET_3BIT_AT_POS(buf, n) (((buf) >> (n)) & 0x7)
#define SET_3BIT_AT_POS(buf, n, val) \
  ((buf) = ((buf) & ~(0x7 << (n))) | (((val)&0x7) << (n)))

namespace bitsy_python {

BitStack::BitStack() {
  bytes = NULL;
  pos = 0;
  len = 0;
}

void BitStack::pushThreeBits(uint8_t bits) {
  if(pos/5==len) {
    len += 16;
    bytes = (uint16_t*) realloc(bytes, len*sizeof(uint16_t));
    assert(bytes);
  }
  SET_3BIT_AT_POS(bytes[pos/5], ((pos%5)*3), bits);
  pos += 3;
}

uint8_t BitStack::popThreeBits() {
  pos -= 3;
  return GET_3BIT_AT_POS(bytes[pos/5], ((pos%5)*3));
}

bool BitStack::getNextThreeBits(uint8_t *ret, uint32_t *p) const {
  if (*p==INVALID_ITERATOR)
    *p=0;
  if (*p >= pos)
    return false;
  *ret = GET_3BIT_AT_POS(bytes[*p/5], ((*p%5)*3));
  *p += 3;
  return true;
}

}
