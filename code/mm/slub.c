#include <sumios/kernel.h>
#include <mm/slub.h>

static void *__kmalloc(size_t sz)
{

}

static void *__kmalloc_large(size_t sz)
{

}

void *kmalloc(size_t sz)
{
    if (sz > SLUB_MAX_CACHE_SIZE) {
        return __kmalloc_large(sz);
    }

    
}

void kfree(void* object)
{

}