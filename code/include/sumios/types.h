#ifndef SUMIOS_TYPES_H
#define SUMIOS_TYPES_H

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