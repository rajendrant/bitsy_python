#ifndef BITSTRING_H_
#define BITSTRING_H_

#include "bitsylimit.h"

#include <stdint.h>

namespace bitsy_python {

class BitString {
 public:
#ifdef DESKTOP
  void init(uint8_t *buf, const uint16_t sz) {_buf=buf; size=sz;}
#endif

  uint8_t get_bit8(uint16_t pos, uint8_t len) const;
  uint16_t get_bit16(uint16_t pos, uint8_t len) const;
  uint32_t get_bit32(uint16_t pos, uint8_t len) const;

 private:
#ifdef DESKTOP
  uint8_t *_buf;
  uint16_t size;
#endif
};
}
#endif /* BITSTRING_H_ */
