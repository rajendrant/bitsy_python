#include "BitStack.h"

#include <cassert>
#include <cstring>
#include <stack>

#include <stdio.h>
#include <stdlib.h>

bitsy_python::BitStack stack;
std::stack<uint8_t> stack_expected;

void test1() {
	for(int i=0; i<20000; i++) {
		if (stack_expected.empty() || rand()%2==0) {
			uint8_t d = rand() % 8;
			stack_expected.push(d);
			stack.pushThreeBits(d);
		} else {
			assert(stack.popThreeBits() == stack_expected.top());
			stack_expected.pop();
		}
	}
}

int main() {
	test1();
	printf(__FILE__ " passed\n");
	return 0;
}
