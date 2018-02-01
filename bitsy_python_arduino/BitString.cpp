#include "BitString.h"

#define BITMASK8(nbits) (uint8_t(~0) >> (8 - (nbits)))

#define FIRSTN(p, n, T) (((T)(p) >> (8 - (n)-1)))
#define LASTN(p, n, T) (((T)(p)&BITMASK8(n)))

#define INDEX(p) ((p) >> 3)

#ifdef DESKTOP
#define BUF(n) _buf[(n)]
#elif defined(ARDUINO) && defined(AVR)
#include <EEPROM.h>
#define BUF(n) EEPROM.read(n)
#elif defined(ARDUINO) && defined(ESP8266)
#include <EEPROM.h>
#define PROG_BYTE_SIZE 1024
uint8_t prog_bytes[PROG_BYTE_SIZE];
#define BUF(n) prog_bytes[n]
#endif

namespace bitsy_python {

namespace BitString {

uint16_t curr_pos = 0;

#ifdef DESKTOP
uint8_t *_buf;
uint16_t size;

void init(uint8_t *buf, uint16_t sz) {_buf=buf; size=sz;}
#else
void init() {
#ifdef ESP8266
  EEPROM.begin(PROG_BYTE_SIZE);
  for(uint16_t i=0; i<PROG_BYTE_SIZE; i++)
    prog_bytes[i] = EEPROM.read(i);
  EEPROM.end();
#endif
}
#endif

uint8_t get_bit8(uint16_t pos, uint8_t len) {
  BITSY_ASSERT(len > 0 && len <= 8);
  if (INDEX(pos) == INDEX(pos + len - 1)) {
    return FIRSTN(BUF(INDEX(pos)), ((pos + len - 1) % 8), uint8_t) &
           BITMASK8(len);
  }
  return LASTN(BUF(INDEX(pos)), 8 - pos % 8, uint8_t) << (len - (8 - pos % 8)) |
         FIRSTN(BUF(INDEX(pos) + 1), ((pos + len - 1) % 8), uint8_t);
}

uint16_t get_bit16(uint16_t pos, uint8_t len) {
  BITSY_ASSERT(len > 0 && len <= 16);
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

uint8_t get_next_bit8(uint8_t len) {
  auto v = get_bit8(curr_pos, len);
  curr_pos += len;
  return v;
}

uint32_t get_next_bit32() {
  uint32_t v = ((uint32_t)get_bit16(curr_pos, 16))<<16;
  v |= get_bit16(curr_pos+16, 16);
  curr_pos += 32;
  return v;
}

}
}
