#include "BitStack.h"

#define GET_2BIT_AT_POS(buf, n) (((buf) >> (n)) & 0x3)
#define SET_2BIT_AT_POS(buf, n, val) \
  ((buf) = ((buf) & ~(0x3 << (n))) | (((val)&0x3) << (n)))

#define GET_3BIT_AT_POS(buf, n) (((buf) >> (n)) & 0x7)
#define SET_3BIT_AT_POS(buf, n, val) \
  ((buf) = ((buf) & ~(0x7 << (n))) | (((val)&0x7) << (n)))

namespace bitsy_python {

BitStack::BitStack() {}

void BitStack::pushThreeBits(uint8_t bits) {
  SET_3BIT_AT_POS(byte, pos, bits);
  if (pos == 12) {
    pos = 0;
    pushBytes((uint8_t*)&byte, 2);
  } else {
    pos += 3;
  }
}

uint8_t BitStack::popThreeBits() {
  if (pos == 0) {
    pos = 12;
    popBytes((uint8_t*)&byte, 2);
  } else {
    pos -= 3;
  }
  return GET_3BIT_AT_POS(byte, pos);
}
}
