#include "FunctionStack.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "bitsy_alloc.h"
#include "test_common.h"

namespace bitsy_python {

Variable zero, one, hundred, kilo, mega, float_pi;
uint16_t ins_ptr = 0;
bool is_callback_mode;

void initvars() {
  zero.set_int32(0);
  one.set_int32(1);
  hundred.set_int32(100);
  kilo.set_int32(1000);
  mega.set_int32(1000000);
  float_pi.set_float(3.14132f);
}

void compare(Variable a, Variable b) {
  assert(a.type == b.type);
  assert(a.size() == b.size());
  assert(!memcmp(&a.val, &b.val, a.size()));
}

void test1() {
  bitsy_alloc_init();
  assert(FunctionStack::is_empty());
  FunctionStack::setup_function_call(0, ins_ptr);
  assert(!FunctionStack::is_empty());
  FunctionStack::return_function(&ins_ptr, &is_callback_mode);
  assert(FunctionStack::is_empty());

  assert(FunctionStack::is_empty());
  FunctionStack::setup_function_call(1, ins_ptr);
  assert(!FunctionStack::is_empty());
  FunctionStack::return_function(&ins_ptr, &is_callback_mode);
  assert(FunctionStack::is_empty());
}

void test2() {
  bitsy_alloc_init();
  assert(FunctionStack::is_empty());
  FunctionStack::setup_function_call(1, ins_ptr);
  FunctionStack::setup_function_call(0, ins_ptr);
  assert(!FunctionStack::is_empty());
  FunctionStack::return_function(&ins_ptr, &is_callback_mode);
  assert(!FunctionStack::is_empty());
  FunctionStack::return_function(&ins_ptr, &is_callback_mode);
  assert(FunctionStack::is_empty());
}

void testvar1() {
  bitsy_alloc_init();
  assert(FunctionStack::is_empty());
  FunctionStack::setup_function_call(4, ins_ptr);
  compare(FunctionStack::getNthVariable(0), zero);
  compare(FunctionStack::getNthVariable(1), zero);
  compare(FunctionStack::getNthVariable(2), zero);
  compare(FunctionStack::getNthVariable(3), zero);
  FunctionStack::setNthVariable(0, one);
  compare(FunctionStack::getNthVariable(0), one);
  compare(FunctionStack::getNthVariable(1), zero);
  compare(FunctionStack::getNthVariable(2), zero);
  compare(FunctionStack::getNthVariable(3), zero);
  FunctionStack::setNthVariable(1, kilo);
  compare(FunctionStack::getNthVariable(0), one);
  compare(FunctionStack::getNthVariable(1), kilo);
  compare(FunctionStack::getNthVariable(2), zero);
  compare(FunctionStack::getNthVariable(3), zero);
  FunctionStack::setNthVariable(3, mega);
  compare(FunctionStack::getNthVariable(0), one);
  compare(FunctionStack::getNthVariable(1), kilo);
  compare(FunctionStack::getNthVariable(2), zero);
  compare(FunctionStack::getNthVariable(3), mega);
  FunctionStack::setNthVariable(0, hundred);
  FunctionStack::setNthVariable(1, mega);
  FunctionStack::setNthVariable(2, mega);
  compare(FunctionStack::getNthVariable(0), hundred);
  compare(FunctionStack::getNthVariable(1), mega);
  compare(FunctionStack::getNthVariable(2), mega);
  compare(FunctionStack::getNthVariable(3), mega);

  // Now reduce.
  FunctionStack::setNthVariable(1, hundred);
  compare(FunctionStack::getNthVariable(0), hundred);
  compare(FunctionStack::getNthVariable(1), hundred);
  compare(FunctionStack::getNthVariable(2), mega);
  compare(FunctionStack::getNthVariable(3), mega);
  FunctionStack::setNthVariable(2, hundred);
  compare(FunctionStack::getNthVariable(0), hundred);
  compare(FunctionStack::getNthVariable(1), hundred);
  compare(FunctionStack::getNthVariable(2), hundred);
  compare(FunctionStack::getNthVariable(3), mega);
  FunctionStack::setNthVariable(3, hundred);
  compare(FunctionStack::getNthVariable(0), hundred);
  compare(FunctionStack::getNthVariable(1), hundred);
  compare(FunctionStack::getNthVariable(2), hundred);
  compare(FunctionStack::getNthVariable(3), hundred);

  // Now set to float.
  FunctionStack::setNthVariable(0, float_pi);
  FunctionStack::setNthVariable(1, float_pi);
  FunctionStack::setNthVariable(2, float_pi);
  FunctionStack::setNthVariable(3, float_pi);
  compare(FunctionStack::getNthVariable(0), float_pi);
  compare(FunctionStack::getNthVariable(1), float_pi);
  compare(FunctionStack::getNthVariable(2), float_pi);
  compare(FunctionStack::getNthVariable(3), float_pi);
  FunctionStack::return_function(&ins_ptr, &is_callback_mode);
  assert(FunctionStack::is_empty());
}

void testvarmultiple() {
  bitsy_alloc_init();
  assert(FunctionStack::is_empty());
  FunctionStack::setup_function_call(5, ins_ptr);
  compare(FunctionStack::getNthVariable(0), zero);
  compare(FunctionStack::getNthVariable(1), zero);
  compare(FunctionStack::getNthVariable(2), zero);
  compare(FunctionStack::getNthVariable(3), zero);
  compare(FunctionStack::getNthVariable(4), zero);
  FunctionStack::setNthVariable(0, hundred);
  FunctionStack::setNthVariable(1, kilo);
  FunctionStack::setNthVariable(2, mega);
  FunctionStack::setNthVariable(3, float_pi);
  compare(FunctionStack::getNthVariable(0), hundred);
  compare(FunctionStack::getNthVariable(1), kilo);
  compare(FunctionStack::getNthVariable(2), mega);
  compare(FunctionStack::getNthVariable(3), float_pi);
  compare(FunctionStack::getNthVariable(4), zero);

  FunctionStack::setup_function_call(3, ins_ptr);
  compare(FunctionStack::getNthVariable(0), zero);
  compare(FunctionStack::getNthVariable(1), zero);
  compare(FunctionStack::getNthVariable(2), zero);
  FunctionStack::setNthVariable(0, mega);
  FunctionStack::setNthVariable(1, hundred);
  FunctionStack::setNthVariable(2, float_pi);
  compare(FunctionStack::getNthVariable(0), mega);
  compare(FunctionStack::getNthVariable(1), hundred);
  compare(FunctionStack::getNthVariable(2), float_pi);
  FunctionStack::return_function(&ins_ptr, &is_callback_mode);

  compare(FunctionStack::getNthVariable(0), hundred);
  compare(FunctionStack::getNthVariable(1), kilo);
  compare(FunctionStack::getNthVariable(2), mega);
  compare(FunctionStack::getNthVariable(3), float_pi);
  compare(FunctionStack::getNthVariable(4), zero);
  FunctionStack::return_function(&ins_ptr, &is_callback_mode);
  assert(FunctionStack::is_empty());
}

void testCustomHeapVaiable() {
  bitsy_alloc_init();
  Variable heap_0, heap_1, heap_2;
  heap_0.type = Variable::CUSTOM;
  heap_0.val.custom_type.type = Variable::CustomType::STRING;
  heap_1 = heap_2 = heap_0;
  heap_0.val.custom_type.val = 0;
  heap_1.val.custom_type.val = 1;
  heap_2.val.custom_type.val = 2;

  assert(FunctionStack::is_empty());
  FunctionStack::setup_function_call(5, ins_ptr);
  assert(FunctionStack::getCustomHeapVariableMap(0) == 0x0);
  FunctionStack::setNthVariable(0, heap_0);
  assert(FunctionStack::getCustomHeapVariableMap(0) == 0x1);
  FunctionStack::setNthVariable(1, heap_1);
  assert(FunctionStack::getCustomHeapVariableMap(0) == 0x3);

  FunctionStack::setup_function_call(3, ins_ptr);
  assert(FunctionStack::getCustomHeapVariableMap(0) == 0x3);
  FunctionStack::setNthVariable(0, heap_2);
  assert(FunctionStack::getCustomHeapVariableMap(0) == 0x7);

  FunctionStack::return_function(&ins_ptr, &is_callback_mode);
  FunctionStack::return_function(&ins_ptr, &is_callback_mode);
  assert(FunctionStack::getCustomHeapVariableMap(0) == 0x0);
}

void test_all() {
  initvars();
  test1();
  test2();
  testvar1();
  testvarmultiple();
  testCustomHeapVaiable();
}

}

int main() {
  bitsy_python::test_all();
  printf(__FILE__ " passed\n");
  return 0;
}
