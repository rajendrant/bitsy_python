#include <NRFLite.h>

#include "../BitsyHeap.h"

namespace nrf24 {

NRFLite radio;
uint16_t on_recv_callback = 0xFF;

#define NRF24_GATEWAY 0xA5

bool ota_nrf24_send(uint8_t *buf, uint8_t len) {
  return radio.send(NRF24_GATEWAY, buf, len);
}

uint8_t ota_nrf24_recv(uint8_t *buf, uint8_t len) {
  uint8_t l=radio.hasData();
  if (l==0 || l>len) return 0;
  radio.readData(buf);
  return l;
}

Variable init(uint8_t argcount, Variable arg[]) {
  if (argcount < 4)
    return Variable::Zero();
  uint8_t radioId=arg[0].as_uint8(), cePin=arg[1].as_uint8(), csnPin=arg[2].as_uint8();
  uint8_t bitrate=NRFLite::BITRATE2MBPS, channel=100;
  if (argcount >= 4)
    bitrate = arg[3].as_uint8();
  if (argcount >= 5)
    channel = arg[4].as_uint8();
  if (argcount >= 6 && arg[5].type==Variable::CUSTOM &&
      arg[5].val.custom_type.type==Variable::CustomType::USER_FUNCTION) {
    on_recv_callback = arg[5].val.custom_type.val;
  }
  return radio.init(radioId, cePin, csnPin, (NRFLite::Bitrates)bitrate, channel);
}

void send_to_callback(uint8_t *buf, uint8_t len) {
  if (on_recv_callback != 0xFF) {
    Variable v;
    uint8_t *vbuf;
    v.set_CustomType(Variable::CustomType::BYTEARRAY,
                     bitsy_python::bitsy_heap.CreateVar(len, &vbuf));
    memcpy(vbuf, buf, len);
    vm.callUserFunction(on_recv_callback, v);
  }
}

Variable send(uint8_t argcount, Variable arg[]) {
  if (argcount != 3 || arg[1].type!=Variable::CUSTOM ||
      arg[1].val.custom_type.type!=Variable::CustomType::BYTEARRAY)
    return Variable::Zero();
  uint8_t *buf;
  uint8_t len = arg[2].as_uint8();
  if (bitsy_python::bitsy_heap.GetVar(arg[1].val.custom_type.val, &buf) < len)
    return Variable::Zero();
  radio.send(arg[0].as_uint8(), buf, arg[2].as_uint8());
  return Variable(1);
}

};
