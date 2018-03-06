#include "ByteStack.h"

#include <string.h>

namespace bitsy_python {

void ByteStack::init() {
  BlockStack::init();
  AllocTopBlock();
  _top_byte = 0;
}

void ByteStack::pushBytes(const uint8_t* src, uint8_t len) {
  if (_top_byte + len >= blocksize()) {
    for (uint8_t i = 0; i < len; i++) {
      pushByte(src[i]);
    }
  } else {
    memcpy(top() + _top_byte, src, len);
    _top_byte += len;
  }
}

void ByteStack::pushByte(uint8_t src) {
  top()[_top_byte] = src;
  _top_byte++;
  if (_top_byte == blocksize()) {
    _top_byte = 0;
    AllocTopBlock();
  }
}

void ByteStack::popBytes(uint8_t* dst, uint8_t len) {
  if (_top_byte < len) {
    while (len--) {
      dst[len] = popByte();
    }
  } else {
    _top_byte -= len;
    memcpy(dst, top() + _top_byte, len);
  }
}

uint8_t ByteStack::popByte() {
  if (_top_byte == 0) {
    FreeTopBlock();
    _top_byte = blocksize() - 1;
  } else {
    _top_byte--;
  }
  return top()[_top_byte];
}

bool ByteStack::getPrevByte(uint8_t *p, uint8_t **bytes, uint8_t *len) const {
  if(!getPrevBlock(p, bytes))
    return false;
  *len = (*bytes == top()) ? _top_byte : blocksize();
  return true;
}

}
