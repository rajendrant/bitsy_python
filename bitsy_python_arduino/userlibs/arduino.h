#ifdef ARDUINO
#include <Arduino.h>
#endif

extern void ota_update_loop();

namespace arduino {

Variable digitalRead(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    Variable ret;
    ret.set_uint12(::digitalRead(arg[0].as_uint8()));
    return ret;
  }
  return Variable::Zero();
}

Variable digitalWrite(uint8_t argcount, Variable arg[]) {
  if (argcount == 2) {
    ::digitalWrite(arg[0].as_uint8(), arg[1].as_uint8());
  }
  return Variable::Zero();
}

Variable pinMode(uint8_t argcount, Variable arg[]) {
  if (argcount == 2) {
    ::pinMode(arg[0].as_uint8(), arg[0].as_uint8());
  }
  return Variable::Zero();
}

Variable analogRead(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    Variable ret;
    ret.set_uint12(::analogRead(arg[0].as_uint8()));
    return ret;
  }
  return Variable::Zero();
}

Variable analogWrite(uint8_t argcount, Variable arg[]) {
  if (argcount == 2) {
    ::analogWrite(arg[0].as_uint8(), arg[1].as_uint8());
  }
  return Variable::Zero();
}

void delay(uint16_t ms)  {
  for(auto end=millis()+ms; end>=millis();) {
    ::delay(5);
    ::ota_update_loop();
    yield();
  }
}

Variable delay(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    arduino::delay(arg[0].as_int32());
  }
  return Variable::Zero();
}

Variable delayMicroseconds(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    ::delayMicroseconds(arg[0].as_int32());
  }
  return Variable::Zero();
}

Variable millis(uint8_t argcount, Variable arg[]) {
  if (argcount == 0) {
    Variable ret;
    ret.set_int32(::millis());
    return ret;
  }
  return Variable::Zero();
}

Variable micros(uint8_t argcount, Variable arg[]) {
  if (argcount == 0) {
    Variable ret;
    ret.set_int32(::micros());
    return ret;
  }
  return Variable::Zero();
}

Variable attachInterrupt(uint8_t argcount, Variable arg[]) {
  if (argcount == 3) {
    // TODO(rajendrant): Add a callback scheme per pin.
    //::attachInterrupt(digitalPinToInterrupt(arg[0].as_uint8()),
    //arg[1].as_uint8(), arg[2].as_uint8());
  }
  return Variable::Zero();
}

Variable detachInterrupt(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    ::detachInterrupt(digitalPinToInterrupt(arg[0].as_uint8()));
  }
  return Variable::Zero();
}
};
