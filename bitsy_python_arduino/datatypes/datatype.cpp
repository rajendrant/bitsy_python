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

#define GLOBAL_VARIABLE_ID 0

// static
Variable DataType::CreateStr(const char *str, uint8_t len) {
  gc();
  uint8_t *var;
  uint8_t id = BitsyHeap::CreateVar(len-1, &var);
  strncpy((char *)var, str, len-1);
  return Variable::CustomTypeVariable(Variable::CustomType::STRING, id);
}

// static
Variable DataType::CreateForType(uint8_t t, uint8_t argcount, Variable args[]) {
  Variable v;
  uint8_t *var;
  switch (t) {
    case Variable::CustomType::BYTEARRAY: {
      BITSY_ASSERT(argcount == 1);
      return Variable::CustomTypeVariable(t, BitsyHeap::CreateVar(args[0].as_uint8(), &var));
    }
    case Variable::CustomType::ITER:
      BITSY_ASSERT(argcount == 1);
      return IterCreate(args[0]);
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
    case Variable::CustomType::INT32: {
      BITSY_ASSERT(argcount == 1);
      int32_t i32 = args[0].as_int32();
      v.set_CustomType(t, BitsyHeap::CreateVar(4, &var));
      memcpy(var, &i32, 4);
      break;
    }
    case Variable::CustomType::FLOAT: {
      BITSY_ASSERT(argcount == 1);
      float f = args[0].as_float();
      v.set_CustomType(t, BitsyHeap::CreateVar(4, &var));
      memcpy(var, &f, 4);
      break;
    }
    case Variable::CustomType::LIST: {
      v.set_CustomType(t, BitsyHeap::CreateVar(1+2*argcount, &var));
      var[0] = argcount;
      for(uint8_t i=0; i<argcount; i++)
        SetIndex(v, i, args[i]);
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

// static
void DataType::SetIndex(const Variable &v, uint8_t ind, const Variable& val) {
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

// static
uint16_t DataType::Len(const Variable &v) {
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

// static
bool DataType::InOperator(const Variable& cont, const Variable& e) {
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

// static
void DataType::updateUsedContainers(uint8_t start_id, Variable::CustomType v, uint32_t *map) {
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

// static
Variable DataType::initGlobalVars(uint8_t global_vars) {
  Variable args[global_vars];
  auto v = CreateForType(Variable::CustomType::LIST, global_vars, args);
  BITSY_ASSERT(v.type==Variable::CUSTOM &&
      v.val.custom_type.type == Variable::CustomType::LIST &&
      v.val.custom_type.val == GLOBAL_VARIABLE_ID);
  return v;
}

// static
Variable DataType::getGlobalVar(uint8_t id) {
  return GetIndex(Variable::CustomTypeVariable(Variable::CustomType::LIST, GLOBAL_VARIABLE_ID), id);
}

// static
void DataType::setGlobalVar(uint8_t id, const Variable& v) {
  SetIndex(Variable::CustomTypeVariable(Variable::CustomType::LIST, GLOBAL_VARIABLE_ID), id, v);
}

}
