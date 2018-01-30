#include "BitString.h"

#include <assert.h>
#include <stdio.h>

#include "test_common.h"

int main() {
  uint8_t BUF[10];
  bitsy_python::BitString bits;
  bits.init(BUF, sizeof(BUF));
  BUF[0] = 0xF1;
  BUF[1] = 0b10100001;

  assert(bits.get_bit8(0, 1) == 1);
  assert(bits.get_bit8(0, 2) == 3);
  assert(bits.get_bit8(0, 3) == 7);
  assert(bits.get_bit8(0, 4) == 0xF);
  assert(bits.get_bit8(0, 5) == 0b11110);
  assert(bits.get_bit8(0, 6) == 0b111100);
  assert(bits.get_bit8(0, 7) == 0b1111000);
  assert(bits.get_bit8(0, 8) == 0xF1);

  assert(bits.get_bit8(1, 1) == 1);
  assert(bits.get_bit8(3, 1) == 1);
  assert(bits.get_bit8(4, 1) == 0);
  assert(bits.get_bit8(3, 2) == 0b10);

  assert(bits.get_bit8(1, 8) == 0b11100011);
  assert(bits.get_bit8(2, 8) == 0b11000110);
  assert(bits.get_bit8(4, 8) == 0b00011010);
  assert(bits.get_bit8(4, 7) == 0b0001101);
  assert(bits.get_bit8(4, 6) == 0b000110);
  assert(bits.get_bit16(0, 9) == 0b111100011);
  assert(bits.get_bit16(0, 10) == 0b1111000110);
  assert(bits.get_bit16(0, 11) == 0b11110001101);
  assert(bits.get_bit16(0, 12) == 0b111100011010);
  assert(bits.get_bit16(2, 10) == 0b1100011010);
  assert(bits.get_bit16(2, 12) == 0b110001101000);
  assert(bits.get_bit16(2, 14) == 0b11000110100001);

  printf(__FILE__ " passed\n");
}
