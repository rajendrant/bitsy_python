#include "BlockStack.h"

namespace bitsy_python {
namespace BlockStack {

uint8_t _top; // : BITS_PER_BLOCK_ID

void init() {
  _top = INVALID_BLOCK;
}

void AllocTopBlock() {
  uint8_t next = alloc_block();
  blocks[next].prev = _top;
  blocks[_top].next = next;
  _top = next;
}

void FreeTopBlock() {
  uint8_t tofree = _top;
  _top = blocks[tofree].prev;
  free_block(tofree);
  blocks[_top].next = INVALID_BLOCK;
}

uint8_t* top() {
  BITSY_ASSERT(_top != INVALID_BLOCK);
  return blocks[_top].bytes;
}

uint8_t blocksize() { return BYTES_PER_BLOCK; }

bool getPrevBlock(uint8_t *id, uint8_t **ret) {
  *id = (*id==0xFF) ? _top : blocks[*id].prev;
  if (*id==INVALID_BLOCK)
    return false;
  *ret = blocks[*id].bytes;
  return true;
}

}
}
