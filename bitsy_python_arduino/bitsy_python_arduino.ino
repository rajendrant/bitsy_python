#include "arduino_util.h"
#include "bitsy_python_vm.h"

#define ENABLE_BITSY_USERLIB_ARDUINO
#define ENABLE_BITSY_USERLIB_EEPROM
#define ENABLE_BITSY_USERLIB_SPI
#define ENABLE_BITSY_USERLIB_NRF24
#define ENABLE_BITSY_USERLIB_SERIAL
#define ENABLE_BITSY_USERLIB_ESP8266WIFIUDP

#ifdef ENABLE_BITSY_USERLIB_ESP8266WIFIUDP
#define ESP8266WIFI_SSID "Ajarirus"
#define ESP8266WIFI_PASSWORD "SeattleBallard"
#define ESP8266WIFI_UDP_PORT 6666
#endif

#include "bitsy_python_userlibs.h"
#include "ota_update.def.h"

void setup() {
  Serial.begin(115200);
  bitsy_python::BitsyPythonVM::init();

#ifdef ENABLE_BITSY_USERLIB_NRF24
#ifdef ESP8266
  nrf24::radio.init(0, 4, 5);
#else // Arduino nano/pro_mini with atmega328
  nrf24::radio.init(0, 8, 10);
#endif
#endif

#ifdef ENABLE_BITSY_USERLIB_ESP8266WIFIUDP
  esp8266wifiudp::connect();
#endif

  Serial.println("INIT");
}

void loop() {
  for (uint8_t i=2; i--; ) {
    if (!bitsy_python::BitsyPythonVM::executeOneStep()) {
      Serial.println("REINIT");
      bitsy_python::BitsyPythonVM::init();
      delay(1000);
    }
  }
  ota_update_loop();
}
