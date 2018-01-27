#ifndef DATATYPES_H_
#define DATATYPES_H_

#include "../BitsyHeap.h"
#include "../variable.h"

#include <stdint.h>

namespace bitsy_python {

class DataType {
 public:
  static Variable CreateStr(BitsyHeap &heap, const char* str, uint8_t len);
  static Variable CreateForType(BitsyHeap &heap, uint8_t t, uint8_t argcount, Variable args[]);

  static uint16_t Len(BitsyHeap &heap, const Variable& v);

  static Variable GetIndex(BitsyHeap &heap, const Variable& v, uint8_t ind);

  static void SetIndex(BitsyHeap &heap, Variable &v, uint8_t ind, const Variable& val);

  static void Print(BitsyHeap &heap, const Variable& v, void (*print)(char));

  static void updateUsedContainers(uint8_t start_id, const Variable &v, uint32_t *map);

  BitsyHeap::var_id_t id;

 private:
};
}

#endif /* DATATYPES_H_ */
