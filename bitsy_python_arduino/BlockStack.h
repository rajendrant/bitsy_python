#ifndef BLOCKSTACK_H_
#define BLOCKSTACK_H_

#include <stdint.h>

#include "bitsylimit.h"
#include "bitsy_alloc.h"

namespace bitsy_python {
namespace BlockStack {

extern uint8_t _top;

void init();
void AllocTopBlock();
void FreeTopBlock();
uint8_t *top();
uint8_t blocksize();

bool getPrevBlock(uint8_t *id, uint8_t **ret);

}
}
#endif /* BLOCKSTACK_H_ */
