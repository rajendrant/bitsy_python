#ifndef DESKTOP
#include <EEPROM.h>

bool ota_update(uint8_t *buf, uint8_t len,
                bool (*sender)(uint8_t *buf, uint8_t len),
                uint8_t (*receiver)(uint8_t *buf, uint8_t len)) {
  if (len!=12)
    return false;
  for(uint8_t i=0; i<len; i++)
    if (buf[i] != 11*(i+1))
      return false;
  // Start OTA update.
  int start=millis();
  uint16_t pos=0;
  uint8_t prog[32];
  do {
    if (!sender((uint8_t*)&pos, 2)) return false;
    for(int t=millis(); millis()<t+30;) {
      if(len=receiver(prog, sizeof(prog)) && ((uint16_t*)prog)[0]==pos) {
        for(int i=0; i<len-2; i++)
          EEPROM.update(pos+i+20, prog[i+2]);
        pos += len-2;
      }
    }
  } while(millis() < start+10000);
  return true;
}

void ota_update_serial() {
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

}

void ota_update_nrf24() {
  uint8_t buf[32], len;
  while(len=nrf24::ota_nrf24_recv(buf, sizeof(buf))) {
    if (!ota_update(buf, len, nrf24::ota_nrf24_send, nrf24::ota_nrf24_recv))
      nrf24::send_to_callback(buf, len);
  }
}

void ota_update_loop() {
  static int last_ota_check=0;
  if (millis() > last_ota_check+100) {
    last_ota_check = millis();
#ifdef ENABLE_BITSY_USERLIB_SERIAL
    ota_update_serial();
#endif
#ifdef ENABLE_BITSY_USERLIB_NRF24
    ota_update_nrf24();
#endif
  }
}

#endif
