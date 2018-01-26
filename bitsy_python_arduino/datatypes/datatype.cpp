#include "datatype.h"

#include <assert.h>
#include <string.h>

#include "../bitsy_python_vm.h"

namespace bitsy_python {

// static
Variable DataType::CreateStr(BitsyHeap &heap, const char *str, uint8_t len) {
  Variable v;
  uint8_t *var;
  uint8_t id = heap.CreateVar(len, &var);
  strncpy((char *)var, str, len);
  v.set_CustomType(Variable::CustomType::STRING, id);
  return v;
}

// static
Variable DataType::CreateForType(BitsyHeap &heap, uint8_t t, uint8_t argcount, Variable args[]) {
  Variable v;
  switch (t) {
    case Variable::CustomType::BYTEARRAY: {
      assert(argcount == 1);
      uint8_t *var;
      uint8_t id = heap.CreateVar(args[0].as_int16(), &var);
      v.set_CustomType(t, id);
      break;
    }
    default:
      assert(false);
  }
  return v;
}

// static
Variable DataType::GetIndex(BitsyHeap &heap, const Variable &v, uint8_t ind) {
  Variable ret;
  assert(v.type == Variable::CUSTOM);
  uint8_t *var;
  uint8_t len = heap.GetVar(v.val.custom_type.val, &var);
  assert(ind < len);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::BYTEARRAY:
      ret.set_CustomType(Variable::CustomType::CHARACTER, var[ind]);
      break;
    case Variable::CustomType::STRING:
      assert(len > ind);
      ret.set_CustomType(Variable::CustomType::CHARACTER, var[ind]);
      break;
    default:
      assert(false);
  }
  return ret;
}

// static
void DataType::SetIndex(BitsyHeap &heap, Variable &v, uint8_t ind, const Variable& val) {
  assert(v.type == Variable::CUSTOM);
  uint8_t *var;
  uint8_t len = heap.GetVar(v.val.custom_type.val, &var);
  assert(ind < len);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::BYTEARRAY:
      if (val.type == Variable::CUSTOM &&
          val.val.custom_type.type == Variable::CustomType::STRING) {
        var[ind] = GetIndex(heap, val, 0).as_uint8();
      } else {
        var[ind] = val.as_uint8();
      }
      break;
    default:
      assert(false);
  }
}

// static
uint16_t DataType::Len(BitsyHeap &heap, const Variable &v) {
  assert(v.type == Variable::CUSTOM);
  uint8_t *var;
  uint8_t len = heap.GetVar(v.val.custom_type.val, &var);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::CHARACTER:
      return 1;
    case Variable::CustomType::BYTEARRAY:
      return len;
    case Variable::CustomType::STRING:
      return len - 1;
    default:
      assert(false);
  }
}

// static
void DataType::Print(BitsyHeap &heap, const Variable &v, void (*print)(char)) {
  assert(v.type == Variable::CUSTOM);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::CHARACTER:
      print((char)v.val.custom_type.val);
      return;
  }

  uint8_t *var;
  uint8_t len = heap.GetVar(v.val.custom_type.val, &var);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::BYTEARRAY:
      for (uint8_t i = 0; i < len; i++) print(var[i]);
      break;
    case Variable::CustomType::STRING:
      for (uint8_t i = 0; i < len - 1; i++) print(var[i]);
      break;
    case Variable::CustomType::CHARACTER:
      // Already handled above.
      break;
    default:
      assert(false);
  }
}
}
