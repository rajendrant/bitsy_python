#include "ByteStack.h"

#include <stack>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "test_common.h"

bitsy_python::ByteStack stack;
std::stack<uint8_t> stack_size;
std::stack<uint8_t> stack_expected;

void test1() {
  int test_iter=1000;
  while(test_iter--) {
    uint8_t val[8] = {0,};
    if (stack_size.empty() || rand()%2==0) {
      uint8_t size = rand()%8 + 1;
      stack_size.push(size);
      for(int i=0; i<size; i++) {
        val[i] = rand()%255;
        stack_expected.push(val[i]);
      }
      stack.pushBytes(val, size);
    } else {
      uint8_t val_expected[8] = {0,};
      uint8_t size = stack_size.top();
      stack_size.pop();
      for(int i=0; i<size; i++) {
        val_expected[size-i-1] = stack_expected.top();
        stack_expected.pop();
      }
      stack.popBytes(val, size);
      assert(!memcmp(val, val_expected, size));
    }
  }
}

void test2() {
  int test_iter=1000;
  while(test_iter--) {
    if (stack_expected.empty() || rand()%2==0) {
      uint8_t d = rand()%255;
      stack_expected.push(d);
      stack.pushByte(d);
    } else {
      assert(stack.popByte() == stack_expected.top());
      stack_expected.pop();
    }
  }
}

bool checkIterator(const bitsy_python::ByteStack &s,
                   std::vector<uint8_t> expected) {
  std::vector<uint8_t> actual;
  uint8_t *bytes, len;
  uint32_t p=INVALID_ITERATOR;
  while(s.getNextByte(&p, &bytes, &len)) {
    for(int i=0; i<len; i++)
      actual.push_back(bytes[i]);
  }
  return expected == actual;
}

void testIterator() {
  bitsy_python::ByteStack s;
  assert(checkIterator(s, {}));
  s.pushByte(101);
  assert(checkIterator(s, {101}));
  s.pushByte(23);
  assert(checkIterator(s, {101, 23}));
  s.pushByte(45);
  assert(checkIterator(s, {101, 23, 45}));

  s.popByte();
  assert(checkIterator(s, {101, 23}));
  s.popByte();
  assert(checkIterator(s, {101}));
  s.popByte();
  assert(checkIterator(s, {}));
}

void testIterator2() {
  bitsy_python::ByteStack s;
  std::vector<uint8_t> expected;
  for(int iter=0; iter<1000; iter++) {
    uint8_t v = rand()%200;
    s.pushByte(v);
    expected.push_back(v);
    assert(checkIterator(s, expected));

    if (rand()%2==0) {
      s.popByte();
      expected.pop_back();
      assert(checkIterator(s, expected));
    }
  }
}

int main() {
  test1();
  test2();
  testIterator();
  testIterator2();
  printf(__FILE__ " passed\n");
  return 0;
}
