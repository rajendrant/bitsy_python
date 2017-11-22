// This file should be included after #define of the userlibs that need to be
// enabled and included in the vm.

#include "datatypes/datatype.h"
#include "variable.h"

// The userlib functions are auto-generated and defined in the following file.
#include "userlibs/gen/userlibs_defines.h"

extern void bitsy_print(char ch);

void BITSY_PYTHON_PRINT(const char* str) {
#if defined(DESKTOP) || defined(ENABLE_BITSY_USERLIB_SERIAL)
  while (*str != '\0') {
    bitsy_print(*str);
    ++str;
  }
#endif
}

void BITSY_PYTHON_PRINT_VAR(const Variable& v) {
  if (v.type == Variable::FLOAT) {
#if defined(DESKTOP)
    printf("%g", v.as_float());
#elif defined(ENABLE_BITSY_USERLIB_SERIAL)
    Serial.print(v.as_float());
#endif
  } else if (v.type == Variable::CUSTOM &&
             (v.val.custom_type.type == Variable::CustomType::CHARACTER ||
              v.val.custom_type.type == Variable::CustomType::STRING)) {
    bitsy_python::DataType::Print(v, bitsy_print);
  } else {
#if defined(DESKTOP)
    printf("%d", v.as_int32());
#elif defined(ENABLE_BITSY_USERLIB_SERIAL)
    Serial.print(v.as_int32());
#endif
  }
}
