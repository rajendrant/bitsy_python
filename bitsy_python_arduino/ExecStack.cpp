#include "ExecStack.h"

namespace bitsy_python {

void ExecStack::push(const Variable &v) {
	hdr.pushThreeBits(v.type);
	data.pushBytes((uint8_t*)&v.val, v.size());
}

Variable ExecStack::pop() {
	Variable v;
	v.type = hdr.popThreeBits();
	data.popBytes((uint8_t*)&v.val, v.size());
	return v;
}
}
