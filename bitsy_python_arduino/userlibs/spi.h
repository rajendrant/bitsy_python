#include <SPI.h>

namespace spi {

Variable begin(uint8_t argcount, Variable arg[]) {
  if (argcount == 0) {
    SPI.begin();
  }
  return Variable::Zero();
}

Variable end(uint8_t argcount, Variable arg[]) {
  if (argcount == 0) {
    SPI.end();
  }
  return Variable::Zero();
}

Variable beginTransaction(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    // TODO(rajendrant): Pass down settings as arguments.
    // SPI.beginTransaction();
  }
  return Variable::Zero();
}

Variable endTransaction(uint8_t argcount, Variable arg[]) {
  if (argcount == 0) {
    SPI.endTransaction();
  }
  return Variable::Zero();
}

Variable transfer(uint8_t argcount, Variable arg[]) {
  Variable ret;
  if (argcount == 1) {
    ret.set_uint12(SPI.transfer(arg[0].as_uint8()));
  } else if (argcount == 2) {
    // TODO(rajendrant): Transfer an array of bytes.
    // ret.set_uint12(SPI.transfer(arg[0].as_uint8()));
  }
  return ret;
}
};
