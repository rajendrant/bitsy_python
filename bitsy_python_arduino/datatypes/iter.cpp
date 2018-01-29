#include "iter.h"

#include <string.h>

#include "../bitsy_python_vm.h"

namespace bitsy_python {
Variable IterCreate(const Variable& v) {
  Variable iter;
  uint8_t *var;
  uint8_t id = BitsyHeap::CreateVar(2+2, &var);
  BITSY_ASSERT(v.type==Variable::CUSTOM);
  iter.set_CustomType(Variable::CustomType::ITER, id);
  memcpy(var, &v.val.custom_type, 2);
  var[2] = var[3] = 0;
  return iter;
}

bool IterForLoopIter(const Variable& iter, Variable *elem) {
  BITSY_ASSERT(iter.type == Variable::CUSTOM &&
         iter.val.custom_type.type == Variable::CustomType::ITER);
  uint8_t *var;
  uint8_t len = BitsyHeap::GetVar(iter.val.custom_type.val, &var);
  BITSY_ASSERT(len == 2+2);
  uint16_t *ind = ((uint16_t*)var)+1;
  Variable::CustomType iter_val;
  memcpy(&iter_val, var, 2);
  if(IterForLoop(iter_val, *ind, elem)) {
    *ind += 1;
    return true;
  }
  return false;
}

bool IterForLoop(Variable::CustomType iter, uint16_t ind, Variable *elem) {
  bool ret = false;
  uint8_t *iter_var;
  uint8_t iter_len = BitsyHeap::GetVar(iter.val, &iter_var);
  switch(iter.type) {
  case Variable::CustomType::BYTEARRAY:
    if(ind < iter_len) {
      elem->set_CustomType(Variable::CustomType::UINT12, iter_var[ind]);
      ret = true;
    }
    break;
  case Variable::CustomType::STRING:
    if(ind < iter_len) {
      elem->set_CustomType(Variable::CustomType::CHARACTER, iter_var[ind]);
      ret = true;
    }
    break;
  case Variable::CustomType::RANGE: {
    uint32_t *range = (uint32_t*)iter_var;
    if (range[0]+ ind*range[2] < range[1]) {
      elem->set_int32(range[0]+ ind*range[2]);
      ret = true;
    }
    break;
  }
  default:
    BITSY_ASSERT(false);
  }
  return ret;
}
}