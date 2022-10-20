#ifndef SUMIOS_STRING_H
#define SUMIOS_STRING_H

#include <sumios/kernel.h>

void *memset(uint8_t *dst, uint8_t val, uint64_t sz);
void *memcpy(uint8_t *dst, uint8_t *src, uint64_t sz);
size_t strlen(const char *str);

#endif