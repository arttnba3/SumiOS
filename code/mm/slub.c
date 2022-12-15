#include <sumios/kernel.h>
#include <mm/slub.h>
#include <mm/page_alloc.h>
#include <mm/layout.h>

struct kmem_cache kmem_cache_list[NR_KOBJECT_SIZE_TYPE];

static struct page *slub_alloc(struct kmem_cache *kc)
{
    struct page *new_slub;
    virt_addr_t slub_addr;
    virt_addr_t next_obj;
    virt_addr_t obj_ptr;
 
    size_t alloc_size = PAGE_SIZE << (kc->obj_size >> PAGE_SHIFT);

    new_slub = alloc_pages(kc->obj_size >> PAGE_SHIFT);
    
    /* trim pages into chunks */
    if (new_slub) {
        slub_addr = PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(page_to_phys(new_slub));
        new_slub->freelist = (void**) slub_addr;

        for (size_t va = kc->obj_size; va < alloc_size; va += kc->obj_size) {
            next_obj = slub_addr + va;
            obj_ptr = next_obj - kc->obj_size;
            *(virt_addr_t*) obj_ptr = next_obj;
        }

        *(virt_addr_t*) next_obj = NULL;
    }

    return new_slub;
}

void kmem_cache_init(void)
{
    size_t orig_sz = 16;
    for (int i = 0; i < NR_KOBJECT_SIZE_TYPE; i++) {
        struct kmem_cache *kc = &kmem_cache_list[i];
        
        /* list for partial and full used pages */
        list_head_init(&kc->partial);
        list_head_init(&kc->full);

        /* list for kmem_caches' linked */
        list_head_init(&kc->list);

        /* list for current allocation */
        kc->freelist = NULL;
        kc->slub = NULL;

        /* size for each kmem_cache, times 2 each time */
        kc->obj_size = orig_sz;
        orig_sz *= 2;
    }
}

static inline int slub_index(size_t sz)
{
    if (sz <= 16) {
        return KOBJECT_16;
    } else if (sz <= 32) {
        return KOBJECT_32;
    } else if (sz <= 64) {
        return KOBJECT_64;
    } else if (sz <= 128) {
        return KOBJECT_128;
    } else if (sz <= 256) {
        return KOBJECT_256;
    } else if (sz <= 512) {
        return KOBJECT_512;
    } else if (sz <= 1024) {
        return KOBJECT_1K;
    } else if (sz <= 2048) {
        return KOBJECT_2K;
    } else if (sz <= 4096) {
        return KOBJECT_4K;
    } else if (sz <= 8192) {
        return KOBJECT_8K;
    } else {
        return -1;
    }
}

/**
 * @brief Fetch an object from a specific kmem_cache
 * 
 * @return void* allocated object, NULL for failure
 */
static void *kmem_cache_alloc(struct kmem_cache *kc)
{
    void *obj = NULL;
    struct page *slub;

    if (!kc) {
        return NULL;
    }

    spin_lock(&kc->lock);

redo:
    /* we have objects on the kmem_cache now, just allocate one */
    if (kc->freelist != NULL) {
        obj = kc->freelist;
        kc->freelist = *kc->freelist;
        goto out;
    }

    /* no object on freelist, put the slub page on the full list */
    if (kc->slub) {
        list_add_next(&kc->full, &kc->slub->list);
        kc->slub = NULL;
    }

    /* try to get the page from partial list */
    if (!list_empty(&kc->partial)) {
        kc->slub = container_of(kc->partial.next, struct page, list);
        kc->freelist = kc->slub->freelist;
        list_del(&kc->slub->list);
        goto redo;
    }

    /* no slub on the partial list, allocated from the buddy */
    kc->slub = slub_alloc(kc);
    if (kc->slub) {
        kc->freelist = kc->slub->freelist;
        goto redo;
    }

out:
    spin_unlock(&kc->lock);
    return obj;
}

static void *__kmalloc(size_t sz)
{
    struct kmem_cache *kc;
    int index;
    void *obj;

    index = slub_index(sz);
    if (index < 0) {
        goto out;
    }

    kc = &kmem_cache_list[index];
    obj = kmem_cache_alloc(kc);

out:
    return obj;
}

static void *__kmalloc_large(size_t sz)
{
    /* not complete yet */
    return NULL;
}

void *kmalloc(size_t sz)
{
    if (sz > KMALLOC_MAX_CACHE_SIZE) {
        return __kmalloc_large(sz);
    }

    return __kmalloc(sz);
}

void kfree(void* object)
{

}