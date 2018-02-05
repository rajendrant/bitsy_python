#include "iter.h"

#include <string.h>

#include "../bitsy_python_vm.h"

namespace bitsy_python {
Variable IterCreate(const Variable& v) {
  Variable iter;
  uint8_t *var;
  uint8_t id = BitsyHeap::CreateVar(2+4, &var);
  BITSY_ASSERT(v.type==Variable::CUSTOM);
  iter.set_CustomType(Variable::CustomType::ITER, id);
  memcpy(var, &v.val.custom_type, 2);
  memset(var+2, 0, 4);
  return iter;
}

bool IterForLoopIter(const Variable& iter, Variable *elem) {
  BITSY_ASSERT(iter.type == Variable::CUSTOM &&
         iter.val.custom_type.type == Variable::CustomType::ITER);
  uint8_t *var;
  uint8_t len = BitsyHeap::GetVar(iter.val.custom_type.val, &var);
  BITSY_ASSERT(len == 2+4);
  uint32_t ind;
  Variable::CustomType iter_val;
  memcpy(&iter_val, var, 2);
  memcpy(&ind, var+2, sizeof(ind));
  if(IterForLoop(iter_val, ind, elem)) {
    ind++;
    memcpy(var+2, &ind, sizeof(ind));
    return true;
  }
  return false;
}

bool IterForLoop(Variable::CustomType iter, uint32_t ind, Variable *elem) {
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
    int32_t r[3], nxt;
    memcpy(r, iter_var, 12);
    nxt=r[0] + ind*r[2];
    if (nxt < r[1]) {
      elem->set_int32(nxt);
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
