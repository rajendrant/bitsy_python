#include "BlockStack.h"

#include <cassert>
#include <cstring>

#include <stdio.h>

namespace bitsy_python {

class BlockStackTest {
public:
void test1() {
	assert(s.GetSizeForTesting() == 0);
	s.AllocTopBlock();
	assert(s.GetSizeForTesting() == 1);
	auto first = s.top();

	s.AllocTopBlock();
	assert(s.GetSizeForTesting() == 2);
	auto second = s.top();
	assert(s.top() == second);

	s.FreeTopBlock();
	assert(s.GetSizeForTesting() == 1);
	assert(s.top() == first);
	s.FreeTopBlock();
	assert(s.GetSizeForTesting() == 0);

	s.AllocTopBlock();
	s.AllocTopBlock();
	s.AllocTopBlock();
	assert(s.GetSizeForTesting() == 3);
	s.FreeTopBlock();
	s.FreeTopBlock();
	s.FreeTopBlock();
	assert(s.GetSizeForTesting() == 0);
}
private:
BlockStack s;
};

}

int main() {
	bitsy_python::BlockStackTest t;
	t.test1();
	printf(__FILE__ " passed\n");
	return 0;
}
