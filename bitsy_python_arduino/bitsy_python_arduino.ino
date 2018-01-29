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
  vm.initExecution();

  /*
  if (!nrf24.init())
    Serial.println("NRF24 init failed");
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setThisAddress((uint8_t*)"n-c", 3))
    Serial.println("setThisAddress failed");
  if (!nrf24.setTransmitAddress((uint8_t*)"n-s", 3))
    Serial.println("setTransmitAddress failed");
  if (!nrf24.setRF(NRF24::NRF24DataRate2Mbps, NRF24::NRF24TransmitPowerm18dBm))
    Serial.println("setRF failed");
    */
  Serial.println("init");
}

int loopcount = 0;

void loop() {
  if (!vm.executeOneStep()) {
    Serial.println("reinit");
    Serial.println(loopcount);
    loopcount++;
    vm.initExecution();
  }
  ota_update_loop();
}
