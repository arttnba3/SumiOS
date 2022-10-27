#ifndef SUMIOS_TYPES_H
#define SUMIOS_TYPES_H

#include <mm/page_types.h>

typedef unsigned char   uint8_t;
typedef signed char     int8_t;
typedef unsigned short  uint16_t;
typedef signed short    int16_t;
typedef unsigned int    uint32_t;
typedef signed int      int32_t;
typedef unsigned long long  uint64_t;
typedef signed long long    int64_t;

typedef unsigned long   size_t;
typedef signed long     ssize_t;

typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;

typedef __u8 u8;
typedef __u16 u16;
typedef __u32 u32;
typedef __u64 u64;

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef bool
#define bool uint8_t
#endif

#endif