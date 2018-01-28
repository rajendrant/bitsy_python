#include <stdint.h>
#include "bitsylimit.h"

namespace bitsy_python {

enum instruction {
  LOAD_CONST,
  LOAD_FAST,
  STORE_FAST,
  LOAD_GLOBAL,

  PRINT_ITEM,
  PRINT_NEWLINE,

  CALL_FUNCTION,
  RETURN_VALUE,
  POP_TOP,

  STORE_GLOBAL,

  UNARY_NOT,
  UNARY_INVERT,
  BINARY_POWER,
  BINARY_MULTIPLY,
  BINARY_FLOOR_DIVIDE,
  BINARY_TRUE_DIVIDE,
  BINARY_MODULO,
  BINARY_ADD,
  BINARY_SUBTRACT,
  BINARY_LSHIFT,
  BINARY_RSHIFT,
  BINARY_AND,
  BINARY_XOR,
  BINARY_OR,

  COMPARE_OP,
  POP_JUMP_IF_TRUE,
  POP_JUMP_IF_FALSE,
  JUMP_IF_TRUE_OR_POP,
  JUMP_IF_FALSE_OR_POP,
  JUMP_ABSOLUTE,

  GET_ITER,
  FOR_ITER,
  // BREAK_LOOP, CONTINUE_LOOP,
  // SETUP_LOOP, POP_BLOCK,

  LOAD_ATTR,

  INPLACE_POWER,
  INPLACE_MULTIPLY,
  INPLACE_FLOOR_DIVIDE,
  INPLACE_TRUE_DIVIDE,
  INPLACE_MODULO,
  INPLACE_ADD,
  INPLACE_SUBTRACT,
  INPLACE_LSHIFT,
  INPLACE_RSHIFT,
  INPLACE_AND,
  INPLACE_XOR,
  INPLACE_OR,

  DELETE_FAST,
  DELETE_GLOBAL,

  UNARY_POSITIVE,
  UNARY_NEGATIVE,

  DUP_TOP,
  ROT_TWO,
  ROT_THREE,
  ROT_FOUR,

  NOP,
  STOP_CODE,

  BINARY_SUBSCR,
  STORE_SUBSCR,
  UNPACK_SEQUENCE,
};

bool is_instr_arg(uint8_t ins);

#ifdef DESKTOP
const char* get_ins_name(uint8_t ins);
#endif
}
