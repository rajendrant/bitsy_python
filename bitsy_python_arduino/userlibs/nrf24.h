#include <RF24.h>

namespace nrf24 {

RF24 *radio = NULL;

Variable begin(uint8_t argcount, Variable arg[]) {
  if (radio) {
    delete radio;
    radio = NULL;
  }
  if (argcount == 2) {
    radio = new RF24(arg[0].as_uint8(), arg[0].as_uint8());
    return Variable(radio->begin());
  }
  return Variable::Zero();
}

Variable setChannel(uint8_t argcount, Variable arg[]) {
  if (radio && argcount == 1)
    radio->setChannel(arg[0].as_uint8());
  return Variable::Zero();
}

Variable setPALevel(uint8_t argcount, Variable arg[]) {
  if (radio && argcount == 1)
    radio->setPALevel(arg[0].as_uint8());
  return Variable::Zero();
}

Variable setDataRate(uint8_t argcount, Variable arg[]) {
  if (radio && argcount == 1)
    radio->setDataRate(arg[0].as_uint8());
  return Variable::Zero();
}

Variable startListening(uint8_t argcount, Variable arg[]) {
  if (radio)
    radio->startListening(arg[0].as_uint8());
  return Variable::Zero();
}

Variable stopListening(uint8_t argcount, Variable arg[]) {
  if (radio)
    radio->stopListening(arg[0].as_uint8());
  return Variable::Zero();
}

Variable openWritingPipe(uint8_t argcount, Variable arg[]) {
  if (radio && argcount == 1) {
    int32_t addr = arg[0].as_int32();
    radio->openWritingPipe(&addr);
  }
  return Variable::Zero();
}

Variable openReadingPipe(uint8_t argcount, Variable arg[]) {
  if (radio && argcount == 2) {
    int32_t addr = arg[1].as_int32();
    radio->openReadingPipe(arg[0].as_uint8(), &addr);
  }
  return Variable::Zero();
}


};
