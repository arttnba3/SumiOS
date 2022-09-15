#ifndef SUMIOS_STRING_H
#define SUMIOS_STRING_H

#include <sumios/kernel.h>

void *memset(uint8_t *dst, uint64_t sz, uint8_t val);
size_t strlen(const char *str);

#endif