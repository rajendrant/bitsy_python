#include "ByteStack.h"

#include <stack>

#include <assert.h>
#include <algorithm>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "test_common.h"

std::stack<uint8_t> stack_size;

void test1() {
  bitsy_alloc_init();

  int test_iter=1000;
  std::stack<uint8_t> stack_expected;
  bitsy_python::ByteStack::init();
  while(test_iter--) {
    uint8_t val[8] = {0,};
    if (stack_size.empty() || rand()%2==0) {
      uint8_t size = rand()%8 + 1;
      stack_size.push(size);
      for(int i=0; i<size; i++) {
        val[i] = rand()%255;
        stack_expected.push(val[i]);
      }
      bitsy_python::ByteStack::pushBytes(val, size);
    } else {
      uint8_t val_expected[8] = {0,};
      uint8_t size = stack_size.top();
      stack_size.pop();
      for(int i=0; i<size; i++) {
        val_expected[size-i-1] = stack_expected.top();
        stack_expected.pop();
      }
      bitsy_python::ByteStack::popBytes(val, size);
      assert(!memcmp(val, val_expected, size));
    }
  }
}

void test2() {
  bitsy_alloc_init();

  int test_iter=1000;
  std::stack<uint8_t> stack_expected;
  bitsy_python::ByteStack::init();
  while(test_iter--) {
    if (stack_expected.empty() || rand()%2==0) {
      uint8_t d = rand()%255;
      stack_expected.push(d);
      bitsy_python::ByteStack::pushByte(d);
    } else {
      assert(bitsy_python::ByteStack::popByte() == stack_expected.top());
      stack_expected.pop();
    }
  }
}

bool checkIterator(std::vector<uint8_t> expected) {
  std::vector<uint8_t> actual;
  uint8_t *bytes, len, p=0xFF;
  while(bitsy_python::ByteStack::getPrevByte(&p, &bytes, &len)) {
    for(int i=len; i!=0; i--)
      actual.push_back(bytes[i-1]);
  }
  std::reverse(actual.begin(), actual.end());
  return expected == actual;
}

void testIterator() {
  bitsy_alloc_init();

  bitsy_python::ByteStack::init();
  assert(checkIterator({}));
  bitsy_python::ByteStack::pushByte(101);
  assert(checkIterator({101}));
  bitsy_python::ByteStack::pushByte(23);
  assert(checkIterator({101, 23}));
  bitsy_python::ByteStack::pushByte(45);
  assert(checkIterator({101, 23, 45}));

  bitsy_python::ByteStack::popByte();
  assert(checkIterator({101, 23}));
  bitsy_python::ByteStack::popByte();
  assert(checkIterator({101}));
  bitsy_python::ByteStack::popByte();
  assert(checkIterator({}));
}

void testIterator2() {
  bitsy_alloc_init();

  bitsy_python::ByteStack::init();
  std::vector<uint8_t> expected;
  for(int iter=0; iter<1000; iter++) {
    uint8_t v = rand()%200;
    bitsy_python::ByteStack::pushByte(v);
    expected.push_back(v);
    assert(checkIterator(expected));

    if (rand()%2==0) {
      bitsy_python::ByteStack::popByte();
      expected.pop_back();
      assert(checkIterator(expected));
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
