#include "arduino_util.h"
#include "bitsy_python_vm.h"

#define ENABLE_BITSY_USERLIB_ARDUINO
#define ENABLE_BITSY_USERLIB_EEPROM
#define ENABLE_BITSY_USERLIB_SPI
#define ENABLE_BITSY_USERLIB_NRF24
#define ENABLE_BITSY_USERLIB_SERIAL

#include "bitsy_python_userlibs.h"
#include "ota_update.def.h"

bitsy_python::BitsyPythonVM vm;

void setup() {
  Serial.begin(115200);
  bitsy_python::BitsyPythonVM::init();
  bitsy_python::BitsyPythonVM::initExecution();
  Serial.println("init");
}

int loopcount = 0;

void loop() {
  if (!vm.executeOneStep()) {
    Serial.println("reinit");
    Serial.println(loopcount);
    loopcount++;
    bitsy_python::BitsyPythonVM::initExecution();
  }
  ota_update_loop();
}
