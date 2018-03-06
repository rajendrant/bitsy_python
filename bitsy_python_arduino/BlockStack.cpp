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

bool BlockStack::getPrevBlock(uint8_t *id, uint8_t **ret) const {
  *id = (*id==0xFF) ? _top : blocks[*id].prev;
  if (*id==INVALID_BLOCK)
    return false;
  *ret = blocks[*id].bytes;
  return true;
}

}
