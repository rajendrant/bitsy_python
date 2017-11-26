#include "bitsy_alloc.h"

#include <assert.h>

Block blocks[TOTAL_BLOCKS];

// Freelist is a circular singly linked list.
uint8_t freelist = INVALID_BLOCK;

// The next block to allocate for the top and bottom blocks.
struct {
  uint8_t top : 4;
  uint8_t bottom : 4;
} __attribute__((packed)) block_next;

void freelist_sanity_check() {
#ifdef DESKTOP
  uint8_t prev = blocks[freelist].prev;
  assert(blocks[prev].next == freelist);
  for (uint8_t i = freelist; blocks[i].next != freelist; i = blocks[i].next) {
    assert(i < TOTAL_BLOCKS);
    assert(prev == blocks[i].prev);
    prev = i;
  }
#endif
}

void init() {
  if (freelist != INVALID_BLOCK) return;
  uint8_t prev = freelist = TOTAL_BLOCKS/2;
  for (uint8_t i = 1; i <= TOTAL_BLOCKS/2; i++) {
    blocks[prev].next = TOTAL_BLOCKS/2+i;
    blocks[TOTAL_BLOCKS/2+i].prev = prev;
    blocks[TOTAL_BLOCKS/2+i].next = TOTAL_BLOCKS/2-i;
    blocks[TOTAL_BLOCKS/2-i].prev = TOTAL_BLOCKS/2+i;
    prev = TOTAL_BLOCKS/2-i;
  }
  // Make it circular.
  blocks[0].next = TOTAL_BLOCKS/2;
  blocks[TOTAL_BLOCKS/2].prev = 0;
  freelist_sanity_check();
  block_next.top = 0;
  block_next.bottom = TOTAL_BLOCKS - 1;
}

uint8_t alloc_block() {
  uint8_t next = freelist;
  freelist = blocks[next].next;
  assert(blocks[freelist].prev == next);
  blocks[freelist].prev = blocks[next].prev;
  blocks[blocks[freelist].prev].next = freelist;
  blocks[next].prev = INVALID_BLOCK;
  blocks[next].next = INVALID_BLOCK;
  freelist_sanity_check();
  return next;
}

void free_block(uint8_t tofree) {
  if (freelist==INVALID_BLOCK) {
    freelist = tofree;
    blocks[tofree].next = blocks[tofree].prev = tofree;
    return;
  }
  // Free the block and add it to the end of the freelist.
  blocks[tofree].prev = blocks[freelist].prev;
  blocks[tofree].next = freelist;
  blocks[freelist].prev = tofree;
  blocks[blocks[tofree].prev].next = tofree;
  freelist_sanity_check();
}

/**
 * TODO(rajendrant): The scheme of allocating sequential memory for stack may
 * break when memory allocation in heap feature is implemented. The next block
 * needed for sequential stack may have been allocated for heap objects and may
 * not be freed by gc. The assert below would fail in that case.
 */
void top_block_alloc() {
  blocks[blocks[block_next.top].prev].next = blocks[block_next.top].next;
  blocks[blocks[block_next.top].next].prev = blocks[block_next.top].prev;
  block_next.top++;
  freelist_sanity_check();
}

void top_block_free() {
  block_next.top--;
  free_block(block_next.top);
  freelist_sanity_check();
}

uint16_t top_block_size() {
  return block_next.top * sizeof(Block);
}

void bottom_block_alloc() {
  if(freelist==block_next.bottom)
    freelist = blocks[freelist].next;
  blocks[blocks[block_next.bottom].prev].next = blocks[block_next.bottom].next;
  blocks[blocks[block_next.bottom].next].prev = blocks[block_next.bottom].prev;
  block_next.bottom--;
  freelist_sanity_check();
}

void bottom_block_free() {
  block_next.bottom++;
  free_block(block_next.bottom);
  freelist_sanity_check();
}

uint16_t bottom_block_size() {
  return (TOTAL_BLOCKS-1-block_next.bottom) * sizeof(Block);
}
