#ifndef SUMIOS_KERNEL_H
#define SUMIOS_KERNEL_H

#include <sumios/align.h>
#include <sumios/print.h>
#include <sumios/string.h>
#include <sumios/types.h>
#include <sumios/list.h>

#include <mm/slub.h>

#ifndef offsetof
#define offsetof(type, member) ((size_t) &(((type*)NULL)->member))
#endif

#define container_of(ptr, type, member) \
        (((size_t)ptr) - offsetof(type, member))

#endif