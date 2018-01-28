#include <RF24.h>

namespace nrf24 {

RF24 radio;

Variable begin(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    Variable ret;
    ret.set_uint12(::digitalRead(arg[0].as_uint8()));
    return ret;
  }
  return Variable::Zero();
}
};
