#include <DHT.h>

namespace dht {
DHT *dht = NULL;

void init() {}
 
Variable begin(uint8_t argcount, Variable arg[]) {
  if (argcount == 2) {
    delete dht;
    dht = new DHT(arg[0].as_uint8(), arg[1].as_uint8());
  }
  return 0;
}

Variable readhumidity(uint8_t argcount, Variable arg[]) {
  if (dht)
    return dht->readHumidity()*10;
  return 0;
}

Variable readtemperature(uint8_t argcount, Variable arg[]) {
  if (dht)
    return dht->readTemperature()*10;
  return 0;
}

};
