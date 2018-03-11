#include "ByteStack.h"

#include <string.h>

namespace bitsy_python {
namespace ByteStack {

uint8_t _top_byte = 0;

void init() {
  BlockStack::init();
  BlockStack::AllocTopBlock();
  _top_byte = 0;
}

void pushBytes(const uint8_t* src, uint8_t len) {
  if (_top_byte + len >= BlockStack::blocksize()) {
    for (uint8_t i = 0; i < len; i++) {
      pushByte(src[i]);
    }
  } else {
    memcpy(BlockStack::top() + _top_byte, src, len);
    _top_byte += len;
  }
}

void pushByte(uint8_t src) {
  BlockStack::top()[_top_byte] = src;
  _top_byte++;
  if (_top_byte == BlockStack::blocksize()) {
    _top_byte = 0;
    BlockStack::AllocTopBlock();
  }
}

void popBytes(uint8_t* dst, uint8_t len) {
  if (_top_byte < len) {
    while (len--) {
      dst[len] = popByte();
    }
  } else {
    _top_byte -= len;
    memcpy(dst, BlockStack::top() + _top_byte, len);
  }
}

uint8_t popByte() {
  if (_top_byte == 0) {
    BlockStack::FreeTopBlock();
    _top_byte = BlockStack::blocksize() - 1;
  } else {
    _top_byte--;
  }
  return BlockStack::top()[_top_byte];
}

bool getPrevByte(uint8_t *p, uint8_t **bytes, uint8_t *len) {
  if(!BlockStack::getPrevBlock(p, bytes))
    return false;
  *len = (*bytes == BlockStack::top()) ? _top_byte : BlockStack::blocksize();
  return true;
}

}
}
