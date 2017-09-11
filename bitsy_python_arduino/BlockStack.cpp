#include "BlockStack.h"

#include <assert.h>
#include <stdio.h>

namespace bitsy_python {

typedef struct {
	uint8_t bytes[BYTES_PER_BLOCK];
	uint8_t next : BITS_PER_BLOCK_ID;
	uint8_t prev : BITS_PER_BLOCK_ID;
} Block;

#define INVALID_BLOCK 0xF

Block blocks[TOTAL_BLOCKS];

uint8_t freelist = INVALID_BLOCK;

void init() {
	if (freelist != INVALID_BLOCK)
		return;
	for(uint8_t i=0; i<TOTAL_BLOCKS; i++) {
		blocks[i].next = i+1;
		blocks[i].prev = i-1;
	}
	blocks[0].prev = INVALID_BLOCK;
	blocks[TOTAL_BLOCKS-1].next = INVALID_BLOCK;
	freelist = 0;
}

uint8_t alloc_block() {
	uint8_t next = freelist;
	freelist = blocks[next].next;
	blocks[next].next = INVALID_BLOCK;
	assert(blocks[next].prev == INVALID_BLOCK);
	blocks[freelist].prev = INVALID_BLOCK;
	return next;
}

void free_block(uint8_t tofree) {
	blocks[tofree].prev = INVALID_BLOCK;
	blocks[tofree].next = freelist;
	blocks[freelist].prev = tofree;
	freelist = tofree;
}

BlockStack::BlockStack() {
	init();
	_top = INVALID_BLOCK;
}

void BlockStack::AllocTopBlock() {
	uint8_t next = alloc_block();
	blocks[next].prev = _top;
	blocks[_top].next = next;
	_top = next;
}

void BlockStack::FreeTopBlock() {
	uint8_t tofree = _top;
	_top = blocks[tofree].prev;
	free_block(tofree);
	blocks[_top].next = INVALID_BLOCK;
}

uint8_t* BlockStack::top() const {
	assert(_top != INVALID_BLOCK);
	return blocks[_top].bytes;
}

uint8_t BlockStack::GetSizeForTesting() const {
	uint8_t size = 0, size2 = 0, last = INVALID_BLOCK;
	for(uint8_t curr=_top; curr!=INVALID_BLOCK; curr=blocks[curr].prev) {
		size++;
		last = curr;
	}
	if (size>0) {
		assert(blocks[_top].next == INVALID_BLOCK);
		assert(blocks[last].prev == INVALID_BLOCK);
	}
	for(uint8_t curr=last; curr!=INVALID_BLOCK; curr=blocks[curr].next) {
		size2++;
	}
	assert(size==size2);
	return size;
}

uint8_t BlockStack::blocksize() const {
	return BYTES_PER_BLOCK;
}

}
