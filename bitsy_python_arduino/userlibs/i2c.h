#include <Wire.h>

namespace i2c {

Variable begin(uint8_t argcount, Variable arg[]) {
  if (argcount == 0) {
    Wire.begin();
  }
  return 0;
}

Variable beginTransmission(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    Wire.beginTransmission(arg[0].as_uint8());
  }
  return 0;
}

Variable endTransmission(uint8_t argcount, Variable arg[]) {
  Variable ret;
  if (argcount == 1) {
    ret.set_uint8(Wire.endTransmission());
  } else if (argcount == 1) {
    ret.set_uint8(Wire.endTransmission(arg[0].as_bool()));
  }
  return ret;
}

Variable write(uint8_t argcount, Variable arg[]) {
  Variable ret;
  if (argcount == 1) {
    ret.set_uint8(Wire.write(arg[0].as_uint8()));
    return ret;
  }
  return ret;
}

Variable available(uint8_t argcount, Variable arg[]) {
  Variable ret;
  if (argcount == 0) {
    ret.set_uint8(Wire.available());
    return ret;
  }
  return ret;
}

Variable read(uint8_t argcount, Variable arg[]) {
  Variable ret;
  if (argcount == 0) {
    ret.set_uint8(Wire.read());
    return ret;
  }
  return ret;
}
}
