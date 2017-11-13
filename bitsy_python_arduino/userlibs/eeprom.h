#include <EEPROM.h>

namespace eeprom {

Variable read(uint8_t argcount, Variable arg[]) {
    Variable ret;
    if (argcount==1) {
        ret.set_int16(EEPROM.read(arg[0].as_uint8()));
    }
    return ret;
}

Variable write(uint8_t argcount, Variable arg[]) {
    if (argcount==2) {
        EEPROM.write(arg[0].as_uint8(), arg[1].as_uint8());
    }
    return Variable::Zero();
}

Variable update(uint8_t argcount, Variable arg[]) {
    if (argcount==2) {
        EEPROM.update(arg[0].as_uint8(), arg[1].as_uint8());
    }
    return Variable::Zero();
}

}
