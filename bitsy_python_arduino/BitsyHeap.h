#ifndef BITSY_HEAP_H_
#define BITSY_HEAP_H_

#include <stdint.h>
#include "bitsylimit.h"

namespace bitsy_python {

class BitsyHeap {
 public:
  typedef uint8_t var_id_t;

  BitsyHeap();
  var_id_t CreateVar(uint8_t size, uint8_t **val);
  uint8_t GetVar(var_id_t id, uint8_t **val);
  bool ExtendVar(var_id_t id, uint8_t *val, uint8_t new_size);
  void FreeVar(var_id_t id);

 private:
  bool hdr_extend();

  uint8_t *hdr;
  uint8_t hdr_len, hdr_last;
  uint8_t *mem;
  uint16_t mem_len;
};
};

#endif /* BITSY_HEAP_H_ */
