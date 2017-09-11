#ifndef BITSTACK_H_
#define BITSTACK_H_

#include "ByteStack.h"

namespace bitsy_python {

class BitStack : ByteStack {
public:
	BitStack();
	void pushThreeBits(uint8_t bits);
	uint8_t popThreeBits();
private:
	uint16_t byte=0, pos = 0;
};

}
#endif /* BITSTACK_H_ */
