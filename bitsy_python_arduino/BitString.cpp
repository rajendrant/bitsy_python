#include "BitString.h"

#include <assert.h>

#define BITMASK8(nbits) (uint8_t(~0) >> (8 - (nbits)))

#define FIRSTN(p, n, T) (((T)(p) >> (8 - (n)-1)))
#define LASTN(p, n, T) (((T)(p)&BITMASK8(n)))

#define INDEX(p) ((p) >> 3)

#ifdef DESKTOP
#define BUF(n) _buf[(n)]
#elif defined(ARDUINO)
#include <EEPROM.h>
#define BUF(n) EEPROM.read(n)
#endif

namespace bitsy_python {

uint8_t BitString::get_bit8(uint16_t pos, uint8_t len) const {
  assert(len > 0 && len <= 8);
  if (INDEX(pos) == INDEX(pos + len - 1)) {
    return FIRSTN(BUF(INDEX(pos)), ((pos + len - 1) % 8), uint8_t) &
           BITMASK8(len);
  }
  return LASTN(BUF(INDEX(pos)), 8 - pos % 8, uint8_t) << (len - (8 - pos % 8)) |
         FIRSTN(BUF(INDEX(pos) + 1), ((pos + len - 1) % 8), uint8_t);
}

uint16_t BitString::get_bit16(uint16_t pos, uint8_t len) const {
  assert(len > 0 && len <= 16);
  uint8_t diff = INDEX(pos + len - 1) - INDEX(pos);
  if (diff == 0) {
    return get_bit8(pos, len);
  } else if (diff == 1) {
    return LASTN(BUF(INDEX(pos)), 8 - pos % 8, uint16_t)
               << (len - (8 - pos % 8)) |
           FIRSTN(BUF(INDEX(pos) + 1), (pos + len - 1) % 8, uint16_t);
  }
  return LASTN(BUF(INDEX(pos)), 8 - pos % 8, uint16_t)
             << (len - (8 - pos % 8)) |
         (uint16_t(BUF(INDEX(pos) + 1))) << ((pos + len) % 8) |
         FIRSTN(BUF(INDEX(pos) + 2), (pos + len - 1) % 8, uint16_t);
}

uint32_t BitString::get_bit32(uint16_t pos, uint8_t len) const {
  assert(len > 0 && len <= 32);
  uint8_t diff = INDEX(pos + len - 1) - INDEX(pos);
  if (diff <= 1) {
    return get_bit16(pos, len);
  } else if (diff == 2) {
    return LASTN(BUF(INDEX(pos)), 8 - pos % 8, uint32_t)
               << (len - (8 - pos % 8)) |
           (uint32_t(BUF(INDEX(pos) + 1))) << ((pos + len) % 8) |
           FIRSTN(BUF(INDEX(pos) + 2), (pos + len - 1) % 8, uint32_t);
  } else if (diff == 3) {
    return LASTN(BUF(INDEX(pos)), 8 - pos % 8, uint32_t)
               << (len - (8 - pos % 8)) |
           (uint32_t(BUF(INDEX(pos) + 1))) << (16 + (pos + len) % 8) |
           (uint32_t(BUF(INDEX(pos) + 2))) << (8 + (pos + len) % 8) |
           FIRSTN(BUF(INDEX(pos) + 3), (pos + len - 1) % 8, uint32_t);
  }
  return LASTN(BUF(INDEX(pos)), 8 - pos % 8, uint32_t)
             << (len - (8 - pos % 8)) |
         (uint32_t(BUF(INDEX(pos) + 1))) << (16 + (pos + len) % 8) |
         (uint32_t(BUF(INDEX(pos) + 2))) << (8 + (pos + len) % 8) |
         (uint32_t(BUF(INDEX(pos) + 3))) << ((pos + len) % 8) |
         FIRSTN(BUF(INDEX(pos) + 4), (pos + len - 1) % 8, uint32_t);
}
}
