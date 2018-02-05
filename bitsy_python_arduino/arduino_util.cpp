#include "arduino_util.h"

#ifdef ARDUINO

#include "BitString.h"

namespace arduino_util {

void soft_reset() {
  // Reset bit string to trigger a reinit.
  bitsy_python::BitString::mark_insane();
}
};
#endif
