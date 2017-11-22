#include "variable.h"
#include <assert.h>
#include <stdlib.h>

// namespace bitsy_python {

Variable::Variable() : type(UINT8) { val.uint8 = 0; }

void Variable::set_int16(int16_t v) {
  if (v >= 0 && v <= 0xFF) {
    type = UINT8;
    val.uint8 = (uint8_t)v;
  } else if (abs(v) <= 0x7FFF) {
    type = INT16;
    val.int16 = (uint16_t)v;
  }
}

void Variable::set_int32(int32_t v) {
  if (v >= 0 && v <= 0xFF) {
    type = UINT8;
    val.uint8 = (uint8_t)v;
  } else if (abs(v) <= 0x7FFF) {
    type = INT16;
    val.int16 = (uint16_t)v;
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
  if (type == INT16) return val.int16;
  if (type == UINT8) return val.uint8;
  if (type == CUSTOM) return val.custom_type.val;
  return 0;
}

int16_t Variable::as_int16() const {
  if (type == INT16) return val.int16;
  if (type == UINT8) return val.uint8;
  if (type == CUSTOM) return val.custom_type.val;
  return 0;
}

int32_t Variable::as_int32() const {
  if (type == INT32) return val.int32;
  if (type == INT16) return val.int16;
  if (type == UINT8) return val.uint8;
  if (type == CUSTOM) return val.custom_type.val;
  return 0;
}

float Variable::as_float() const {
  if (type == INT32) return val.int32;
  if (type == INT16) return val.int16;
  if (type == UINT8) return val.uint8;
  if (type == FLOAT) return val.float32;
  return 0;
}

bool Variable::as_bool() const {
  if (type == INT32) return val.int32;
  if (type == INT16) return val.int16;
  if (type == UINT8) return val.uint8;
  if (type == FLOAT) return val.float32;
  return 0;
}

// static
Variable Variable::Zero() {
  Variable v;
  v.set_int16(0);
  return v;
}

// static
Variable Variable::FunctionVariable(uint8_t id) {
  Variable v;
  assert(sizeof(v.val.custom_type) == 2);
  v.type = Variable::CUSTOM;
  v.val.custom_type.type = Variable::CustomType::USER_FUNCTION;
  v.val.custom_type.val = id;
  return v;
}

// static
Variable Variable::ModuleVariable(uint8_t id) {
  assert(id <= 0x3F);
  Variable v;
  assert(sizeof(v.val.custom_type) == 2);
  v.type = Variable::CUSTOM;
  v.val.custom_type.type = Variable::CustomType::USER_MODULE;
  v.val.custom_type.val = id;
  return v;
}

// static
Variable Variable::ModuleFunctionVariable(const Variable& module, uint8_t id) {
  assert(id <= 0x3f);
  assert(module.type == Variable::CUSTOM &&
         module.val.custom_type.type == Variable::CustomType::USER_MODULE);
  assert(module.val.custom_type.val <= 0x3F);
  Variable v;
  assert(sizeof(v.val.custom_type) == 2);
  v.type = Variable::CUSTOM;
  v.val.custom_type.type = Variable::CustomType::USER_MODULE_FUNCTION;
  v.val.custom_type.val =
      (module.val.custom_type.val & 0x3F) << 6 | (id & 0x3F);
  return v;
}

void Variable::set_CustomType(uint8_t t, uint16_t v) {
  type = Variable::CUSTOM;
  val.custom_type.type = t;
  val.custom_type.val = v;
}

uint8_t Variable::size() const {
  switch (type) {
    case UINT8:
      return 1;
    case INT16:
    case CUSTOM:
      return 2;
    case INT32:
    case FLOAT:
      return 4;
    default:
      assert(false);
  }
}

uint8_t Variable::get_extra_bits() const {
  switch (type) {
    case UINT8:
    case INT16:
    case INT32:
      return 0;
    case FLOAT:
      return 1;
    case CUSTOM:
      return 1;
    default:
      assert(false);
  }
}

// static
uint8_t Variable::get_type_from_size_and_extra_bits(uint8_t size,
                                                    uint8_t extra) {
  switch (size) {
    case 1:
      return UINT8;
    case 2:
      if (extra == 0)
        return INT16;
      else
        return CUSTOM;
    case 4:
      if (extra == 0)
        return INT32;
      else
        return FLOAT;
    default:
      assert(false);
  }
}

//}
