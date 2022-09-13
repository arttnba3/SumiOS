#include <sumios/kernel.h>
#include <sumios/types.h>

void *memset(uint8_t *dst, uint64_t sz, uint8_t val)
{
    for (uint64_t i = 0; i < sz; i++) {
        dst[i] = val;
    }

    return dst;
}