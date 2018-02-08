#include "Program.h"

#include "bitsy_python_vm.h"
#include "BitString.h"
#include "datatypes/datatype.h"
#include "instructions.h"

#include <stdio.h>
#include <stdlib.h>

#define MODULE_HEADER 16 /* bits */
#define HEADER_PER_FUNCTION 10 /* bits */

namespace bitsy_python {

namespace Program {

uint16_t ins_ptr_function_start = 0;

uint8_t get_next_instruction(Variable *arg) {
  uint8_t ins = 0;
  if (BitString::get_next_bit8(1) == 0)
    ins = BitString::get_next_bit8(4);
  else
    ins = BitString::get_next_bit8(7);
  uint8_t global_type = 0;
  if (ins == LOAD_GLOBAL || ins == STORE_GLOBAL || ins == DELETE_GLOBAL) {
    // 2 bit indicates whether its a function, global, builtin, module.
    global_type = BitString::get_next_bit8(3);
  }
  if (is_instr_arg(ins)) {
    *arg = get_number();
    // printf("arg %d\n", arg->as_int32());
  }
  if (ins == LOAD_GLOBAL || ins == STORE_GLOBAL || ins == DELETE_GLOBAL) {
    switch (global_type) {
      case 0:
        // TODO: global variable.
        arg->set_uint8(0);
        //BITSY_ASSERT(false);
      case 1:
        *arg = Variable::FunctionVariable(arg->as_int32());
        break;
      case 2:
        arg->set_CustomType(Variable::CustomType::BUILTIN_FUNCTION,
                            arg->as_uint12());
        break;
      case 3:
        *arg = Variable::ModuleVariable(arg->as_uint12());
        break;
      case 4:
        // None or True or False
        break;
      default:
        BITSY_ASSERT(false);
    }
  }
  switch (ins) {
    case POP_JUMP_IF_TRUE:
    case POP_JUMP_IF_FALSE:
    case JUMP_IF_TRUE_OR_POP:
    case JUMP_IF_FALSE_OR_POP:
    case JUMP_ABSOLUTE:
    // case CONTINUE_LOOP:
    // case SETUP_LOOP:
    case FOR_ITER:
    // case BREAK_LOOP:
      arg->set_uint12(BitString::get_bit16(BitString::curr_pos, 10));
      BitString::curr_pos += 10;
      break;
  }
  return ins;
}

bool sanity_check() {
  uint8_t total_functions = BitString::get_bit8(0, 8);
  uint16_t module_len = BitString::get_bit16(MODULE_HEADER + total_functions * HEADER_PER_FUNCTION, HEADER_PER_FUNCTION) * 8;
  if(BitString::get_bit8(8, 8) != BitString::get_bit8(module_len, 8)) {
    BitString::curr_pos = 0;
    return false;
  }
  return true;
}

bool is_sane() { return BitString::curr_pos; }

FunctionParams setup_function_call(uint8_t fn) {
  Program::FunctionParams ret;
  ret.old_ins_ptr = BitString::curr_pos;
  ret.old_start_ins_ptr = ins_ptr_function_start;
  BitString::curr_pos = ins_ptr_function_start = BitString::get_bit16(MODULE_HEADER + fn * HEADER_PER_FUNCTION, HEADER_PER_FUNCTION) * 8;
  get_number(); // args
  ret.vars = get_number().as_int32();
  ret.is_callback_mode = 0;
  return ret;
}

void return_function(const FunctionParams& ret) {
  BitString::curr_pos = ret.old_ins_ptr;
  ins_ptr_function_start = ret.old_start_ins_ptr;
}

Variable get_number() {
  Variable v;
  if (BitString::get_next_bit8(1) == 0) {
    v.set_uint8(BitString::get_next_bit8(3));
  } else if (BitString::get_next_bit8(1) == 0) {
    v.set_uint8(BitString::get_next_bit8(6));
  } else {
    switch (BitString::get_next_bit8(2)) {
      case 0:
        v.set_int32(BitString::get_bit16(BitString::curr_pos, 12));
        BitString::curr_pos += 12;
        return v;
      case 1:
        //v.set_int32(BitString::get_next_bit32(BitString::curr_pos, 32));
        v.set_int32(BitString::get_next_bit32());
        //BitString::curr_pos += 32;
        return v;
      case 2:
        //v.val.int32 = BitString::get_next_bit32(BitString::curr_pos, 32);
        v.val.int32 = BitString::get_next_bit32();
        v.set_float(v.val.float32);
        //BitString::curr_pos += 32;
        return v;
      case 3: {
        // String
        uint8_t len = BitString::get_next_bit8(8);
        char str[len];
        for (uint8_t i = 0; i < len; i++)
          str[i] = BitString::get_next_bit8(8);
        v = DataType::CreateStr(str, len);
        break;
      }
      default:
        BITSY_ASSERT(false);
    }
  }
  return v;
}

void jump_to_target(uint16_t target) {
  BitString::curr_pos = ins_ptr_function_start + target;
}

#ifdef DESKTOP
void FromFile(const char *fname) {
  FILE *fp;
  fp = fopen(fname, "r");
  char *buf = (char *)malloc(1024);
  int size = fread(buf, 1, 1024, fp);
  fclose(fp);
  BitString::init((uint8_t *)buf, size);
}

#elif defined(ARDUINO)
void FromEEPROM() {
  ins_ptr_function_start = 0;
  BitString::init();
}
#endif
}
}
