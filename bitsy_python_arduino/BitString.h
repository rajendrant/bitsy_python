#ifndef BITSTRING_H_
#define BITSTRING_H_

#include "bitsylimit.h"

#include <stdint.h>

namespace bitsy_python {

namespace BitString {

#define PROG_BYTE_SIZE 1024
#define INVALID_CURR_POS 0xFF00

extern uint16_t curr_pos;

#ifdef DESKTOP
void init(uint8_t *buf, uint16_t sz);
#else
void init();
#endif

bool is_sane();
void mark_insane();

uint8_t get_bit8(uint16_t pos, uint8_t len);
uint16_t get_bit16(uint16_t pos, uint8_t len);

uint8_t get_next_bit8(uint8_t len);
uint32_t get_next_bit32();

}
}
#endif /* BITSTRING_H_ */
