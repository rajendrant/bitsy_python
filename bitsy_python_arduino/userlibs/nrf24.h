#include <NRFLite.h>

#include "../bitsy_heap.h"

namespace nrf24 {

NRFLite radio;

Variable init(uint8_t argcount, Variable arg[]) {
  if (argcount < 3)
    return Variable::Zero();
  uint8_t radioId=arg[0].as_uint8(), cePin=arg[1].as_uint8(), csnPin=arg[2].as_uint8();
  uint8_t bitrate=BITRATE2MBPS, channel=100;
  if (argcount >= 4) {
    bitrate = arg[3].as_uint8();
    if (argcount >= 5)
      channel = arg[4].as_uint8();
  }
  return radio.init(radioId, cePin, csnPin, bitrate, channel);
}

Variable read(uint8_t argcount, Variable arg[]) {
  uint8_t len = radio.hasData();
  if (len==0)
    return Variable::Zero();
  Variable v;
  uint8_t *buf;
  v.set_CustomType(Variable::CustomType::BYTEARRAY,
                   bitsy_python::bitsy_heap.CreateVar(len, &buf));
  radio.readData(buf);
  return v;
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
