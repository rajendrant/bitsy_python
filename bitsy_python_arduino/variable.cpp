#include "variable.h"
#include <stdlib.h>

#include "bitsylimit.h"

// namespace bitsy_python {

Variable::Variable(uint8_t v) : type(UINT8) { val.uint8 = v; }

void Variable::set_uint8(uint8_t v) {
    type = UINT8;
    val.uint8 = v;
}

void Variable::set_uint12(uint16_t v) {
  if (v <= 0xFF) {
    type = UINT8;
    val.uint8 = (uint8_t)v;
  } else if (v <= 0xFFF) {
    set_CustomType(CustomType::UINT12, v);
  }
}

void Variable::set_int32(int32_t v) {
  if (v >= 0 && v <= 0xFFF) {
    set_uint12(v);
  } else {
    type = INT32;
    val.int32 = v;
  }
}

void Variable::set_float(float v) {
  type = FLOAT;
  val.float32 = v;
}

uint8_t Variable::as_uint8() const {
  if (type == UINT8) return val.uint8;
  if (type == CUSTOM) return val.custom_type.val;
  return 0;
}

uint16_t Variable::as_uint12() const {
  if (type == UINT8) return val.uint8;
  if (type == CUSTOM) return val.custom_type.val;
  return 0;
}

int32_t Variable::as_int32() const {
  if (type == INT32) return val.int32;
  if (type == UINT8) return val.uint8;
  if (type == CUSTOM) return val.custom_type.val;
  return 0;
}

float Variable::as_float() const {
  if (type == INT32) return val.int32;
  if (type == UINT8) return val.uint8;
  if (type == FLOAT) return val.float32;
  return 0;
}

bool Variable::as_bool() const {
  if (type == INT32) return val.int32;
  if (type == UINT8) return val.uint8;
  if (type == FLOAT) return val.float32;
  return 0;
}

// static
Variable Variable::CustomTypeVariable(uint8_t type, uint16_t val) {
  Variable v;
  BITSY_ASSERT(sizeof(v.val.custom_type) == 2);
  v.type = Variable::CUSTOM;
  v.val.custom_type.type = type;
  v.val.custom_type.val = val;
  return v;
}

// static
Variable Variable::FunctionVariable(uint8_t id) {
  return CustomTypeVariable(Variable::CustomType::USER_FUNCTION, id);
}

void Variable::set_CustomType(uint8_t t, uint16_t v) {
  type = Variable::CUSTOM;
  val.custom_type.type = t;
  val.custom_type.val = v;
}

uint8_t Variable::size() const {
  return get_size_from_type(type);
}

// static
uint8_t Variable::get_size_from_type(uint8_t type) {
  switch (type) {
    case UINT8:
      return 1;
    case INT32:
    case FLOAT:
      return 4;
  }
  // case CUSTOM:
  return VARIABLE_SIZE_CUSTOM_TYPE;
}

// static
bool Variable::is_custom_heap_type(Variable::CustomType v) {
  switch(v.type) {
  case CustomType::STRING:
  case CustomType::LIST:
  case CustomType::BYTEARRAY:
  case CustomType::RANGE:
  case CustomType::ITER:
  case CustomType::INT32:
  case CustomType::FLOAT:
    return true;
  }
  return false;
}

Variable Variable::ToHeap() const {
  Variable v=*this;
  switch(type) {
    case UINT8:
      v.set_CustomType(CustomType::UINT12, val.uint8);
      break;
    case INT32:
    case FLOAT:
      BITSY_ASSERT(false);
      break;
  }
  return v;
}

Variable Variable::ToWithin() const {
  Variable v=*this;
  switch(type) {
    case INT32:
    case FLOAT:
      BITSY_ASSERT(false);
      break;
  }
  return v;
}

//}
