#include <VoltageReference.h>

namespace readvcc {

Variable readvcc(uint8_t argcount, Variable arg[]) {
  VoltageReference vRef;
  vRef.begin();
  Variable v;
  v.set_int32(vRef.readVcc());
  return v;
}

}
