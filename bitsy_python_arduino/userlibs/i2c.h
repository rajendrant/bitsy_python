#include <Wire.h>

namespace i2c {

Variable begin(uint8_t argcount, Variable arg[]) {
  if (argcount == 0) {
    Wire.begin();
  }
  return Variable::Zero();
}

Variable beginTransmission(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    Wire.beginTransmission(arg[0].as_uint8());
  }
  return Variable::Zero();
}

Variable endTransmission(uint8_t argcount, Variable arg[]) {
  Variable ret;
  if (argcount == 1) {
    ret.set_int16(Wire.endTransmission());
  } else if (argcount == 1) {
    ret.set_int16(Wire.endTransmission(arg[0].as_bool()));
  }
  return ret;
}

Variable write(uint8_t argcount, Variable arg[]) {
  Variable ret;
  if (argcount == 1) {
    ret.set_int16(Wire.write(arg[0].as_uint8()));
    return ret;
  }
  return ret;
}

Variable available(uint8_t argcount, Variable arg[]) {
  Variable ret;
  if (argcount == 0) {
    ret.set_int16(Wire.available());
    return ret;
  }
  return ret;
}

Variable read(uint8_t argcount, Variable arg[]) {
  Variable ret;
  if (argcount == 0) {
    ret.set_int16(Wire.read());
    return ret;
  }
  return ret;
}
}
