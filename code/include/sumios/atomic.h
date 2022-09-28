#ifndef SUMIOS_ATOMIC_H
#define SUMIOS_ATOMIC_H

#include <sumios/types.h>

typedef uint32_t atomic_t;

/**
 * if the *ptr == oldval, it'll be set to newval, the return val will be true,
 * otherwise it'll return false and nothing got changed
 */
#define atomic_compare_and_swap(ptr, oldval, newval) \
    __sync_bool_compare_and_swap(ptr, oldval, newval)

#define atomic_set(ptr, newval) __sync_lock_test_and_set(ptr, newval)

#endif