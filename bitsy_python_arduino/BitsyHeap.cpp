#include "BitsyHeap.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

namespace bitsy_python {

uint8_t *heap_hdr = NULL;
uint8_t heap_hdr_len = 0;

BitsyHeap::BitsyHeap() {
  hdr = NULL;
  hdr_len = 0;
  hdr_last = 0;
  mem = (uint8_t *)malloc(6400);
  mem_len = 6400;
}

bool BitsyHeap::hdr_extend() {
  hdr_len += 16;
  hdr = (uint8_t *)realloc(hdr, hdr_len);
  if (!hdr) {
    hdr_len = 0;
    return false;
  }
  return true;
}

BitsyHeap::var_id_t BitsyHeap::CreateVar(uint8_t size, uint8_t **val) {
  if (hdr_last >= hdr_len) assert(hdr_extend());
  uint16_t start = 0;
  for (uint8_t i = 0; i < hdr_last; i++) start += hdr[i];
  *val = mem + start;
  hdr[hdr_last] = size;
  assert(mem_len >= start + size);
  return hdr_last++;
}

uint8_t BitsyHeap::GetVar(BitsyHeap::var_id_t id, uint8_t **val) {
  if (id >= hdr_last) return 0;
  uint16_t start = 0;
  for (uint8_t i = 0; i < id; i++) start += hdr[i];
  *val = mem + start;
  return hdr[id];
}

bool BitsyHeap::ExtendVar(BitsyHeap::var_id_t id, uint8_t *val,
                          uint8_t new_size) {
  if (id >= hdr_last) return false;
  uint16_t start = val - mem;
  uint16_t rem = 0;
  for (uint8_t i = id + 1; i < hdr_last; i++) rem += hdr[i];
  memmove(val + new_size, val + hdr[id], rem);
  hdr[id] = new_size;
  assert(mem_len >= start + rem + new_size);
  return true;
}

void BitsyHeap::FreeVar(var_id_t id) {
  // TODO(rajendrant): implement this.
}
};
