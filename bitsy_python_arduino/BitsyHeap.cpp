#include "BitsyHeap.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "bitsy_alloc.h"

namespace bitsy_python {

BitsyHeapHeader::BitsyHeapHeader() {
  assert(sizeof(BitsyHeapHeader::Header)==3);
  hdr = NULL;
  len = 0;
  last = 0;
}

#define GET_HDR(id) (((id)%2) ? hdr[(id)/2].v1 : hdr[(id)/2].v0)
#define INC_HDR(id, val) (((id)%2) ? hdr[(id)/2].v1 += (val) : hdr[(id)/2].v0 += (val))

uint8_t BitsyHeapHeader::create(uint8_t size, uint16_t *start) {
  if (last >= len) {
    hdr = (Header *)realloc(hdr, (len+16)*sizeof(BitsyHeapHeader::Header)/2);
    assert(hdr);
    memset(hdr+len/2, 0, 16/2*sizeof(BitsyHeapHeader::Header));
    len += 16;
  }
  *start = last ? GET_HDR(last-1) : 0;
  INC_HDR(last, *start + size);
  return last++;
}

uint8_t BitsyHeapHeader::get(uint8_t id, uint16_t *start, uint16_t *rem) {
  assert(id < last);
  *start = id ? GET_HDR(id-1) : 0;
  if (rem)
    *rem = GET_HDR(last-1)-GET_HDR(id);
  return GET_HDR(id) - *start;
}

void BitsyHeapHeader::extend(uint8_t id, uint16_t increase) {
  assert(id < last);
  for(uint8_t i=id; i<last; i++)
    INC_HDR(i, increase);
}

BitsyHeap::BitsyHeap() {
  bitsy_alloc_init();
}

BitsyHeap::~BitsyHeap() {
  while(bottom_block_size())
    bottom_block_free();
}

BitsyHeap::var_id_t BitsyHeap::CreateVar(uint8_t size, uint8_t **val) {
  uint16_t start;
  uint8_t id = hdr.create(size, &start);
  if(bottom_block_size() <= start + size)
    bottom_block_alloc();
  assert(bottom_block_size() > start + size);
  *val = BLOCKS_END - start - size;
  return id;
}

uint8_t BitsyHeap::GetVar(BitsyHeap::var_id_t id, uint8_t **val) {
  uint16_t start;
  uint8_t len = hdr.get(id, &start);
  *val = BLOCKS_END - start - len;
  return len;
}

uint8_t* BitsyHeap::ExtendVar(BitsyHeap::var_id_t id, uint8_t *val,
                          uint8_t new_size) {
  uint16_t start, rem;
  uint8_t old_size = hdr.get(id, &start, &rem);
  hdr.extend(id, new_size-old_size);
  if(bottom_block_size() <= start + rem + new_size)
    bottom_block_alloc();
  assert(bottom_block_size() > start + rem + new_size);
  memmove(val-rem + old_size - new_size, val-rem, rem+old_size);
  return val+old_size-new_size;
}

void BitsyHeap::FreeVar(var_id_t id) {
  // TODO(rajendrant): implement this.
}
};
