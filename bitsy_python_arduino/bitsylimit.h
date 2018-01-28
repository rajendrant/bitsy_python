#define BYTES_PER_BLOCK 80
#define TOTAL_BLOCKS 15
#define BITS_PER_BLOCK_ID 4

#ifdef ARDUINO
#else
#define DESKTOP
#endif

#ifdef DESKTOP
#include <assert.h>
#define BITSY_ASSERT(c) assert(c)
#else
#define BITSY_ASSERT(c) if(c);
#endif
