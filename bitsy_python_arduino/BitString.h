#ifndef BITSTRING_H_
#define BITSTRING_H_

#include "bitsylimit.h"

#include <stdint.h>

namespace bitsy_python {

class BitString {
public:
#ifdef DESKTOP
	BitString(uint8_t *buf, const uint16_t size): _buf(buf), size(size) {}
#else
	BitString() {}
#endif

	uint8_t get_bit8(uint16_t pos, uint8_t len) const;
	uint16_t get_bit16(uint16_t pos, uint8_t len) const;
	uint32_t get_bit32(uint16_t pos, uint8_t len) const;

private:
#ifdef DESKTOP
	const uint8_t *_buf;
	const uint16_t size;
#endif
};

}
#endif /* BITSTRING_H_ */
