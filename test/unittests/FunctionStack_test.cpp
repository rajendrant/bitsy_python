#include "FunctionStack.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "test_common.h"

namespace bitsy_python {

FunctionStack f;
Variable zero, one, hundred, kilo, mega, float_pi;
uint16_t ins_ptr = 0;

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
	assert(f.is_empty());
	f.setup_function_call(0, 0, ins_ptr);
	assert(!f.is_empty());
	f.return_function(&ins_ptr);
	assert(f.is_empty());

	assert(f.is_empty());
	f.setup_function_call(1, 1, ins_ptr);
	assert(!f.is_empty());
	f.return_function(&ins_ptr);
	assert(f.is_empty());
}

void test2() {
	assert(f.is_empty());
	f.setup_function_call(1, 1, ins_ptr);
	f.setup_function_call(0, 0, ins_ptr);
	assert(!f.is_empty());
	f.return_function(&ins_ptr);
	assert(!f.is_empty());
	f.return_function(&ins_ptr);
	assert(f.is_empty());
}

void testvar1() {
	assert(f.is_empty());
	f.setup_function_call(0, 4, ins_ptr);
	compare(f.getNthVariable(0), zero);
	compare(f.getNthVariable(1), zero);
	compare(f.getNthVariable(2), zero);
	compare(f.getNthVariable(3), zero);
	f.setNthVariable(0, one);
	compare(f.getNthVariable(0), one);
	compare(f.getNthVariable(1), zero);
	compare(f.getNthVariable(2), zero);
	compare(f.getNthVariable(3), zero);
	f.setNthVariable(1, kilo);
	compare(f.getNthVariable(0), one);
	compare(f.getNthVariable(1), kilo);
	compare(f.getNthVariable(2), zero);
	compare(f.getNthVariable(3), zero);
	f.setNthVariable(3, mega);
	compare(f.getNthVariable(0), one);
	compare(f.getNthVariable(1), kilo);
	compare(f.getNthVariable(2), zero);
	compare(f.getNthVariable(3), mega);
	f.setNthVariable(0, hundred);
	f.setNthVariable(1, mega);
	f.setNthVariable(2, mega);
	compare(f.getNthVariable(0), hundred);
	compare(f.getNthVariable(1), mega);
	compare(f.getNthVariable(2), mega);
	compare(f.getNthVariable(3), mega);

	// Now reduce.
	f.setNthVariable(1, hundred);
	compare(f.getNthVariable(0), hundred);
	compare(f.getNthVariable(1), hundred);
	compare(f.getNthVariable(2), mega);
	compare(f.getNthVariable(3), mega);
	f.setNthVariable(2, hundred);
	compare(f.getNthVariable(0), hundred);
	compare(f.getNthVariable(1), hundred);
	compare(f.getNthVariable(2), hundred);
	compare(f.getNthVariable(3), mega);
	f.setNthVariable(3, hundred);
	compare(f.getNthVariable(0), hundred);
	compare(f.getNthVariable(1), hundred);
	compare(f.getNthVariable(2), hundred);
	compare(f.getNthVariable(3), hundred);

	// Now set to float.
	f.setNthVariable(0, float_pi);
	f.setNthVariable(1, float_pi);
	f.setNthVariable(2, float_pi);
	f.setNthVariable(3, float_pi);
	compare(f.getNthVariable(0), float_pi);
	compare(f.getNthVariable(1), float_pi);
	compare(f.getNthVariable(2), float_pi);
	compare(f.getNthVariable(3), float_pi);
	f.return_function(&ins_ptr);
	assert(f.is_empty());
}

void testvarmultiple() {
	assert(f.is_empty());
	f.setup_function_call(0, 5, ins_ptr);
	compare(f.getNthVariable(0), zero);
	compare(f.getNthVariable(1), zero);
	compare(f.getNthVariable(2), zero);
	compare(f.getNthVariable(3), zero);
	compare(f.getNthVariable(4), zero);
	f.setNthVariable(0, hundred);
	f.setNthVariable(1, kilo);
	f.setNthVariable(2, mega);
	f.setNthVariable(3, float_pi);
	compare(f.getNthVariable(0), hundred);
	compare(f.getNthVariable(1), kilo);
	compare(f.getNthVariable(2), mega);
	compare(f.getNthVariable(3), float_pi);
	compare(f.getNthVariable(4), zero);

	f.setup_function_call(0, 3, ins_ptr);
	compare(f.getNthVariable(0), zero);
	compare(f.getNthVariable(1), zero);
	compare(f.getNthVariable(2), zero);
	f.setNthVariable(0, mega);
	f.setNthVariable(1, hundred);
	f.setNthVariable(2, float_pi);
	compare(f.getNthVariable(0), mega);
	compare(f.getNthVariable(1), hundred);
	compare(f.getNthVariable(2), float_pi);
	f.return_function(&ins_ptr);

	compare(f.getNthVariable(0), hundred);
	compare(f.getNthVariable(1), kilo);
	compare(f.getNthVariable(2), mega);
	compare(f.getNthVariable(3), float_pi);
	compare(f.getNthVariable(4), zero);
	f.return_function(&ins_ptr);
	assert(f.is_empty());
}

void test_all() {
	initvars();
	test1();
	test2();
	testvar1();
	testvarmultiple();
}

}

int main() {
	bitsy_python::test_all();
	printf(__FILE__ " passed\n");
	return 0;
}
