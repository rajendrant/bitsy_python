#ifndef BITSY_HEAP_H_
#define BITSY_HEAP_H_

#include <stdint.h>
#include "bitsylimit.h"

namespace bitsy_python {

class BitsyHeapHeader {
public:
  BitsyHeapHeader();
  uint8_t create(uint8_t size, uint16_t *start);
  uint8_t get(uint8_t id, uint16_t *start, uint16_t *rem=0);
  void extend(uint8_t id, uint16_t increase);
private:
  struct Header {
    uint16_t v0 : 12;
    uint16_t v1 : 12;
  }__attribute__((packed)) *hdr;
  uint8_t len, last;
};

class BitsyHeap {
 public:
  typedef uint8_t var_id_t;

  BitsyHeap();
  ~BitsyHeap();

  var_id_t CreateVar(uint8_t size, uint8_t **val);
  uint8_t GetVar(var_id_t id, uint8_t **val);
  uint8_t* ExtendVar(var_id_t id, uint8_t *val, uint8_t new_size);
  void FreeVar(var_id_t id);

 private:
  BitsyHeapHeader hdr;
};

}
#endif /* BITSY_HEAP_H_ */
