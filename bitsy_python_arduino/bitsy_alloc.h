#ifndef BITSY_ALLOC_H_
#define BITSY_ALLOC_H_

#include <stdint.h>

#include "bitsylimit.h"

#define BLOCKS_END (((uint8_t*)blocks) + sizeof(blocks))

#define INVALID_BLOCK 0xF

#define INVALID_ITERATOR 0xFFFFFFFF

typedef struct {
  uint8_t bytes[BYTES_PER_BLOCK];
  uint8_t next : BITS_PER_BLOCK_ID;
  uint8_t prev : BITS_PER_BLOCK_ID;
} Block;

extern Block blocks[TOTAL_BLOCKS];

extern void bitsy_alloc_init();

extern uint8_t alloc_block();

extern void free_block(uint8_t tofree);

extern void top_block_alloc();

extern void top_block_free();

extern uint16_t top_block_size();

extern void bottom_block_alloc();

extern void bottom_block_free();

extern uint16_t bottom_block_size();

#endif /* BITSY_ALLOC_H_ */
