#include "ExecStack.h"

#include <stack>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "test_common.h"

namespace bitsy_python {

ExecStack stack;
std::stack<Variable> stack_expected;

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
  stack.push(zero);
  stack.push(one);
  stack.push(hundred);
  stack.push(kilo);
  stack.push(mega);
  stack.push(float_pi);

  compare(stack.pop(), float_pi);
  compare(stack.pop(), mega);
  compare(stack.pop(), kilo);
  compare(stack.pop(), hundred);
  compare(stack.pop(), one);
  compare(stack.pop(), zero);
}

void test2() {
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
      stack.push(v);
      stack_expected.push(v);
    } else {
      compare(stack.pop(), stack_expected.top());
      stack_expected.pop();
    }
  }
}
}

int main() {
  bitsy_python::initvars();
  bitsy_python::test1();
  bitsy_python::test2();
  printf(__FILE__ " passed\n");
  return 0;
}
