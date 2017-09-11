#ifndef BLOCKSTACK_H_
#define BLOCKSTACK_H_

#include <stdint.h>

#include "bitsylimit.h"

namespace bitsy_python {

class BlockStack {
public:
	BlockStack();
	void AllocTopBlock();
	void FreeTopBlock();
	uint8_t *top() const;
	uint8_t blocksize() const;
private:
	friend class BlockStackTest;
	uint8_t GetSizeForTesting() const;

	uint8_t _top : BITS_PER_BLOCK_ID;
};

}
#endif /* BLOCKSTACK_H_ */
