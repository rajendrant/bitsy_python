#ifndef DATATYPES_H_
#define DATATYPES_H_

#include "../BitsyHeap.h"
#include "../variable.h"

#include <stdint.h>

namespace bitsy_python {

class DataType {
 public:
  static Variable CreateStr(const char* str, uint8_t len);
  static Variable CreateForType(uint8_t t, uint8_t argcount, Variable args[]);

  static uint16_t Len(const Variable& v);

  static Variable GetIndex(const Variable& v, uint8_t ind);

  static void SetIndex(Variable &v, uint8_t ind, const Variable& val);

  static void Print(const Variable& v, void (*print)(char));

  static void updateUsedContainers(uint8_t start_id, const Variable &v, uint32_t *map);

  static bool InOperator(const Variable& cont, const Variable& e);

  BitsyHeap::var_id_t id;

 private:
};
}

#endif /* DATATYPES_H_ */
