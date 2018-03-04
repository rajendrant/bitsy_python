#include <SPI.h>

namespace spi {

Variable begin(uint8_t argcount, Variable arg[]) {
  if (argcount == 0) {
    SPI.begin();
  }
  return 0;
}

Variable end(uint8_t argcount, Variable arg[]) {
  if (argcount == 0) {
    SPI.end();
  }
  return 0;
}

Variable beginTransaction(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    // TODO(rajendrant): Pass down settings as arguments.
    // SPI.beginTransaction();
  }
  return 0;
}

Variable endTransaction(uint8_t argcount, Variable arg[]) {
  if (argcount == 0) {
    SPI.endTransaction();
  }
  return 0;
}

Variable transfer(uint8_t argcount, Variable arg[]) {
  Variable ret;
  if (argcount == 1) {
    ret.set_uint8(SPI.transfer(arg[0].as_uint8()));
  } else if (argcount == 2) {
    // TODO(rajendrant): Transfer an array of bytes.
    // ret.set_uint8(SPI.transfer(arg[0].as_uint8()));
  }
  return ret;
}
};
