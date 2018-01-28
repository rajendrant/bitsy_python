#include "BitStack.h"

#include "test_common.h"

#include <cassert>
#include <cstring>
#include <stack>
#include <vector>

#include <stdio.h>
#include <stdlib.h>

void test1() {
  bitsy_python::BitStack stack;
  std::stack<uint8_t> stack_expected;
  for(int i=0; i<20000; i++) {
    if (stack_expected.empty() || rand()%2==0) {
      uint8_t d = rand() % 4;
      stack_expected.push(d);
      stack.pushTwoBits(d);
    } else {
      assert(stack.popTwoBits() == stack_expected.top());
      stack_expected.pop();
    }
  }
}

bool checkIterator(const bitsy_python::BitStack &s, uint8_t count,
                   uint8_t b0=0, uint8_t b1=0, uint8_t b2=0) {
  std::vector<uint8_t> actual;
  uint32_t id = INVALID_ITERATOR;
  uint8_t ret;
  while(s.getNextTwoBits(&ret, &id))
    actual.push_back(ret);
  if (count != actual.size())
    return false;
  return !((count>0 && b0!=actual[0]) || (count>1 && b1!=actual[1]) ||
           (count>2 && b2!=actual[2]));
}

void testIterator() {
  bitsy_python::BitStack s;
  assert(checkIterator(s, 0));
  s.pushTwoBits(3);
  assert(checkIterator(s, 1, 3));
  s.popTwoBits();
  assert(checkIterator(s, 0));
  s.pushTwoBits(1);
  assert(checkIterator(s, 1, 1));
  s.pushTwoBits(2);
  assert(checkIterator(s, 2, 1, 2));
  s.pushTwoBits(3);
  assert(checkIterator(s, 3, 1, 2, 3));

  s.popTwoBits();
  assert(checkIterator(s, 2, 1, 2));
  s.popTwoBits();
  assert(checkIterator(s, 1, 1));
  s.popTwoBits();
  assert(checkIterator(s, 0));
}

int main() {
  test1();
  testIterator();
  printf(__FILE__ " passed\n");
  return 0;
}
