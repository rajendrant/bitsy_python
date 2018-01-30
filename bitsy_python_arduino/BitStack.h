#ifndef BITSTACK_H_
#define BITSTACK_H_

#include <stdint.h>

namespace bitsy_python {

class BitStack {
 public:
  void init();
  void pushTwoBits(uint8_t bits);
  uint8_t popTwoBits();
  bool getNextTwoBits(uint8_t *ret, uint32_t *p) const;

 private:
  uint8_t *bytes = 0;
  uint16_t pos;
  uint8_t len;
};
}
#endif /* BITSTACK_H_ */
