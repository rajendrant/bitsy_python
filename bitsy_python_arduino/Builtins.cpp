#include "Builtins.h"

#include "datatypes/datatype.h"

namespace bitsy_python {

Variable handle_builtin_call(BitsyBuiltin type, uint8_t argcount,
                             Variable arg[]) {
  Variable v;
  switch (type) {
    case BitsyBuiltin::RANGE:
      v = DataType::CreateForType(Variable::CustomType::RANGE, argcount,
                                  arg);
      break;
    case BitsyBuiltin::LEN:
      BITSY_ASSERT(argcount == 1);
      v.set_int16(DataType::Len(arg[0]));
      break;
    case BitsyBuiltin::BYTEARRAY:
      BITSY_ASSERT(argcount == 1);
      v = DataType::CreateForType(Variable::CustomType::BYTEARRAY, argcount,
                                  arg);
      break;
    default:
      BITSY_ASSERT(false);
  }
  return v;
}
}
