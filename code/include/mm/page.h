#ifndef MM_PAGE_H
#define MM_PAGE_H

#include <sumios/kernel.h>
#include <mm/types.h>

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
 * - page-aligned size
 */
struct page {
    struct list_head list;
    struct {
        unsigned type: 8;
        unsigned is_free: 1;
        unsigned is_head: 1;
    };
    int ref_count;
    int map_count;
    union {
        void **freelist;
        void *virtual;
    };
    /* unused area to make it page-aligned, maybe we can put sth else there? */
    size_t unused[3];
} __attribute__((aligned(16)));

extern struct page *pages;
extern uint64_t pages_num;

static inline phys_addr_t page_to_phys(struct page *p)
{
    return (phys_addr_t)(p - (struct page *)KERNEL_PAGE_ARRAY) * PAGE_SIZE;
}

static inline struct page *phys_to_page(phys_addr_t pa)
{
    return &(pages[pa / PAGE_SIZE]);
}

#endif