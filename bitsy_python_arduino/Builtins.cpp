#include "Builtins.h"

#include <assert.h>

#include "datatypes/datatype.h"

namespace bitsy_python {

Variable handle_builtin_call(BitsyHeap &heap, BitsyBuiltin type, uint8_t argcount,
                             Variable arg[]) {
  Variable v;
  switch (type) {
    case BitsyBuiltin::RANGE:
      v = DataType::CreateForType(heap, Variable::CustomType::RANGE, argcount,
                                  arg);
      break;
    case BitsyBuiltin::LEN:
      assert(argcount == 1);
      v.set_int16(DataType::Len(heap, arg[0]));
      break;
    case BitsyBuiltin::BYTEARRAY:
      assert(argcount == 1);
      v = DataType::CreateForType(heap, Variable::CustomType::BYTEARRAY, argcount,
                                  arg);
      break;
    default:
      assert(false);
  }
  return v;
}
}
