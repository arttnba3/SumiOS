#include <sumios/kernel.h>
#include <mm/layout.h>
#include <mm/page.h>
#include <mm/types.h>
#include <mm/page_table.h>
#include <mm/page_alloc.h>

struct page *pages = KERNEL_PAGE_ARRAY;

/* the `core` of the buddy system, which should be a cycle linked-list */
struct list_head freelist[MAX_PAGE_ORDER] = { NULL };

struct page *next_page_by_order(struct page *p, unsigned int order)
{
    if (order >= MAX_PAGE_ORDER) {
        return NULL;
    }

    return p + (1 << order);
}

/**
 * @brief allocate pages directly from the freelist array.
 * 
 * @param order the order of memory allocation
 * @return struct page* pointer to the page struct, NULL for failed
 */
static inline struct page *__alloc_page_direct(unsigned int order)
{
    struct page *p = NULL;
    int __alloc = order;

    while (__alloc < MAX_PAGE_ORDER) {
        if (freelist[__alloc].next != NULL) {
            p = list_entry(freelist[__alloc].next, struct page, list);
            list_del(&p->list);
            break;
        }

        __alloc++;
    }

    /* failed */
    if (!p) {
        goto out;
    }

    while (__alloc > order) {
        list_add_next(&freelist[__alloc], next_page_by_order(p, __alloc));
        __alloc--;
    }

out:
    return p;
}

/* This is the `heart` of buddy system */
struct page *__alloc_pages(unsigned int order)
{
    struct page *p = NULL;

    if (order >= MAX_PAGE_ORDER) {
        kprintf("[!] invalid memory allocation for order %u\n", order);
        return NULL;
    }

    /* try to alloc directly */
    p = __alloc_page_direct(order);
    if (p) {
        goto out;
    }

    /* failed to allocate! try to reclaim memory... */


out:
    return p;
}

void __free_pages(struct page *p, unsigned int order)
{
    if (order >= MAX_PAGE_ORDER) {
        kprintf("[!] invalid memory free for order %u\n", order);
        return;
    }

    while (order < MAX_PAGE_ORDER) {
        struct page *next = next_page_by_order(p, order);
        if (mm_pgtable_get_va_pte(kern_pgtable, p) == -1) {
            break;
        }
        else if (next->ref_count == -1) {
            list_del(&next->list);
            order++;
        } else {
            break;
        }
    }

    list_add_next(&freelist[order], &p->list);
}

phys_addr_t mm_phys_alloc(size_t sz)
{
    /* expand the size to page-order aligned */

}
