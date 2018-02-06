#ifndef DESKTOP

bool is_ota_packet(const uint8_t *buf, uint8_t len) {
  if (len!=12)
    return false;
  for(uint8_t i=1; i<len; i++)
    if (buf[i] != 11*i)
      return false;
  return true;
}

bool ota_update(uint8_t end, bool (*sender)(const uint8_t *buf, uint8_t len),
                uint8_t (*receiver)(uint8_t *buf, uint8_t len)) {
  uint16_t ind=0;
  uint8_t pos=0;
#ifdef ESP8266
  EEPROM.begin(4096);
#endif
  delay(100);
  for (auto endtime=millis()+10000; pos < end && millis() < endtime; ) {
    for(uint8_t tries=4; tries-- && !sender(&pos, 1); delay(200));
    for(auto t=millis()+500; millis()<t; ) {
      uint8_t prog[32], len;
      if((len=receiver(prog, sizeof(prog))) && prog[len-1]==pos) {
        for(int i=0; i<len-1; i++) {
#ifdef AVR
          EEPROM.update(ind+i, prog[i]);
#else
          EEPROM.write(ind+i, prog[i]);
#endif
        }
        ind += len-1;
        pos++;
        break;
      }
    }
  }
#ifdef ESP8266
  EEPROM.commit();
  EEPROM.end();
#endif
  delay(1000);
  arduino_util::soft_reset();
  return pos==end;
}

#ifdef ENABLE_BITSY_USERLIB_SERIAL
bool ota_serial_send(const uint8_t *buf, uint8_t len) {
  return true;
}

uint8_t ota_serial_recv(uint8_t *buf, uint8_t len) {
  return Serial.readBytes(buf, len);
}

void ota_update_serial() {
  if (Serial.available()) {
    char buf[16];
    Serial.readBytesUntil('\n', buf, sizeof(buf));
    if (String(buf).indexOf("BEGIN\r") == -1) return;
    Serial.print("\r\nREADY\r\n");
    Serial.readBytesUntil('\n', buf, sizeof(buf));
    if (strncmp("LEN ", buf, 3)) return;
    if (ota_update(String(buf + 4).toInt(), ota_serial_send, ota_serial_recv))
      Serial.print("RESET\r\n");
  }
}
#endif

void ota_update_or_send_to_callback(
                bool (*sender)(const uint8_t *buf, uint8_t len),
                uint8_t (*receiver)(uint8_t *buf, uint8_t len),
                void (*send_to_callback)(const uint8_t *buf, uint8_t len)
              ) {
  uint8_t buf[32], len;
  while((len=receiver(buf, sizeof(buf))) != 0) {
    if (is_ota_packet(buf, len))
      ota_update(buf[0], sender, receiver);
    else
      send_to_callback(buf, len);
  }
}

#ifdef ENABLE_BITSY_USERLIB_NRF24
void ota_update_nrf24() {
  ota_update_or_send_to_callback(nrf24::ota_send, nrf24::ota_recv,
      nrf24::send_to_callback);
}
#endif

#ifdef ENABLE_BITSY_USERLIB_ESP8266WIFIUDP
void ota_update_esp8266wifi() {
  ota_update_or_send_to_callback(esp8266wifiudp::ota_send,
      esp8266wifiudp::ota_recv,
      esp8266wifiudp::send_to_callback);
}
#endif

void ota_update_loop() {
  static unsigned long last_ota_check=0;

  if (millis() > last_ota_check+10) {
#ifdef ENABLE_BITSY_USERLIB_SERIAL
    ota_update_serial();
#endif
#ifdef ENABLE_BITSY_USERLIB_NRF24
    ota_update_nrf24();
#endif
#ifdef ENABLE_BITSY_USERLIB_ESP8266WIFIUDP
    ota_update_esp8266wifi();
#endif
    last_ota_check = millis();
  }
}

#endif
