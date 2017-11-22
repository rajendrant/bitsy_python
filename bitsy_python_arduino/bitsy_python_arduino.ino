#include "arduino_util.h"
#include "bitsy_python_vm.h"

#define ENABLE_BITSY_USERLIB_ARDUINO
#define ENABLE_BITSY_USERLIB_EEPROM
#define ENABLE_BITSY_USERLIB_SPI
//#define ENABLE_BITSY_USERLIB_NRF24
#define ENABLE_BITSY_USERLIB_SERIAL

#include "bitsy_python_userlibs.h"

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
  if (Serial.available()) {
    char buf[16];
    Serial.readBytesUntil('\n', buf, sizeof(buf));
    if (String(buf).indexOf("BEGIN\r") == -1) return;
    Serial.print("\r\nREADY\r\n");

    Serial.readBytesUntil('\n', buf, sizeof(buf));
    if (strncmp("LEN ", buf, 3)) return;
    int len = String(buf + 4).toInt();
    int addr = 0;
    while (len > 0) {
      if (Serial.readBytes(buf, 1) != 1) return;
      EEPROM.update(addr, buf[0]);
      len--;
      addr++;
    }
    Serial.readBytesUntil('\n', buf, sizeof(buf));
    if (String(buf).indexOf("END\r") == -1) return;
    Serial.print("\r\nRESET\r\n");
    delay(1000);
    arduino_util::soft_reset();
  }
  if (!vm.executeOneStep()) {
    Serial.println("reinit");
    Serial.println(loopcount);
    loopcount++;
    vm.initExecution();
  }
}
