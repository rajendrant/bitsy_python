#include "bitsy_alloc.h"

#include <assert.h>

Block blocks[TOTAL_BLOCKS];

uint8_t freelist = INVALID_BLOCK;

uint8_t sequential_stack_next = 0;

void freelist_sanity_check() {
#ifdef DESKTOP
    uint8_t prev=INVALID_BLOCK;
    for(uint8_t i=freelist; i!=INVALID_BLOCK; i=blocks[i].next) {
        assert(prev==blocks[i].prev);
        assert(prev>i);
        prev=i;
    }
#endif
}

void init() {
	if (freelist != INVALID_BLOCK)
		return;
	for(uint8_t i=TOTAL_BLOCKS-1; i>0; i--) {
		blocks[i].next = i-1;
		blocks[i].prev = i+1;
	}
	blocks[0].prev = 1;
	blocks[0].next = INVALID_BLOCK;
	blocks[TOTAL_BLOCKS-1].prev = INVALID_BLOCK;
	freelist = TOTAL_BLOCKS-1;
	freelist_sanity_check();
}

uint8_t alloc_block() {
	uint8_t next = freelist;
	freelist = blocks[next].next;
	blocks[next].next = INVALID_BLOCK;
	assert(blocks[next].prev == INVALID_BLOCK);
	blocks[freelist].prev = INVALID_BLOCK;
	freelist_sanity_check();
	return next;
}

void free_block(uint8_t tofree) {
    // Free the block and keep the freelist sorted in descending order.
    uint8_t prev=INVALID_BLOCK, next=freelist;
    while(next!=INVALID_BLOCK && next>tofree) {
        prev = next;
        next=blocks[next].next;
    }
	blocks[tofree].prev = prev;
	blocks[tofree].next = next;
    if (next!=INVALID_BLOCK)
    	blocks[next].prev = tofree;
    if (prev!=INVALID_BLOCK)
    	blocks[prev].next = tofree;
	if (prev==INVALID_BLOCK)
	    freelist = tofree;
	freelist_sanity_check();
}

/**
 * TODO(rajendrant): The scheme of allocating sequential memory for stack may
 * break when memory allocation in heap feature is implemented. The next block
 * needed for sequential stack may have been allocated for heap objects and may
 * not be freed by gc. The assert below would fail in that case.
 */
void sequential_stack_alloc() {
    uint8_t prev=INVALID_BLOCK;
    for(uint8_t i=freelist; i!=INVALID_BLOCK; i=blocks[i].next) {
        prev=i;
    }
    assert(prev==sequential_stack_next);
    blocks[blocks[sequential_stack_next].prev].next = INVALID_BLOCK;
    sequential_stack_next++;
	freelist_sanity_check();
}

void sequential_stack_free() {
    sequential_stack_next--;
    free_block(sequential_stack_next);
	freelist_sanity_check();
}

uint16_t sequential_stack_size() {
    return sequential_stack_next*sizeof(Block);
}
