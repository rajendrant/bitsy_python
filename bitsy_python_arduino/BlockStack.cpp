#include "BlockStack.h"

#include <assert.h>
#include <stdio.h>

#include "bitsy_alloc.h"

namespace bitsy_python {

BlockStack::BlockStack() {
  init();
  _top = INVALID_BLOCK;
}

void BlockStack::AllocTopBlock() {
  uint8_t next = alloc_block();
  blocks[next].prev = _top;
  blocks[_top].next = next;
  _top = next;
}

void BlockStack::FreeTopBlock() {
  uint8_t tofree = _top;
  _top = blocks[tofree].prev;
  free_block(tofree);
  blocks[_top].next = INVALID_BLOCK;
}

uint8_t* BlockStack::top() const {
  assert(_top != INVALID_BLOCK);
  return blocks[_top].bytes;
}

uint8_t BlockStack::GetSizeForTesting() const {
  uint8_t size = 0, size2 = 0, last = INVALID_BLOCK;
  for (uint8_t curr = _top; curr != INVALID_BLOCK; curr = blocks[curr].prev) {
    size++;
    last = curr;
  }
  if (size > 0) {
    assert(blocks[_top].next == INVALID_BLOCK);
    assert(blocks[last].prev == INVALID_BLOCK);
  }
  for (uint8_t curr = last; curr != INVALID_BLOCK; curr = blocks[curr].next) {
    size2++;
  }
  assert(size == size2);
  return size;
}

uint8_t BlockStack::blocksize() const { return BYTES_PER_BLOCK; }
}
