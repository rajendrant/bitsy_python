#include "FunctionStack.h"

#include <assert.h>
#include <string.h>

#include <stdio.h>

namespace bitsy_python {

bool FunctionStack::is_empty() {
	return top == 0;
}

#define HDR_START 5
#define HDR_SIZE_FOR_VARS(v) ((((v)-1)*2)/4+1)

void FunctionStack::setup_function_call(uint8_t args, uint8_t vars, uint16_t old_ins_ptr) {
	stack[top] = vars;
	memcpy(stack+top+1, &old_ins_ptr, 2);
	memcpy(stack+top+3, &start, 2);
	start = top;
	top += HDR_START + HDR_SIZE_FOR_VARS(vars) + vars;
	memset(stack+start+HDR_START, 0, top-start);
}

bool FunctionStack::return_function(uint16_t *old_ins_ptr) {
	top = start;
	memcpy(old_ins_ptr, stack+start+1, 2);
	memcpy(&start, stack+start+3, 2);
	return is_empty();
}

uint8_t FunctionStack::get_var_hdr(uint8_t n) const {
	return ((stack[start+HDR_START+n/4]>>(2*(n%4))) & 0x3);
}

void FunctionStack::set_var_hdr(uint8_t n, uint8_t val) {
	stack[start+HDR_START+n/4] = (stack[start+HDR_START+n/4] & ~(0x3<<((2*(n%4))))) | (val&0x3)<<((2*(n%4)));
}

Variable FunctionStack::getNthVariable(uint8_t n) const {
	assert(n < stack[start]);
	Variable v;
	uint16_t pre = start + HDR_START + HDR_SIZE_FOR_VARS(stack[start]);
	for(uint8_t i=0; i<n; i++) {
		pre += get_var_hdr(i) + 1;
	}
	auto size = get_var_hdr(n) + 1;
	v.type = Variable::get_type_from_size_and_extra_bits(size, get_var_hdr(n+stack[start]));
	memcpy(&v.val, stack+pre, size);
	return v;
}

void FunctionStack::setNthVariable(uint8_t n, const Variable v) {
	assert(n < stack[start]);
	uint16_t pre = start + HDR_START + HDR_SIZE_FOR_VARS(stack[start]);
	for(uint8_t i=0; i<n; i++) {
		pre += get_var_hdr(i) + 1;
	}
	auto old_size = get_var_hdr(n) + 1;
	auto size = v.size();
	set_var_hdr(n+stack[start], v.get_extra_bits());
	if (size != old_size) {
		set_var_hdr(n, size-1);
		memmove(stack+pre+size, stack+pre+old_size, top-pre-old_size);
		if (size > old_size) top += size-old_size;
		else top -= old_size-size;
	}
	memcpy(stack+pre, &v.val, size);
}
}
