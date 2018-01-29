#ifndef BITSY_HEAP_H_
#define BITSY_HEAP_H_

#include <stdint.h>
#include "bitsylimit.h"

#define INVALID_VARID 0xFF

namespace bitsy_python {

namespace BitsyHeap {

typedef uint8_t var_id_t;

void init();
void destroy();

var_id_t CreateVar(uint8_t size, uint8_t **val);
uint8_t GetVar(var_id_t id, uint8_t **val);
uint8_t* ExtendVar(var_id_t id, uint8_t *val, uint8_t new_size);
void FreeVar(var_id_t id);
uint32_t getFreeIDMap(uint8_t start_id);

uint8_t freeID();
uint8_t lastID();

}
}
#endif /* BITSY_HEAP_H_ */
