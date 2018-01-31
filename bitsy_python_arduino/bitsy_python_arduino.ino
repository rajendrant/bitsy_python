#include "arduino_util.h"
#include "bitsy_python_vm.h"

#define ENABLE_BITSY_USERLIB_ARDUINO
#define ENABLE_BITSY_USERLIB_EEPROM
#define ENABLE_BITSY_USERLIB_SPI
#define ENABLE_BITSY_USERLIB_NRF24
#define ENABLE_BITSY_USERLIB_SERIAL

#include "bitsy_python_userlibs.h"
#include "ota_update.def.h"

void setup() {
  Serial.begin(115200);
#ifdef ENABLE_BITSY_USERLIB_NRF24
  nrf24::radio.init(1, 8, 10);
#endif
  bitsy_python::BitsyPythonVM::init();
  Serial.println("init");
}

int loopcount = 0;

void loop() {
  if (!bitsy_python::BitsyPythonVM::executeOneStep()) {
    Serial.println("reinit");
    Serial.println(loopcount);
    loopcount++;
    //bitsy_python::BitsyPythonVM::init();
  }
  ota_update_loop();
  //delay(50);
}
