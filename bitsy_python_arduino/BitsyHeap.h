#ifndef BITSY_HEAP_H_
#define BITSY_HEAP_H_

#include <stdint.h>
#include "bitsylimit.h"

#define INVALID_VARID 0xFF

namespace bitsy_python {

class BitsyHeap {
 public:
  typedef uint8_t var_id_t;

  BitsyHeap();
  ~BitsyHeap();

  var_id_t CreateVar(uint8_t size, uint8_t **val);
  uint8_t GetVar(var_id_t id, uint8_t **val) const;
  uint8_t* ExtendVar(var_id_t id, uint8_t *val, uint8_t new_size);
  void FreeVar(var_id_t id);
  uint32_t getFreeIDMap(uint8_t start_id) const;
  uint8_t lastID() const { return last; }

 private:
  friend class GCTest;

  uint8_t HdrCreate(uint8_t size, uint16_t *start);
  uint8_t HdrGet(uint8_t id, uint16_t *start, uint16_t *rem=0) const;
  void HdrExtend(uint8_t id, int16_t increase);

  uint8_t len, last, free_id;
  struct Header {
    uint16_t v0 : 12;
    uint16_t v1 : 12;
  }__attribute__((packed)) *hdr;
};

}
#endif /* BITSY_HEAP_H_ */
