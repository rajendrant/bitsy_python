#include "Program.h"

#include "bitsy_python_vm.h"
#include "BitString.h"
#include "datatypes/datatype.h"
#include "instructions.h"
#include "ExecStack.h"

#include <stdio.h>
#include <stdlib.h>

#define MODULE_HEADER 16 /* bits */
#define HEADER_PER_FUNCTION 10 /* bits */

namespace bitsy_python {

namespace Program {

uint8_t curr_fn = 0xFF;
uint16_t ins_ptr_function_start = 0;
uint8_t code_len_bits = 0;

uint8_t get_next_instruction(Variable *arg) {
  uint8_t ins = 0;
  if (BitString::get_next_bit8(1) == 0)
    ins = BitString::get_next_bit8(4);
  else
    ins = BitString::get_next_bit8(7);
  uint8_t global_type = 0;
  if (ins == LOAD_GLOBAL || ins == STORE_GLOBAL || ins == DELETE_GLOBAL) {
    // 3 bit indicates whether its a function, global, builtin, module.
    global_type = BitString::get_next_bit8(3);
  }
  if (is_instr_arg(ins)) {
    *arg = get_number();
    // printf("arg %d %d\n", arg->as_int32(), arg->type);
  }
  if (ins == LOAD_GLOBAL || ins == STORE_GLOBAL || ins == DELETE_GLOBAL) {
    switch (global_type) {
      case 0:
        if(ins==LOAD_GLOBAL)
          *arg = DataType::getGlobalVar(arg->as_uint8());
        break;
      case 1:
        *arg = Variable::FunctionVariable(arg->as_int32());
        break;
      case 2:
        arg->set_CustomType(Variable::CustomType::BUILTIN_FUNCTION,
                            arg->as_uint12());
        break;
      case 3: // Module
        *arg = Variable::CustomTypeVariable(
            Variable::CustomType::USER_MODULE_FUNCTION,
            arg->as_uint8());
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
      arg->set_int32(BitString::get_bit16(BitString::curr_pos, code_len_bits));
      BitString::curr_pos += code_len_bits;
      break;
  }
  return ins;
}

bool sanity_check() {
  uint8_t total_functions = BitString::get_bit8(0, 4);
  uint8_t global_vars = BitString::get_bit8(4, 4);
  uint16_t module_len = BitString::get_bit16(MODULE_HEADER + total_functions * HEADER_PER_FUNCTION, HEADER_PER_FUNCTION) * 8;
  if(BitString::get_bit8(8, 8) != ((~BitString::get_bit8(module_len, 8))&0xFF)) {
    BitString::mark_insane();
    return false;
  }
  if (global_vars)
    ExecStack::push(DataType::initGlobalVars(global_vars));
  return true;
}

uint8_t get_code_len_bits(uint16_t len) {
  uint8_t ret=1;
  while (len >>= 1)
    ++ret;
  return ret;
}

void update_for_function(uint8_t fn) {
  ins_ptr_function_start = BitString::get_bit16(MODULE_HEADER + fn * HEADER_PER_FUNCTION, HEADER_PER_FUNCTION) * 8;
  code_len_bits =
    get_code_len_bits(BitString::get_bit16(MODULE_HEADER + (fn+1) * HEADER_PER_FUNCTION, HEADER_PER_FUNCTION) * 8
                      - ins_ptr_function_start);
}

FunctionParams setup_function_call(uint8_t fn) {
  Program::FunctionParams ret;
  ret.old_ins_ptr = BitString::curr_pos;
  update_for_function(fn);
  BitString::curr_pos = ins_ptr_function_start;
  get_number(); // args
  ret.vars = get_number().as_int32();
  ret.is_callback_mode = 0;
  ret.fn = curr_fn;
  curr_fn = fn;
  return ret;
}

void return_function(const FunctionParams& ret) {
  BitString::curr_pos = ret.old_ins_ptr;
  update_for_function(ret.fn);
  curr_fn = ret.fn;
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

void jump_to_target(const Variable &target) {
  BitString::curr_pos = ins_ptr_function_start + target.as_uint12();
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
