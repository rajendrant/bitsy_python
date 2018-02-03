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
  uint16_t ind;
  Variable::CustomType iter_val;
  memcpy(&ind, var+2, 2);
  memcpy(&iter_val, var, 2);
  if(IterForLoop(iter_val, ind, elem)) {
    ind++;
    memcpy(var+2, &ind, 2);
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
    int32_t start, end, inc;
    memcpy(&start, iter_var, 4);
    memcpy(&end, iter_var+4, 4);
    memcpy(&inc, iter_var+8, 4);
    if (start + ind*inc < end) {
      elem->set_int32(start + ind*inc);
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
