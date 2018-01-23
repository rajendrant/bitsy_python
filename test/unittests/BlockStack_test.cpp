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
  assert(GetSizeForTesting() == 0);
  s.AllocTopBlock();
  assert(GetSizeForTesting() == 1);
  auto first = s.top();

  s.AllocTopBlock();
  assert(GetSizeForTesting() == 2);
  auto second = s.top();
  assert(s.top() == second);

  s.FreeTopBlock();
  assert(GetSizeForTesting() == 1);
  assert(s.top() == first);
  s.FreeTopBlock();
  assert(GetSizeForTesting() == 0);

  s.AllocTopBlock();
  s.AllocTopBlock();
  s.AllocTopBlock();
  assert(GetSizeForTesting() == 3);
  s.FreeTopBlock();
  s.FreeTopBlock();
  s.FreeTopBlock();
  assert(GetSizeForTesting() == 0);
}

bool checkIterator(uint8_t count, uint8_t *b0=NULL, uint8_t *b1=NULL, uint8_t *b2=NULL) {
  std::vector<uint8_t*> actual;
  uint32_t id = INVALID_ITERATOR;
  uint8_t *ret;
  while(s.getNextBlock(&id, &ret))
    actual.push_back(ret);
  if (count != actual.size())
    return false;
  return !((count>0 && b0!=actual[0]) || (count>1 && b1!=actual[1]) ||
           (count>2 && b2!=actual[2]));
}

void testIterator() {
  uint8_t *b0, *b1, *b2;
  assert(checkIterator(0));
  s.AllocTopBlock();
  b0=s.top();
  assert(checkIterator(1, b0));
  s.AllocTopBlock();
  b1=s.top();
  assert(checkIterator(2, b0, b1));
  s.AllocTopBlock();
  b2=s.top();
  assert(checkIterator(3, b0, b1, b2));

  s.FreeTopBlock();
  assert(checkIterator(2, b0, b1));
  s.FreeTopBlock();
  assert(checkIterator(1, b0));
  s.FreeTopBlock();
  assert(checkIterator(0));
}

private:
BlockStack s;

uint8_t GetSizeForTesting() const {
  Block *blocks = s.GetBlocksForTesting();
  uint8_t size = 0, size2 = 0, last = INVALID_BLOCK;
  for (uint8_t curr = s._top; curr != INVALID_BLOCK; curr = blocks[curr].prev) {
    size++;
    last = curr;
  }
  if (size > 0) {
    assert(blocks[s._top].next == INVALID_BLOCK);
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
