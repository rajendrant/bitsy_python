#include <EEPROM.h>

namespace eeprom {

Variable read(uint8_t argcount, Variable arg[]) {
  Variable ret;
  if (argcount == 1) {
    ret.set_uint12(EEPROM.read(arg[0].as_uint8()));
  }
  return ret;
}

Variable write(uint8_t argcount, Variable arg[]) {
  if (argcount == 2) {
#ifdef AVR
    EEPROM.update(arg[0].as_uint8(), arg[1].as_uint8());
#else
    EEPROM.write(arg[0].as_uint8(), arg[1].as_uint8());
#endif
  }
  return Variable::Zero();
}
}
