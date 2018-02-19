#include "ExecStack.h"

#include <stack>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitsy_alloc.h"
#include "test_common.h"

namespace bitsy_python {

Variable zero, one, hundred, kilo, mega, float_pi;

void initvars() {
  zero.set_int32(0);
  one.set_int32(1);
  hundred.set_int32(100);
  kilo.set_int32(1000);
  mega.set_int32(1000000);
  float_pi.set_float(3.14132f);
}

void compare(const Variable &a, const Variable &b) {
  assert(a.type == b.type);
  assert(a.size() == b.size());
  assert(!memcmp(&a.val, &b.val, a.size()));
}

void test1() {
  bitsy_alloc_init();
  ExecStack::init();

  ExecStack::push(zero);
  ExecStack::push(one);
  ExecStack::push(hundred);
  ExecStack::push(kilo);
  ExecStack::push(mega);
  ExecStack::push(float_pi);

  compare(ExecStack::pop(), float_pi);
  compare(ExecStack::pop(), mega);
  compare(ExecStack::pop(), kilo);
  compare(ExecStack::pop(), hundred);
  compare(ExecStack::pop(), one);
  compare(ExecStack::pop(), zero);
}

void test2() {
  std::stack<Variable> stack_expected;
  bitsy_alloc_init();
  ExecStack::init();

  int test_iter=1000;
  while(test_iter--) {
    Variable v;
    if (stack_expected.empty() || rand()%2==0) {
      auto type = rand()%5 + 1;
      switch(type) {
      case 1:
        v.set_int32(rand()%0xFF);
        break;
      case 2:
        v.set_int32(rand()%0xFFFF);
        break;
      case 3:
        v.set_int32(rand()%0xFFFFFF);
        break;
      case 4:
        v.set_int32(rand()%0xFFFFFFFF);
        break;
      case 5:
        v.set_float((float)rand()/(float)(RAND_MAX / 10000));
        break;
      }
      ExecStack::push(v);
      stack_expected.push(v);
    } else {
      compare(ExecStack::pop(), stack_expected.top());
      stack_expected.pop();
    }
  }
}

void testCustomHeapVaiable() {
  bitsy_alloc_init();
  ExecStack::init();
  BitsyHeap::init();

  Variable heap_0, heap_1, heap_2;
  heap_0.type = Variable::CUSTOM;
  heap_0.val.custom_type.type = Variable::CustomType::STRING;
  heap_1 = heap_2 = heap_0;
  heap_0.val.custom_type.val = 0;
  heap_1.val.custom_type.val = 1;
  heap_2.val.custom_type.val = 2;

  assert(ExecStack::getCustomHeapVariableMap(0) == 0x0);
  ExecStack::push(heap_2);
  assert(ExecStack::getCustomHeapVariableMap(0) == 0x4);
  ExecStack::pop();
  assert(ExecStack::getCustomHeapVariableMap(0) == 0x0);
  assert(ExecStack::getCustomHeapVariableMap(32) == 0x0);

  ExecStack::push(heap_1);
  assert(ExecStack::getCustomHeapVariableMap(0) == 0x2);
  ExecStack::push(heap_2);
  assert(ExecStack::getCustomHeapVariableMap(0) == 0x6);
  ExecStack::pop();
  assert(ExecStack::getCustomHeapVariableMap(0) == 0x2);
  ExecStack::push(heap_0);
  assert(ExecStack::getCustomHeapVariableMap(0) == 0x3);
  ExecStack::push(heap_2);
  assert(ExecStack::getCustomHeapVariableMap(0) == 0x7);
  assert(ExecStack::getCustomHeapVariableMap(32) == 0x0);

  ExecStack::pop();
  assert(ExecStack::getCustomHeapVariableMap(0) == 0x3);
  ExecStack::pop();
  assert(ExecStack::getCustomHeapVariableMap(0) == 0x2);
  ExecStack::pop();
  assert(ExecStack::getCustomHeapVariableMap(0) == 0x0);
}
}

int main() {
  bitsy_python::initvars();
  bitsy_python::test1();
  bitsy_python::test2();
  bitsy_python::testCustomHeapVaiable();
  printf(__FILE__ " passed\n");
  return 0;
}
