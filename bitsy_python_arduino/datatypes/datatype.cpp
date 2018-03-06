#include "datatype.h"

#include <string.h>
#include <stdio.h>

#include "../gc.h"
#include "../bitsylimit.h"
#include "../bitsy_python_vm.h"
#include "iter.h"

extern void BITSY_PYTHON_PRINT(const char* str);
extern void BITSY_PYTHON_PRINT_VAR(const Variable& v);

namespace bitsy_python {
namespace DataType {

#define GLOBAL_VARIABLE_ID 0

Variable CreateStr(const char *str, uint8_t len) {
  gc();
  uint8_t *var;
  uint8_t id = BitsyHeap::CreateVar(len-1, &var);
  strncpy((char *)var, str, len-1);
  return Variable::CustomTypeVariable(Variable::CustomType::STRING, id);
}

Variable CreateByteArray(uint8_t len) {
  uint8_t *var;
  return Variable::CustomTypeVariable(Variable::CustomType::BYTEARRAY,
      BitsyHeap::CreateVar(len, &var));
}

Variable CreateIter(const Variable& container) {
  return IterCreate(container);
}

Variable CreateInt32(int32_t no) {
  Variable v;
  uint8_t *var;
  v.set_CustomType(Variable::CustomType::INT32, BitsyHeap::CreateVar(4, &var));
  memcpy(var, &no, 4);
  return v;
}

Variable CreateFloat(float no) {
  Variable v;
  uint8_t *var;
  v.set_CustomType(Variable::CustomType::FLOAT, BitsyHeap::CreateVar(4, &var));
  memcpy(var, &no, 4);
  return v;
}

Variable CreateList(uint8_t argcount, Variable args[]) {
  Variable v;
  uint8_t *var;
  v.set_CustomType(Variable::CustomType::LIST,
      BitsyHeap::CreateVar(1+2*argcount, &var));
  var[0] = argcount;
  for(uint8_t i=0; i<argcount; i++)
    SetIndex(v, i, args[i]);
  return v;
}

Variable CreateForType(uint8_t t, uint8_t argcount, Variable args[]) {
  Variable v;
  uint8_t *var;
  switch (t) {
    case Variable::CustomType::RANGE: {
      int32_t r[3];
      r[0]=0;
      r[2]=1;
      if (argcount==1) {
        r[1] = args[0].as_int32();
      } else if (argcount==2) {
        r[0] = args[0].as_int32();
        r[1] = args[1].as_int32();
      } else if (argcount==3) {
        r[0] = args[0].as_int32();
        r[1] = args[1].as_int32();
        r[2] = args[2].as_int32();
      } else {
        BITSY_ASSERT(false);
        break;
      }
      v.set_CustomType(t, BitsyHeap::CreateVar(12, &var));
      memcpy(var, r, 12);
      break;
    }
    default:
      BITSY_ASSERT(false);
  }
  return v;
}

Variable GetIndex(const Variable &v, uint8_t ind) {
  Variable ret;
  BITSY_ASSERT(v.type == Variable::CUSTOM);
  uint8_t *var;
  uint8_t len = BitsyHeap::GetVar(v.val.custom_type.val, &var);
  BITSY_ASSERT(ind < len);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::BYTEARRAY:
    case Variable::CustomType::STRING:
      BITSY_ASSERT(len > ind);
      return Variable::CustomTypeVariable(Variable::CustomType::CHARACTER, var[ind]);
    case Variable::CustomType::LIST:
      if(len>0 && ind<var[0]) {
        ret.type=Variable::CUSTOM;
        memcpy(&ret.val.custom_type, var+1+2*ind, VARIABLE_SIZE_CUSTOM_TYPE);
        ret = ret.ToWithin();
      }
      break;
    default:
      BITSY_ASSERT(false);
  }
  return ret;
}

void SetIndex(const Variable &v, uint8_t ind, const Variable& val) {
  BITSY_ASSERT(v.type == Variable::CUSTOM);
  uint8_t *var;
  uint8_t len = BitsyHeap::GetVar(v.val.custom_type.val, &var);
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
    case Variable::CustomType::LIST:
      if(len>0 && ind<var[0]) {
        Variable v=val.ToHeap();
        memcpy(var+1+2*ind, &v.val.custom_type, VARIABLE_SIZE_CUSTOM_TYPE);
      }
      break;
    default:
      BITSY_ASSERT(false);
  }
}

uint16_t Len(const Variable &v) {
  BITSY_ASSERT(v.type == Variable::CUSTOM);
  uint8_t *var;
  uint8_t len = BitsyHeap::GetVar(v.val.custom_type.val, &var);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::CHARACTER:
      return 1;
    case Variable::CustomType::BYTEARRAY:
    case Variable::CustomType::STRING:
      return len;
    case Variable::CustomType::LIST:
      return var[0];
    default:
      BITSY_ASSERT(false);
  }
  return 0;
}

void Print(const Variable &v, void (*print)(char)) {
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
  uint8_t len = BitsyHeap::GetVar(v.val.custom_type.val, &var);
  switch (v.val.custom_type.type) {
    case Variable::CustomType::BYTEARRAY:
    case Variable::CustomType::STRING:
      for (uint8_t i = 0; i < len; i++) print(var[i]);
      break;
    case Variable::CustomType::LIST:
      BITSY_PYTHON_PRINT("[");
      for (uint8_t i = 0; i < var[0]; i++) {
        BITSY_PYTHON_PRINT_VAR(GetIndex(v, i));
        if(i+1!=var[0])BITSY_PYTHON_PRINT(", ");
      }
      BITSY_PYTHON_PRINT("]");
      break;
    default:
      BITSY_ASSERT(false);
  }
}

bool InOperator(const Variable& cont, const Variable& e) {
  BITSY_ASSERT(cont.type == Variable::CUSTOM);
  uint8_t *var;
  uint8_t len = BitsyHeap::GetVar(cont.val.custom_type.val, &var);
  switch (cont.val.custom_type.type) {
    case Variable::CustomType::BYTEARRAY:
    case Variable::CustomType::STRING:
    {
      // TODO(rajendrant): This only finds if the character is single letter.
      uint8_t v;
      if (e.type==Variable::UINT8 || (e.type==Variable::CUSTOM &&
          (e.val.custom_type.type==Variable::CustomType::CHARACTER ||
           e.val.custom_type.type==Variable::CustomType::UINT12))) {
        v = e.as_uint8();
      } else if (e.type==Variable::CUSTOM &&
                 e.val.custom_type.type==Variable::CustomType::STRING) {
        uint8_t *strvar;
        if(BitsyHeap::GetVar(e.val.custom_type.val, &strvar)==1)
          v = strvar[0];
        else return false;
      } else
        return false;
      for (uint8_t i = 0; i < len; i++)
        if (var[i]==v) return true;
      break;
    }
    case Variable::CustomType::LIST:
      BITSY_ASSERT(false);
  }
  return false;
}

static void mark_if_custom_heap_type(const uint8_t *type, uint8_t start_id, uint32_t *map) {
  Variable::CustomType v;
  memcpy(&v, type, VARIABLE_SIZE_CUSTOM_TYPE);
  if (Variable::is_custom_heap_type(v) && v.val>=start_id &&
      v.val<start_id+32) {
    *map |= 0x1L<<(v.val-start_id);
  }
}

void updateUsedContainers(uint8_t start_id, Variable::CustomType v, uint32_t *map) {
  switch (v.type) {
    case Variable::CustomType::ITER:
    case Variable::CustomType::LIST:
      break;
    default:
      return;
  }
  uint8_t *var;
  BitsyHeap::GetVar(v.val, &var);
  switch (v.type) {
    case Variable::CustomType::ITER:
      mark_if_custom_heap_type(var, start_id, map);
      break;
    case Variable::CustomType::LIST:
      for (uint8_t i = 0; i < var[0]; i++)
        mark_if_custom_heap_type(var+1+2*i, start_id, map);
      break;
  }
}

Variable initGlobalVars(uint8_t global_vars) {
  Variable args[global_vars];
  auto v = CreateList(global_vars, args);
  BITSY_ASSERT(v.type==Variable::CUSTOM &&
      v.val.custom_type.type == Variable::CustomType::LIST &&
      v.val.custom_type.val == GLOBAL_VARIABLE_ID);
  return v;
}

Variable getGlobalVar(uint8_t id) {
  return GetIndex(Variable::CustomTypeVariable(Variable::CustomType::LIST, GLOBAL_VARIABLE_ID), id);
}

void setGlobalVar(uint8_t id, const Variable& v) {
  SetIndex(Variable::CustomTypeVariable(Variable::CustomType::LIST, GLOBAL_VARIABLE_ID), id, v);
}

}
}
