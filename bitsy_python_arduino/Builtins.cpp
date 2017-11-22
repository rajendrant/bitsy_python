#include "Builtins.h"

#include <cassert>

#include "datatypes/datatype.h"

namespace bitsy_python {

Variable handle_builtin_call(BitsyBuiltin type, uint8_t argcount, Variable arg[]) {
    Variable v;
    switch(type) {
    case BitsyBuiltin::LEN:
        assert(argcount==1);
        v.set_int16(DataType::Len(arg[0]));
        break;
    case BitsyBuiltin::BYTEARRAY: {
        assert(argcount==1);
        v = DataType::CreateForType(Variable::CustomType::BYTEARRAY, argcount, arg);
        break;
    }
    default:
        assert(false);
    }
    return v;
}

}
