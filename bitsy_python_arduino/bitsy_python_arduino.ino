//#include <NRF24_low_latency.h>
#include "bitsy_python_vm.h"

bitsy_python::BitsyPythonVM vm;
//NRF24 nrf24;

void setup() {
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

  Serial.println("initialised");
}

void loop() {
    vm.executeOneStep();
}

