#ifndef BLOCKSTACK_H_
#define BLOCKSTACK_H_

#include <stdint.h>

#include "bitsylimit.h"
#include "bitsy_alloc.h"

namespace bitsy_python {

class BlockStack {
 public:
  void init();
  void AllocTopBlock();
  void FreeTopBlock();
  uint8_t *top() const;
  uint8_t blocksize() const;

  bool getPrevBlock(uint8_t *id, uint8_t **ret) const;

 private:
  friend class BlockStackTest;
  Block* GetBlocksForTesting() const;

  uint8_t _top; // : BITS_PER_BLOCK_ID
};
}
#endif /* BLOCKSTACK_H_ */
