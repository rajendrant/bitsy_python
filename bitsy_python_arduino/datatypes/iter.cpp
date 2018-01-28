#include "iter.h"

#include <string.h>

#include "../bitsy_python_vm.h"

namespace bitsy_python {
Variable IterCreate(const Variable& v) {
  Variable iter;
  uint8_t *var;
  uint8_t id = bitsy_heap.CreateVar(2+2, &var);
  BITSY_ASSERT(v.type==Variable::CUSTOM);
  iter.set_CustomType(Variable::CustomType::ITER, id);
  memcpy(var, &v.val.custom_type, 2);
  var[2] = var[3] = 0;
  return iter;
}

bool IterForLoop(const Variable& iter, Variable *elem) {
  bool ret = false;
  BITSY_ASSERT(iter.type == Variable::CUSTOM &&
         iter.val.custom_type.type == Variable::CustomType::ITER);
  uint8_t *var, *iter_var;
  uint8_t len = bitsy_heap.GetVar(iter.val.custom_type.val, &var);
  BITSY_ASSERT(len == 2+2);
  uint16_t *ind = ((uint16_t*)var)+1;
  Variable::CustomType iter_val;
  memcpy(&iter_val, var, 2);
  uint8_t iter_len = bitsy_heap.GetVar(iter_val.val, &iter_var);
  switch(iter_val.type) {
  case Variable::CustomType::BYTEARRAY:
    if(*ind < iter_len) {
      elem->set_CustomType(Variable::CustomType::UINT12, iter_var[*ind]);
      ret = true;
    }
    break;
  case Variable::CustomType::STRING:
    if(*ind < iter_len-1) {
      elem->set_CustomType(Variable::CustomType::CHARACTER, iter_var[*ind]);
      ret = true;
    }
    break;
  case Variable::CustomType::RANGE: {
    uint32_t *range = (uint32_t*)iter_var;
    if (range[0]+ *ind*range[2] < range[1]) {
      elem->set_int32(range[0]+ *ind*range[2]);
      ret = true;
    }
    break;
  }
  default:
    BITSY_ASSERT(false);
  }
  *ind += 1;
  return ret;
}
}