#ifndef BUILTINS_H__
#define BUILTINS_H__

#include "variable.h"

namespace bitsy_python {

enum BitsyBuiltin {
  // This should be in sync with builtins in main.py
  RANGE,
  TYPE,
  ITER,
  GETATTR,
  BYTEARRAY,
  LEN,
};

Variable handle_builtin_call(BitsyBuiltin type, uint8_t argcount,
                             Variable arg[]);
}

#endif /* BUILTINS_H__ */
