#ifndef MM_PAGE_H
#define MM_PAGE_H

#include <sumios/kernel.h>

enum {
    PAGE_RAM = 1, 
    PAGE_RESERVED,
    PAGE_ACPI_RECLAIMABLE,
    PAGE_NVS,
    PAGE_BADRAM,
};

/**
 * struct page
 * - representing an existed physical page frame
 */
struct page {
    struct page *next;
    size_t flags;
    int ref_count;
    int map_count;
    union {
        void **freelist;
        void *virtual;
    };
};

extern struct page *pages;
extern uint64_t pages_num;

#endif