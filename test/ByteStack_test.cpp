#include "ByteStack.h"

#include <cassert>
#include <cstring>
#include <stack>

#include <stdio.h>
#include <stdlib.h>

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

int main() {
	test1();
	test2();
	printf(__FILE__ " passed\n");
	return 0;
}
