#ifdef ARDUINO
#include <Arduino.h>
#endif

extern void ota_update_loop();

namespace arduino {

Variable digitalRead(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    Variable ret;
    ret.set_uint8(::digitalRead(arg[0].as_uint8()));
    return ret;
  }
  return 0;
}

Variable digitalWrite(uint8_t argcount, Variable arg[]) {
  if (argcount == 2) {
    ::digitalWrite(arg[0].as_uint8(), arg[1].as_uint8());
  }
  return 0;
}

Variable pinMode(uint8_t argcount, Variable arg[]) {
  if (argcount == 2) {
    ::pinMode(arg[0].as_uint8(), arg[1].as_uint8());
  }
  return 0;
}

Variable analogRead(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    Variable ret;
    ret.set_uint8(::analogRead(arg[0].as_uint8()));
    return ret;
  }
  return 0;
}

Variable analogWrite(uint8_t argcount, Variable arg[]) {
  if (argcount == 2) {
    ::analogWrite(arg[0].as_uint8(), arg[1].as_uint8());
  }
  return 0;
}

void delay_local(uint16_t ms)  {
  for(auto end=millis()+ms; end>=millis();) {
    ::delay(10);
    ::ota_update_loop();
    yield();
  }
}

Variable delay(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    arduino::delay_local(arg[0].as_int32());
  }
  return 0;
}

Variable delayMicroseconds(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    ::delayMicroseconds(arg[0].as_int32());
  }
  return 0;
}

Variable millis(uint8_t argcount, Variable arg[]) {
  if (argcount == 0) {
    Variable ret;
    ret.set_int32(::millis());
    return ret;
  }
  return 0;
}

Variable micros(uint8_t argcount, Variable arg[]) {
  if (argcount == 0) {
    Variable ret;
    ret.set_int32(::micros());
    return ret;
  }
  return 0;
}

Variable attachInterrupt(uint8_t argcount, Variable arg[]) {
  if (argcount == 3) {
    // TODO(rajendrant): Add a callback scheme per pin.
    //::attachInterrupt(digitalPinToInterrupt(arg[0].as_uint8()),
    //arg[1].as_uint8(), arg[2].as_uint8());
  }
  return 0;
}

Variable detachInterrupt(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    ::detachInterrupt(digitalPinToInterrupt(arg[0].as_uint8()));
  }
  return 0;
}
};
