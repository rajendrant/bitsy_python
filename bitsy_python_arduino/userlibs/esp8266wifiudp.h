#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

namespace esp8266wifiudp {

WiFiUDP wifiUDP;
char ssid[32], password[32];
uint16_t udp_port;
uint16_t on_recv_callback = 0xFF;
uint32_t last_check = 0;
uint32_t remote_ip;
uint16_t remote_port;

void do_connect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  wifiUDP.begin(udp_port);
}

void connect(const char *_ssid, const char *_password, uint16_t _port) {
  strncpy(ssid, _ssid, sizeof(ssid));
  strncpy(password, _password, sizeof(password));
  ssid[sizeof(ssid)-1] = password[sizeof(password)-1] = 0;
  udp_port = _port;
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
  wifiUDP.beginPacket(remote_ip, remote_port);
  wifiUDP.write(buf, len);
  wifiUDP.endPacket();
}

uint8_t ota_recv(uint8_t *buf, uint8_t len) {
  checkConnectivity();
  if (!wifiUDP.parsePacket())
    return 0;
  remote_ip = wifiUDP.remoteIP();
  remote_port = wifiUDP.remotePort();
  return wifiUDP.read(buf, len);
}

Variable set_on_recv_callback(uint8_t argcount, Variable arg[]) {
  if (argcount == 1 && arg[0].type==Variable::CUSTOM &&
      arg[0].val.custom_type.type==Variable::CustomType::USER_FUNCTION) {
    on_recv_callback = arg[0].val.custom_type.val;
  }
  return Variable::Zero();
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
  if (argcount != 2 || arg[0].type!=Variable::CUSTOM ||
      arg[0].val.custom_type.type!=Variable::CustomType::BYTEARRAY)
    return Variable::Zero();
  uint8_t *buf;
  uint8_t len = arg[1].as_uint8();
  if (bitsy_python::BitsyHeap::GetVar(arg[0].val.custom_type.val, &buf) < len)
    return Variable::Zero();
  ota_send(buf, len);
  return Variable(1);
}

Variable local_ip(uint8_t argcount, Variable arg[]) {
  Variable v;
  v.set_int32((uint32_t)WiFi.localIP());
  return v;
}

}
