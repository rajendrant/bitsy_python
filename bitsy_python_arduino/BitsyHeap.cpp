#include "BitsyHeap.h"

#include <stdlib.h>
#include <string.h>

#include "bitsy_alloc.h"
#include "gc.h"

namespace bitsy_python {

namespace BitsyHeap {

#define GET_HDR(id) (((id)%2) ? hdr[(id)/2].v1 : hdr[(id)/2].v0)
#define INC_HDR(id, val) (((id)%2) ? hdr[(id)/2].v1 += (val) : hdr[(id)/2].v0 += (val))

uint8_t len, last, free_id;
struct Header {
  uint16_t v0 : 12;
  uint16_t v1 : 12;
}__attribute__((packed)) *hdr;

var_id_t HdrCreate(uint8_t size, uint16_t *start) {
  if (last >= len) {
    hdr = (Header *)realloc(hdr, (len+16)*sizeof(Header)/2);
    BITSY_ASSERT(hdr);
    memset(hdr+len/2, 0, 16/2*sizeof(Header));
    len += 16;
  }
  *start = last ? GET_HDR(last-1) : 0;
  INC_HDR(last, *start + size);
  return last++;
}

uint8_t HdrGet(var_id_t id, uint16_t *start, uint16_t *rem=NULL) {
  BITSY_ASSERT(id < last);
  *start = id ? GET_HDR(id-1) : 0;
  if (rem)
    *rem = GET_HDR(last-1)-GET_HDR(id);
  return GET_HDR(id) - *start;
}

void HdrExtend(var_id_t id, int16_t increase) {
  BITSY_ASSERT(id < last);
  for(uint8_t i=id; i<last; i++)
    INC_HDR(i, increase);
}

void init() {
  hdr = NULL;
  len = 0;
  last = 0;
  free_id = INVALID_VARID;
}

void destroy() {
  while(bottom_block_size())
    bottom_block_free();
}

uint8_t freeID() { return free_id; }
uint8_t lastID() { return last; }

var_id_t CreateVar(uint8_t size, uint8_t **val) {
  uint16_t start;
  uint8_t id;

  BITSY_ASSERT(size>0);

  // Attempt to garbage collect before creating a new heap variable.
  gc();

  if (free_id != INVALID_VARID) {
    // TODO(rajendrant): Use the free ids only after certain (>100) variables
    // have been used.
    id = free_id;
    uint8_t old_size = GetVar(id, val);
    free_id = **val;
    if (old_size < size) {
      *val = ExtendVar(id, *val, size);
    } else if (old_size > size) {
      // Shrink var.
      uint16_t rem;
      HdrExtend(id, size-old_size);
      HdrGet(id, &start, &rem);
      GetVar(id, val);
      memmove(*val-rem, *val-rem - old_size + size, rem);
    }
  } else {
    id = HdrCreate(size, &start);
    if(bottom_block_size() <= start + size)
      bottom_block_alloc();
    BITSY_ASSERT(bottom_block_size() > start + size);
    *val = BLOCKS_END - start - size;
  }
  return id;
}

uint8_t GetVar(var_id_t id, uint8_t **val) {
  uint16_t start;
  uint8_t len = HdrGet(id, &start);
  *val = BLOCKS_END - start - len;
  return len;
}

uint8_t* ExtendVar(var_id_t id, uint8_t *val,
                          uint8_t new_size) {
  BITSY_ASSERT(new_size>0);

  uint16_t start, rem;
  uint8_t old_size = HdrGet(id, &start, &rem);
  HdrExtend(id, new_size-old_size);
  if(bottom_block_size() <= start + rem + new_size)
    bottom_block_alloc();
  BITSY_ASSERT(bottom_block_size() > start + rem + new_size);
  memmove(val-rem + old_size - new_size, val-rem, rem+old_size);
  return val+old_size-new_size;
}

void FreeVar(var_id_t id) {
  BITSY_ASSERT(id < last);
  // Create a link list of free variable ids.
  uint8_t *ptr;
  GetVar(id, &ptr);
  ptr[0] = free_id;
  free_id = id;
}

uint32_t getFreeIDMap(uint8_t start_id) {
  uint32_t map = 0;
  for(var_id_t id=free_id; id != INVALID_VARID;) {
    if (id >= start_id && id < start_id+32)
      map |= 0x1<<(id-start_id);
    uint8_t *ptr;
    GetVar(id, &ptr);
    id = ptr[0];
  }
  for(var_id_t id=last; id>=start_id && id<start_id+32; id++)
    map |= 0x1<<(id-start_id);
  return map;
}

}
}
