#include "arduino_util.h"
#include "bitsy_python_vm.h"

#define ENABLE_BITSY_USERLIB_ARDUINO
#define ENABLE_BITSY_USERLIB_EEPROM
#define ENABLE_BITSY_USERLIB_SPI
#define ENABLE_BITSY_USERLIB_NRF24
#define ENABLE_BITSY_USERLIB_SERIAL
#define ENABLE_BITSY_USERLIB_ESP8266WIFIUDP

#include "bitsy_python_userlibs.h"
#include "ota_update.def.h"

void setup() {
  Serial.begin(115200);
  bitsy_python::BitsyPythonVM::init();

#ifdef ENABLE_BITSY_USERLIB_ESP8266WIFIUDP
#define ESP8266WIFI_SSID "Ajarirus"
#define ESP8266WIFI_PASSWORD ""
#define ESP8266WIFI_UDP_PORT 6666
  esp8266wifiudp::connect();
#endif

  Serial.println("init");
}

int loopcount = 0;

void loop() {
  if (!bitsy_python::BitsyPythonVM::executeOneStep()) {
    Serial.println("reinit");
    Serial.println(loopcount);
    loopcount++;
    bitsy_python::BitsyPythonVM::init();
    delay(2000);
  }
  ota_update_loop();
}
