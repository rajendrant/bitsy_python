#include <NRFLite.h>

#include "../BitsyHeap.h"
#include "../bitsy_python_vm.h"

extern void ota_update_nrf24();

namespace nrf24 {

NRFLite radio;
uint16_t on_recv_callback = 0xFF;
uint8_t is_powered_down = 0;

bool ota_send2(const uint8_t *buf, uint8_t len, uint8_t to_radio) {
  is_powered_down = 0;
  return radio.send(to_radio, (void*)buf, len);
}

bool ota_send(const uint8_t *buf, uint8_t len) {
  return ota_send2(buf, len, NRF24_GATEWAY);
}

uint8_t ota_recv(uint8_t *buf, uint8_t len) {
  if (is_powered_down)
    return 0;
  uint8_t l=radio.hasData();
  if (l==0 || l>len) return 0;
  radio.readData(buf);
  return l;
}

Variable init(uint8_t argcount, Variable arg[]) {
  if (argcount < 3)
    return 0;
  uint8_t radioId=arg[0].as_uint8(), cePin=arg[1].as_uint8(), csnPin=arg[2].as_uint8();
  uint8_t bitrate=NRFLite::BITRATE2MBPS, channel=100;
  if (argcount >= 4)
    bitrate = arg[3].as_uint8();
  if (argcount >= 5)
    channel = arg[4].as_uint8();
  is_powered_down = 0;
  return radio.init(radioId, cePin, csnPin, (NRFLite::Bitrates)bitrate, channel);
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

Variable send(uint8_t argcount, Variable arg[]) {
  if (argcount != 2 || arg[1].type!=Variable::CUSTOM ||
      arg[1].val.custom_type.type!=Variable::CustomType::BYTEARRAY)
    return Variable();
  uint8_t *buf;
  uint8_t len = bitsy_python::BitsyHeap::GetVar(arg[1].val.custom_type.val, &buf);
  return ota_send2(buf, len, arg[0].as_uint8());
}

Variable powerup(uint8_t argcount, Variable arg[]) {
  is_powered_down = 0;
  return 0;
}

Variable powerdown(uint8_t argcount, Variable arg[]) {
  is_powered_down = 1;
  radio.powerDown();
  return 0;
}

};
