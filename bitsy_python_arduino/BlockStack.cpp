#include "BlockStack.h"

namespace bitsy_python {

void BlockStack::init() {
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
  BITSY_ASSERT(_top != INVALID_BLOCK);
  return blocks[_top].bytes;
}

Block* BlockStack::GetBlocksForTesting() const { return blocks; }

uint8_t BlockStack::blocksize() const { return BYTES_PER_BLOCK; }

bool BlockStack::getNextBlock(uint32_t *id, uint8_t **ret) const {
  if(_top==INVALID_BLOCK)
    return false;
  if (*id==INVALID_ITERATOR) {
    for (uint8_t curr = _top; curr != INVALID_BLOCK; curr = blocks[curr].prev)
      *id = curr;
  } else {
      *id = blocks[*id].next;
  }
  if (*id==INVALID_BLOCK)
    return false;
  *ret = blocks[*id].bytes;
  return true;
}

}
