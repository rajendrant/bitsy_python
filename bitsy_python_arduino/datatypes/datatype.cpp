#include "datatype.h"

#include <string.h>
#include <stdio.h>

#include "../bitsylimit.h"
#include "../bitsy_python_vm.h"
#include "iter.h"

namespace bitsy_python {

// static
Variable DataType::CreateStr(const char *str, uint8_t len) {
  Variable v;
  uint8_t *var;
  uint8_t id = bitsy_heap.CreateVar(len, &var);
  strncpy((char *)var, str, len);
  v.set_CustomType(Variable::CustomType::STRING, id);
  return v;
}

// static
Variable DataType::CreateForType(uint8_t t, uint8_t argcount, Variable args[]) {
  Variable v;
  switch (t) {
    case Variable::CustomType::BYTEARRAY: {
      BITSY_ASSERT(argcount == 1);
      uint8_t *var;
      uint8_t id = bitsy_heap.CreateVar(args[0].as_uint12(), &var);
      v.set_CustomType(t, id);
      break;
    }
    case Variable::CustomType::ITER:
      BITSY_ASSERT(argcount == 1);
      return IterCreate(args[0]);
    case Variable::CustomType::RANGE: {
      int32_t start=0, end, inc=1;
      if (argcount==1) {
        end = args[0].as_int32();
      } else if (argcount==2) {
        start = args[0].as_int32();
        end = args[1].as_int32();
      } else if (argcount==3) {
        start = args[0].as_int32();
        end = args[1].as_int32();
        inc = args[2].as_int32();
      } else {
        BITSY_ASSERT(false);
        break;
      }
      uint32_t *var;
      auto id=bitsy_heap.CreateVar(12, (uint8_t**)&var);
      v.set_CustomType(t, id);
      var[0]=start;
      var[1]=end;
      var[2]=inc;
      break;
    }
    default:
      BITSY_ASSERT(false);
  }
  return v;
}

// static
Variable DataType::GetIndex(const Variable &v, uint8_t ind) {
  Variable ret;
  BITSY_ASSERT(v.type == Variable::CUSTOM);
  uint8_t *var;
  uint8_t len = bitsy_heap.GetVar(v.val.custom_type.val, &var);
  BITSY_ASSERT(ind < len);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::BYTEARRAY:
      ret.set_CustomType(Variable::CustomType::CHARACTER, var[ind]);
      break;
    case Variable::CustomType::STRING:
      BITSY_ASSERT(len > ind);
      ret.set_CustomType(Variable::CustomType::CHARACTER, var[ind]);
      break;
    default:
      BITSY_ASSERT(false);
  }
  return ret;
}

// static
void DataType::SetIndex(Variable &v, uint8_t ind, const Variable& val) {
  BITSY_ASSERT(v.type == Variable::CUSTOM);
  uint8_t *var;
  uint8_t len = bitsy_heap.GetVar(v.val.custom_type.val, &var);
  BITSY_ASSERT(ind < len);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::BYTEARRAY:
      if (val.type == Variable::CUSTOM &&
          val.val.custom_type.type == Variable::CustomType::STRING) {
        var[ind] = GetIndex(val, 0).as_uint8();
      } else {
        var[ind] = val.as_uint8();
      }
      break;
    case Variable::CustomType::STRING:
      // TypeError: 'str' object does not support item assignment
    default:
      BITSY_ASSERT(false);
  }
}

// static
uint16_t DataType::Len(const Variable &v) {
  BITSY_ASSERT(v.type == Variable::CUSTOM);
  uint8_t *var;
  uint8_t len = bitsy_heap.GetVar(v.val.custom_type.val, &var);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::CHARACTER:
      return 1;
    case Variable::CustomType::BYTEARRAY:
      return len;
    case Variable::CustomType::STRING:
      return len - 1;
    default:
      BITSY_ASSERT(false);
  }
  return 0;
}

// static
void DataType::Print(const Variable &v, void (*print)(char)) {
  BITSY_ASSERT(v.type == Variable::CUSTOM);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::CHARACTER:
      print((char)v.val.custom_type.val);
      return;
    case Variable::CustomType::UINT12: {
#ifdef DESKTOP
      char buf[5];
      uint8_t no=snprintf(buf, 5, "%d", v.val.custom_type.val);
      for(uint8_t i=0; i<no; i++) print(buf[i]);
#endif
      return;
    }
  }

  uint8_t *var;
  uint8_t len = bitsy_heap.GetVar(v.val.custom_type.val, &var);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::BYTEARRAY:
      for (uint8_t i = 0; i < len; i++) print(var[i]);
      break;
    case Variable::CustomType::STRING:
      for (uint8_t i = 0; i < len - 1; i++) print(var[i]);
      break;
    default:
      BITSY_ASSERT(false);
  }
}

// static
void DataType::updateUsedContainers(uint8_t start_id, const Variable &v, uint32_t *map) {
  BITSY_ASSERT(v.type == Variable::CUSTOM);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::ITER: {
      uint16_t *val;
      bitsy_heap.GetVar(v.val.custom_type.val, (uint8_t**)&val);
      Variable iter;
      iter.type = Variable::CUSTOM;
      memcpy(&iter.val.custom_type, val, 2);
      if (iter.is_custom_heap_type() && iter.val.custom_type.val>=start_id &&
          iter.val.custom_type.val<start_id+32) {
        *map |= 0x1<<(iter.val.custom_type.val-start_id);
      }
      break;
    }
    case Variable::CustomType::LIST:
      break;
  }
}

}
