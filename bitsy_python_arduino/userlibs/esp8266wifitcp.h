#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

namespace esp8266wifitcp {

WiFiServer wifiServer(6666);
WiFiClient wifiClient;

char ssid[32], password[32];
uint16_t on_recv_callback = 0xFF;
uint32_t last_check = 0;

void init() {
  on_recv_callback = 0xFF;
  last_check = 0;
}

void do_connect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  wifiServer.begin();
}

void connect(const char *_ssid, const char *_password, uint16_t _port) {
  strncpy(ssid, _ssid, sizeof(ssid));
  strncpy(password, _password, sizeof(password));
  ssid[sizeof(ssid)-1] = password[sizeof(password)-1] = 0;
  do_connect();
}

void checkConnectivity() {
  if (WiFi.status() != WL_CONNECTED) {
    // If disconnected, attempt to connect every minute.
    if (millis() - last_check > 60000) {
      do_connect();
      last_check = millis();
    }
  }
}

bool ota_send(const uint8_t *buf, uint8_t len) {
  if (wifiClient.connected()) {
    wifiClient.write(len);
    for(uint8_t i=0; i<len; i++)
      wifiClient.write(buf[i]);
  }
  return wifiClient.connected();
}

uint8_t ota_recv(uint8_t *buf, uint8_t len) {
  checkConnectivity();
  if (!wifiClient.connected()) {
    wifiClient = wifiServer.available();
  }
  if (wifiClient.connected()) {
    auto l=wifiClient.read();
    if(l<=0 || l>len)
      return 0;
    len = l;
    uint8_t i=0;
    for(auto end = millis()+100; i<len && millis()<=end;) {
      auto byte=wifiClient.read();
      if(byte!=-1)
        buf[i++] = byte;
    }
    if(i==len)
      return len;
  }
  return 0;
}

Variable set_on_recv_callback(uint8_t argcount, Variable arg[]) {
  if (argcount == 1 && arg[0].type==Variable::CUSTOM &&
      arg[0].val.custom_type.type==Variable::CustomType::USER_FUNCTION) {
    on_recv_callback = arg[0].val.custom_type.val;
  }
  return 0;
}

void send_to_callback(const uint8_t *buf, uint8_t len) {
  if (on_recv_callback != 0xFF) {
    uint8_t *vbuf;
    auto v = Variable::CustomTypeVariable(Variable::CustomType::BYTEARRAY,
                     bitsy_python::BitsyHeap::CreateVar(len, &vbuf));
    memcpy(vbuf, buf, len);
    bitsy_python::BitsyPythonVM::callUserFunction(on_recv_callback, v);
  }
}

Variable send_response(uint8_t argcount, Variable arg[]) {
  if (argcount != 1 || arg[0].type!=Variable::CUSTOM ||
      arg[0].val.custom_type.type!=Variable::CustomType::BYTEARRAY)
    return Variable();
  uint8_t *buf;
  uint8_t len = bitsy_python::BitsyHeap::GetVar(arg[0].val.custom_type.val, &buf);
  ota_send(buf, len);
  return Variable(1);
}

Variable local_ip(uint8_t argcount, Variable arg[]) {
  Variable v;
  v.set_int32((uint32_t)WiFi.localIP());
  return v;
}

}
