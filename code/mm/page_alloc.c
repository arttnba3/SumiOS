#include <sumios/kernel.h>
#include <sumios/lock.h>
#include <mm/layout.h>
#include <mm/page.h>
#include <mm/types.h>
#include <mm/page_table.h>
#include <mm/page_alloc.h>

struct page *pages = KERNEL_PAGE_ARRAY;

/* the `core` of the buddy system, which should be a cycle linked-list */
struct list_head freelist[MAX_PAGE_ORDER] = { NULL };
spinlock_t freelist_lock;

void freelist_init(void)
{
    for (int i = 0;i < MAX_PAGE_ORDER; i++) {
        list_head_init(&freelist[i]);
    }
}

struct page *next_page_by_order(struct page *p, int order)
{
    if (order >= MAX_PAGE_ORDER) {
        return NULL;
    }

    return p + (1 << order);
}

struct page *prev_page_by_order(struct page *p, int order)
{
    if (order >= MAX_PAGE_ORDER) {
        return NULL;
    }

    return p - (1 << order);
}

/**
 * @brief allocate pages directly from the freelist array.
 * 
 * @param order the order of memory allocation
 * @return struct page* pointer to the page struct, NULL for failed
 */
static struct page *__alloc_page_direct(int order)
{
    struct page *p = NULL;
    int __alloc = order;

    while (__alloc < MAX_PAGE_ORDER) {
        if (!list_empty(&freelist[__alloc])) {
            p = list_entry(freelist[__alloc].next, struct page, list);
            list_del(&p->list);
            break;
        } else {
            __alloc++;
        }
    }

    /* failed */
    if (!p) {
        goto out;
    }

    /* it means that we acquire pages from higher order */
    if (__alloc != order) {
        /* put half pages back to buddy */
        __alloc--;
        while (__alloc >= order) {
            list_add_next(&freelist[__alloc], next_page_by_order(p, __alloc));
            __alloc--;
        }
    }

out:
    return p;
}

static void __reclaim_memory(void)
{

}

/* This is the `heart` of buddy system allocation */
static struct page *__alloc_pages(int order)
{
    struct page *p = NULL;

    if (order >= MAX_PAGE_ORDER) {
        kprintf("[!] invalid memory allocation for order %u\n", order);
        return NULL;
    }

    /* try to alloc directly */
    p = __alloc_page_direct(order);
    if (p) {
        for (int i = 0; i < (1 << order); i++) {
            p[i].ref_count++;
            p[i].is_free = false;
        }
        goto out;
    }

    /* failed to allocate! try to reclaim memory... */
    __reclaim_memory();

out:
    return p;
}

/* This is the `heart` of buddy system free */
static void __free_pages(struct page *p, int order)
{
    if (!p) {
        return;
    }

    if (order >= MAX_PAGE_ORDER) {
        kprintf("[!] invalid memory free for order %u\n", order);
        return;
    }

    /* try to combine nearby pages */
    while (order < (MAX_PAGE_ORDER - 1)) {
        struct page *next, *prev;

        next = next_page_by_order(p, order);
        if ((mm_pgtable_get_va_pte(kern_pgtable, next) != -1) 
            && next->is_free && next->is_head) {
            list_del(&next->list);
            next->is_head = false;
            order++;
            continue;
        }

        prev = prev_page_by_order(p, order);
        if ((mm_pgtable_get_va_pte(kern_pgtable, prev) != -1) 
            && prev->is_free && prev->is_head) {
            list_del(&prev->list);
            p->is_head = false;
            p->is_free = true;
            order++;
            p = prev;
            continue;
        }

        /* we can't combine forward or backward, just break */
        break;
    }

    list_add_next(&(freelist[order]), &p->list);
    p->ref_count = p->map_count = -1;
    p->is_free = p->is_head = true;
}

struct page *alloc_pages(int order)
{
    struct page *p;

    spin_lock(&freelist_lock);

    p = __alloc_pages(order);

    spin_unlock(&freelist_lock);

    return p;
}

void free_pages(struct page *p, int order)
{
    spin_lock(&freelist_lock);
    __free_pages(p, order);
    spin_unlock(&freelist_lock);
}

phys_addr_t mm_phys_alloc(size_t sz)
{
    /* expand the size to page-order aligned */
    struct page *p;
    int order = 0;

    while (sz < (PAGE_SIZE << order)) {
        order++;
    }

    p = alloc_pages(order);

    if (!p) {
        return -1;
    }

    return page_to_phys(p);
}

/**
 * @brief allocate memory from buddy system
 * 
 * @param sz size to allocate, which will be 2^order aligned
 * @return virt_addr_t address of allocated memory, NULL for failed
 */
virt_addr_t mm_alloc(size_t sz)
{
    virt_addr_t physm = mm_phys_alloc(sz);

    if (physm == -1) {
        return NULL;
    } else {
        return PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(physm);
    }
}
