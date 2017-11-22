#include "Program.h"

#include "instructions.h"
#include "datatypes/datatype.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define HEADER_PER_FUNCTION 10

namespace bitsy_python {

uint8_t Program::get_next_instruction(Variable *arg) {
	uint8_t ins = 0;
	if (bits.get_bit8(ins_ptr, 1)==0) {
		ins = bits.get_bit8(ins_ptr+1, 4);
		ins_ptr += 5;
	} else {
		ins = bits.get_bit8(ins_ptr+1, 7);
		ins_ptr += 8;
	}
	uint8_t global_type=0;
	if (ins==LOAD_GLOBAL || ins==STORE_GLOBAL || ins==DELETE_GLOBAL) {
		// 2 bit indicates whether its a function, global, builtin, module.
		global_type = bits.get_bit8(ins_ptr, 2);
		ins_ptr += 2;
	}
	if (is_instr_arg(ins)) {
		uint8_t next;
		*arg = get_number(ins_ptr, &next);
		ins_ptr += next;
		//printf("arg %d\n", arg->as_int32());
	}
	if (ins==LOAD_GLOBAL || ins==STORE_GLOBAL || ins==DELETE_GLOBAL) {
		switch(global_type) {
		case 0:
			// TODO: global variable.
			assert(false);
		case 1:
			*arg = Variable::FunctionVariable(arg->as_int32());
			break;
		case 2:
			arg->set_CustomType(Variable::CustomType::BUILTIN_FUNCTION, arg->as_int16());
			break;
		case 3:
			*arg = Variable::ModuleVariable(arg->as_int16());
			break;
		default:
			assert(false);
		}
	}
	switch(ins) {
	case POP_JUMP_IF_TRUE:
	case POP_JUMP_IF_FALSE:
	case JUMP_IF_TRUE_OR_POP:
	case JUMP_IF_FALSE_OR_POP:
	case JUMP_ABSOLUTE:
	//case CONTINUE_LOOP:
	//case SETUP_LOOP:
	case FOR_ITER:
	//case BREAK_LOOP:
		arg->set_int16(bits.get_bit16(ins_ptr, 10));
		ins_ptr += 10;
		break;
	}
	return ins;
}

Program::FunctionParams Program::setup_function_call(uint8_t fn) {
	Program::FunctionParams ret;
	ret.old_ins_ptr = ins_ptr;
	ins_ptr = bits.get_bit16(fn*HEADER_PER_FUNCTION, HEADER_PER_FUNCTION)*8;
	ins_ptr_function_start = ins_ptr;
	ret.len = bits.get_bit16((fn+1)*HEADER_PER_FUNCTION, HEADER_PER_FUNCTION)*8;
	uint8_t next;
	ret.args = get_number(ins_ptr, &next).as_int32();
	ins_ptr += next;
	ret.vars = get_number(ins_ptr, &next).as_int32();
	ins_ptr += next;
	return ret;
}

void Program::return_function(uint16_t ins_ptr) {
	this->ins_ptr = ins_ptr;
}

Variable Program::get_number(uint16_t pos, uint8_t *next) {
	Variable v;
	if (bits.get_bit8(pos, 1)==0) {
		*next = 4;
		v.set_int32(bits.get_bit8(pos+1, 3));
	} else if (bits.get_bit8(pos+1, 1)==0) {
		*next = 8;
		v.set_int32(bits.get_bit8(pos+2, 6));
	} else {
		switch (bits.get_bit8(pos+2, 2)) {
		case 0:
			*next = 16;
			v.set_int32(bits.get_bit16(pos+4, 12));
			return v;
		case 1:
			*next = 36;
			v.set_int32(bits.get_bit32(pos+4, 32));
			return v;
		case 2:
			*next = 36;
			v.val.int32 = bits.get_bit32(pos+4, 32);
			v.set_float(v.val.float32);
			return v;
		case 3: {
			// String
			uint8_t len = bits.get_bit8(pos+4, 8);
			char str[len];
			for(uint8_t i=0; i<len; i++)
				str[i] = bits.get_bit8(pos+4+8+i*8, 8);
			v = DataType::CreateStr(str, len);
			*next = 4 + 8 + len*8;
			break;
		}
		default:
			assert(false);
		}
	}
	return v;
}

void Program::jump_to_target(uint16_t target) {
	ins_ptr = ins_ptr_function_start + target;
}

#ifdef DESKTOP
//static
Program Program::FromFile(const char *fname) {
	FILE *fp;
    fp = fopen(fname, "r");
    char *buf = (char*)malloc(1024);
    int size = fread(buf, 1, 1024, fp);
    fclose(fp);
    return Program(BitString((uint8_t*)buf, size));
}

void Program::dump() {
	for(int f=0; f<4; f++) {
		auto fn = setup_function_call(f);
		printf("function %d\n", f);
		printf("codelen: %d\n", fn.len-ins_ptr);
		printf("args: %d\n", fn.args);
		printf("vars: %d\n", fn.vars);
		do {
			Variable arg;
			uint8_t ins = get_next_instruction(&arg);
			if(ins_ptr < fn.len)
				printf("%d %s %d\n", ins, get_ins_name(ins), arg.as_int32());
		} while(ins_ptr < fn.len);
	}
}

#elif defined(ARDUINO)
//static
Program Program::FromEEPROM() {
    return Program(BitString());
}
#endif

}
