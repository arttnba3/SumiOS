#ifndef MM_PAGE_H
#define MM_PAGE_H

#include <sumios/kernel.h>

enum {
    PAGE_RAM, 
    PAGE_RESERVED,
    PAGE_ACPI_RECLAIMABLE,
    PAGE_NVS,
    PAGE_BADRAM,
};

struct page {
    struct page *next;
    size_t flags;
    int ref_count;
    int map_count;
    void **freelist;
};

extern struct page **pages;
extern uint64_t pages_num;

#endif