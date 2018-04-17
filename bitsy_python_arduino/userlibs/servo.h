#include <Servo.h>

namespace servo {

struct ServoPinMap {
  byte pin;
  Servo s;
};

static ServoPinMap *map = NULL;
static byte len = 0;

static Servo *GetOrAlloc(uint8_t pin) {
  for (byte i = 0; i < len; i++) {
    if (map[i].pin == pin) return &map[i].s;
  }
  len++;
  map = (ServoPinMap *)realloc(map, sizeof(ServoPinMap) * len);
  if (map) {
    map[len - 1].pin = pin;
    Servo s;
    map[len - 1].s = s;
    return &map[len - 1].s;
  }
  return NULL;
}

void Dealloc(uint8_t pin) {
  for (byte i = 0; i < len; i++) {
    if (map[i].pin == pin) {
      map[i] = map[len - 1];
      len--;
      map = (ServoPinMap *)realloc(map, sizeof(ServoPinMap) * len);
    }
  }
}

void init() {
  if (map)
    free(map);
  map = NULL;
  len = 0;
}

Variable attach(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    Servo *s = GetOrAlloc(arg[0].as_uint8());
    s->attach(arg[0].as_uint8());
  }
  return 0;
}

Variable detach(uint8_t argcount, Variable arg[]) {
  if (argcount == 1) {
    Servo *s = GetOrAlloc(arg[0].as_uint8());
    s->detach();
    //Dealloc(arg[0].as_uint8());
  }
  return 0;
}

Variable read(uint8_t argcount, Variable arg[]) {
  Variable ret;
  if (argcount == 1) {
    Servo *s = GetOrAlloc(arg[0].as_uint8());
    ret.set_uint8(s->read());
  }
  return ret;
}

Variable write(uint8_t argcount, Variable arg[]) {
  if (argcount == 2) {
    Servo *s = GetOrAlloc(arg[0].as_uint8());
    s->write(arg[1].as_uint8());
  }
  return 0;
}
};
