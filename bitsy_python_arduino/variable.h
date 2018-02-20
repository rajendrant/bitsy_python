#ifndef VARIABLE_H_
#define VARIABLE_H_

#include <stdint.h>

// namespace bitsy_python {

class Variable {
 public:
  enum Type {
    UINT8,
    INT32,
    FLOAT,
    CUSTOM,  // Uses 2 bytes. 4 bits are used to encode the type.
  };

  typedef struct {
    enum {
      USER_FUNCTION,
      GLOBAL_VARIABLE,
      BUILTIN_FUNCTION,
      USER_MODULE,
      USER_MODULE_FUNCTION,
      CHARACTER,
      UINT12,

      // These variables live in the heap and their ID is stored in val
      STRING,
      BYTEARRAY,
      RANGE,
      ITER,
      LIST,
      INT32,
      FLOAT,
    };
    uint8_t type : 4;
    uint16_t val : 12;
  } CustomType;

  union {
    uint8_t uint8;
    int32_t int32;
    float float32;
    CustomType custom_type;
  } val;
  unsigned char type : 2;

  static Variable Zero();
  static Variable CustomTypeVariable(uint8_t type, uint16_t val);
  static Variable FunctionVariable(uint8_t id);
  static Variable ModuleVariable(uint8_t id);
  static Variable ModuleFunctionVariable(const Variable& module, uint8_t id);

  Variable(uint8_t v=0);

  bool as_bool() const;
  uint8_t as_uint8() const;
  uint16_t as_uint12() const;
  int32_t as_int32() const;
  float as_float() const;

  void set_CustomType(uint8_t type, uint16_t val);
  void set_uint8(uint8_t v);
  void set_uint12(uint16_t v);
  void set_int32(int32_t v);
  void set_float(float v);

  Variable ToHeap() const;
  Variable ToWithin() const;

  uint8_t size() const;
  bool is_custom_heap_type() const;

  static uint8_t get_size_from_type(uint8_t type);
};

//}
#endif /* VARIABLE_H_ */
