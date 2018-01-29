// DO NOT EDIT THIS FILE
// This file is autogenerated from userlibsgen.py

#include "../../bitsylimit.h"

#ifdef ENABLE_BITSY_USERLIB_ARDUINO
#include "../arduino.h"

Variable userlib_module_arduino(uint8_t function, uint8_t argcount, Variable arg[]) {
  switch (function) {
    case 0:
      return arduino::digitalRead(argcount, arg);
    case 1:
      return arduino::digitalWrite(argcount, arg);
    case 2:
      return arduino::pinMode(argcount, arg);
    case 3:
      return arduino::analogRead(argcount, arg);
    case 4:
      return arduino::analogWrite(argcount, arg);
    case 5:
      return arduino::delay(argcount, arg);
    case 6:
      return arduino::delayMicroseconds(argcount, arg);
    case 7:
      return arduino::millis(argcount, arg);
    case 8:
      return arduino::micros(argcount, arg);
    case 9:
      return arduino::attachInterrupt(argcount, arg);
    case 10:
      return arduino::detachInterrupt(argcount, arg);
    default:
      BITSY_ASSERT(false);
  }
  return Variable::Zero();
}
#endif

#ifdef ENABLE_BITSY_USERLIB_SERIAL
#include "../serial.h"

Variable userlib_module_serial(uint8_t function, uint8_t argcount, Variable arg[]) {
  switch (function) {
    case 0:
      return serial::printstr(argcount, arg);
    case 1:
      return serial::println(argcount, arg);
    default:
      BITSY_ASSERT(false);
  }
  return Variable::Zero();
}
#endif

#ifdef ENABLE_BITSY_USERLIB_SPI
#include "../spi.h"

Variable userlib_module_spi(uint8_t function, uint8_t argcount, Variable arg[]) {
  switch (function) {
    case 0:
      return spi::begin(argcount, arg);
    case 1:
      return spi::end(argcount, arg);
    case 2:
      return spi::beginTransaction(argcount, arg);
    case 3:
      return spi::endTransaction(argcount, arg);
    case 4:
      return spi::transfer(argcount, arg);
    default:
      BITSY_ASSERT(false);
  }
  return Variable::Zero();
}
#endif

#ifdef ENABLE_BITSY_USERLIB_I2C
#include "../i2c.h"

Variable userlib_module_i2c(uint8_t function, uint8_t argcount, Variable arg[]) {
  switch (function) {
    case 0:
      return i2c::begin(argcount, arg);
    case 1:
      return i2c::beginTransmission(argcount, arg);
    case 2:
      return i2c::endTransmission(argcount, arg);
    case 3:
      return i2c::write(argcount, arg);
    case 4:
      return i2c::available(argcount, arg);
    case 5:
      return i2c::read(argcount, arg);
    default:
      BITSY_ASSERT(false);
  }
  return Variable::Zero();
}
#endif

#ifdef ENABLE_BITSY_USERLIB_SERVO
#include "../servo.h"

Variable userlib_module_servo(uint8_t function, uint8_t argcount, Variable arg[]) {
  switch (function) {
    case 0:
      return servo::attach(argcount, arg);
    case 1:
      return servo::detach(argcount, arg);
    case 2:
      return servo::read(argcount, arg);
    case 3:
      return servo::write(argcount, arg);
    default:
      BITSY_ASSERT(false);
  }
  return Variable::Zero();
}
#endif

#ifdef ENABLE_BITSY_USERLIB_NRF24
#include "../nrf24.h"

Variable userlib_module_nrf24(uint8_t function, uint8_t argcount, Variable arg[]) {
  switch (function) {
    case 0:
      return nrf24::init(argcount, arg);
    case 1:
      return nrf24::send(argcount, arg);
    default:
      BITSY_ASSERT(false);
  }
  return Variable::Zero();
}
#endif

#ifdef ENABLE_BITSY_USERLIB_EEPROM
#include "../eeprom.h"

Variable userlib_module_eeprom(uint8_t function, uint8_t argcount, Variable arg[]) {
  switch (function) {
    case 0:
      return eeprom::read(argcount, arg);
    case 1:
      return eeprom::write(argcount, arg);
    case 2:
      return eeprom::update(argcount, arg);
    default:
      BITSY_ASSERT(false);
  }
  return Variable::Zero();
}
#endif

#ifdef ENABLE_BITSY_USERLIB_TESTUSERLIB
#include "../testuserlib.h"

Variable userlib_module_testuserlib(uint8_t function, uint8_t argcount, Variable arg[]) {
  switch (function) {
    case 0:
      return testuserlib::printstr(argcount, arg);
    case 1:
      return testuserlib::println(argcount, arg);
    case 2:
      return testuserlib::math_add(argcount, arg);
    case 3:
      return testuserlib::math_power(argcount, arg);
    case 4:
      return testuserlib::init_callback(argcount, arg);
    case 5:
      return testuserlib::trigger_callback(argcount, arg);
    default:
      BITSY_ASSERT(false);
  }
  return Variable::Zero();
}
#endif

bool is_userlib_module_enabled(uint8_t module) {
  switch (module) {
#ifdef ENABLE_BITSY_USERLIB_ARDUINO
    case 0: // arduino
#endif
#ifdef ENABLE_BITSY_USERLIB_SERIAL
    case 1: // serial
#endif
#ifdef ENABLE_BITSY_USERLIB_SPI
    case 2: // spi
#endif
#ifdef ENABLE_BITSY_USERLIB_I2C
    case 3: // i2c
#endif
#ifdef ENABLE_BITSY_USERLIB_SERVO
    case 4: // servo
#endif
#ifdef ENABLE_BITSY_USERLIB_NRF24
    case 5: // nrf24
#endif
#ifdef ENABLE_BITSY_USERLIB_EEPROM
    case 6: // eeprom
#endif
#ifdef ENABLE_BITSY_USERLIB_TESTUSERLIB
    case 7: // testuserlib
#endif
      return true;
  }
  return false;
}

Variable call_userlib_function(uint8_t module, uint8_t function, uint8_t argcount, Variable arg[]) {
    switch(module) {
#ifdef ENABLE_BITSY_USERLIB_ARDUINO
    case 0:
      return userlib_module_arduino(function, argcount, arg);
#endif
#ifdef ENABLE_BITSY_USERLIB_SERIAL
    case 1:
      return userlib_module_serial(function, argcount, arg);
#endif
#ifdef ENABLE_BITSY_USERLIB_SPI
    case 2:
      return userlib_module_spi(function, argcount, arg);
#endif
#ifdef ENABLE_BITSY_USERLIB_I2C
    case 3:
      return userlib_module_i2c(function, argcount, arg);
#endif
#ifdef ENABLE_BITSY_USERLIB_SERVO
    case 4:
      return userlib_module_servo(function, argcount, arg);
#endif
#ifdef ENABLE_BITSY_USERLIB_NRF24
    case 5:
      return userlib_module_nrf24(function, argcount, arg);
#endif
#ifdef ENABLE_BITSY_USERLIB_EEPROM
    case 6:
      return userlib_module_eeprom(function, argcount, arg);
#endif
#ifdef ENABLE_BITSY_USERLIB_TESTUSERLIB
    case 7:
      return userlib_module_testuserlib(function, argcount, arg);
#endif
    default:
      BITSY_ASSERT(false);
  }
  return Variable::Zero();
}
