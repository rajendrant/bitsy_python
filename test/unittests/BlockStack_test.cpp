#include "BlockStack.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <vector>

#include "test_common.h"
#include "bitsy_alloc.h"

namespace bitsy_python {

class BlockStackTest {
public:
void test1() {
  bitsy_alloc_init();
  BlockStack::init();

  assert(GetSizeForTesting() == 0);
  BlockStack::AllocTopBlock();
  assert(GetSizeForTesting() == 1);
  auto first = BlockStack::top();

  BlockStack::AllocTopBlock();
  assert(GetSizeForTesting() == 2);
  auto second = BlockStack::top();
  assert(BlockStack::top() == second);

  BlockStack::FreeTopBlock();
  assert(GetSizeForTesting() == 1);
  assert(BlockStack::top() == first);
  BlockStack::FreeTopBlock();
  assert(GetSizeForTesting() == 0);

  BlockStack::AllocTopBlock();
  BlockStack::AllocTopBlock();
  BlockStack::AllocTopBlock();
  assert(GetSizeForTesting() == 3);
  BlockStack::FreeTopBlock();
  BlockStack::FreeTopBlock();
  BlockStack::FreeTopBlock();
  assert(GetSizeForTesting() == 0);
}

bool checkIterator(uint8_t count, uint8_t *b0=NULL, uint8_t *b1=NULL, uint8_t *b2=NULL) {
  std::vector<uint8_t*> actual;
  uint8_t id = 0xFF, *ret;
  while(BlockStack::getPrevBlock(&id, &ret))
    actual.insert(actual.begin(), ret);
  if (count != actual.size())
    return false;
  return !((count>0 && b0!=actual[0]) || (count>1 && b1!=actual[1]) ||
           (count>2 && b2!=actual[2]));
}

void testIterator() {
  bitsy_alloc_init();
  BlockStack::init();

  uint8_t *b0, *b1, *b2;
  assert(checkIterator(0));
  BlockStack::AllocTopBlock();
  b0=BlockStack::top();
  assert(checkIterator(1, b0));
  BlockStack::AllocTopBlock();
  b1=BlockStack::top();
  assert(checkIterator(2, b0, b1));
  BlockStack::AllocTopBlock();
  b2=BlockStack::top();
  assert(checkIterator(3, b0, b1, b2));

  BlockStack::FreeTopBlock();
  assert(checkIterator(2, b0, b1));
  BlockStack::FreeTopBlock();
  assert(checkIterator(1, b0));
  BlockStack::FreeTopBlock();
  assert(checkIterator(0));
}

private:

uint8_t GetSizeForTesting() const {
  uint8_t size = 0, size2 = 0, last = INVALID_BLOCK;
  for (uint8_t curr = BlockStack::_top; curr != INVALID_BLOCK; curr = blocks[curr].prev) {
    size++;
    last = curr;
  }
  if (size > 0) {
    assert(blocks[BlockStack::_top].next == INVALID_BLOCK);
    assert(blocks[last].prev == INVALID_BLOCK);
  }
  for (uint8_t curr = last; curr != INVALID_BLOCK; curr = blocks[curr].next) {
    size2++;
  }
  assert(size == size2);
  return size;
}

};

}

int main() {
  bitsy_python::BlockStackTest t;
  t.test1();
  t.testIterator();
  printf(__FILE__ " passed\n");
  return 0;
}
