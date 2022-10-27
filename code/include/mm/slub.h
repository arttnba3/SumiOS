#ifndef MM_SLUB_H
#define MM_SLUB_H

#include <sumios/types.h>
#include <sumios/lock.h>
#include <mm/page.h>

/* we can only allocate max 8 pages from slub at a time */
#define SLUB_MAX_SHIFT 3
#define SLUB_MAX_CACHE_SIZE (PAGE_SIZE << SLUB_MAX_SHIFT)

struct kmem_cache {
    spinlock_t lock;
    size_t obj_size;
    struct list_head full;      /* list of full pages */
    struct list_head partial;   /* list of partial pages */
    struct list_head list;      /* list of slub caches */
    struct page *slub;          /* current slub page */
    void **freelist;            /* current freelist */
};

extern void *kmalloc(size_t sz);
extern void kfree(void* object);

#endif