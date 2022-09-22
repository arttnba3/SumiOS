#include <sumios/kernel.h>
#include <sumios/types.h>

void *memset(uint8_t *dst, uint8_t val, uint64_t sz)
{
    for (uint64_t i = 0; i < sz; i++) {
        dst[i] = val;
    }

    return dst;
}

size_t strlen(const char *str)
{
    size_t res = 0;

    while (str[res]) {
        res++;
    }

    return res;
}
