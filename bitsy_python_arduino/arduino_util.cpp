#include "arduino_util.h"

#ifdef ARDUINO

namespace arduino_util {

void soft_reset() {
  volatile void (*resetFunc)(void) = 0;
  resetFunc();
}
};
#endif
