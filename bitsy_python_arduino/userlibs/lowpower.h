#include "LowPower.h"

namespace lowpower {

Variable powerdown(uint8_t argcount, Variable arg[]) {
  period_t p=SLEEP_1S;
  if (argcount==1) {
    uint16_t ms=arg[0].as_int32();
    if (ms <= 60)
      p = SLEEP_60MS;
    else if (ms <= 120)
      p = SLEEP_120MS;
    else if (ms <= 250)
      p = SLEEP_250MS;
    else if (ms <= 500)
      p = SLEEP_500MS;
    else if (ms <= 1000)
      p = SLEEP_1S;
    else if (ms <= 2000)
      p = SLEEP_2S;
    else if (ms <= 4000)
      p = SLEEP_4S;
    else if (ms <= 8000)
      p = SLEEP_8S;
  }
  LowPower.powerDown(p, ADC_OFF, BOD_OFF);
  return Variable::Zero();
}

}
