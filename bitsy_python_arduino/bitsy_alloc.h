#ifndef BITSY_ALLOC_H_
#define BITSY_ALLOC_H_

#include <stdint.h>

#include "bitsylimit.h"

#define INVALID_BLOCK 0xF

typedef struct {
	uint8_t bytes[BYTES_PER_BLOCK];
	uint8_t next : BITS_PER_BLOCK_ID;
	uint8_t prev : BITS_PER_BLOCK_ID;
} Block;

extern Block blocks[];

extern void init();

extern uint8_t alloc_block();

extern void free_block(uint8_t tofree);

extern void sequential_stack_alloc();

extern void sequential_stack_free();

extern uint16_t sequential_stack_size();

#endif /* BITSY_ALLOC_H_ */
