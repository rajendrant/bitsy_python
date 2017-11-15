// This file should be included after #define of the userlibs that need to be
// enabled and included in the vm.

#include "variable.h"

// The userlib functions are auto-generated and defined in the following file.
#include "userlibs/gen/userlibs_defines.h"


#if defined(DESKTOP)
void BITSY_PYTHON_PRINT(const char* str) {
    printf("%s", str);
}

void BITSY_PYTHON_PRINT_VAR(const Variable &v) {
    if (v.type==Variable::FLOAT)
        printf("%g", v.as_float());
    else
        printf("%d", v.as_int32());
}

#elif defined(ENABLE_BITSY_USERLIB_SERIAL)
void BITSY_PYTHON_PRINT(const char* str) {
    Serial.print(str);
}

void BITSY_PYTHON_PRINT_VAR(const Variable &v) {
    if (v.type==Variable::FLOAT)
        Serial.print(v.as_float());
    else
        Serial.print(v.as_int32());
}

#else
void BITSY_PYTHON_PRINT(const char* str) {
}

void BITSY_PYTHON_PRINT_VAR(const Variable &v) {
}
#endif
