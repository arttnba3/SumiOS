#include <sumios/kernel.h>
#include <mm/layout.h>
#include <mm/page_types.h>
#include <mm/page_table.h>
#include <mm/page_alloc.h>

struct page *pages = KERNEL_PAGE_ARRAY;
struct page *freelist[MAX_PAGE_ORDER];

/* This is the `heart` of buddy system */
struct page *__alloc_pages(unsigned int order)
{

}

void __free_pages(struct page *p, unsigned int order)
{

}

phys_addr_t mm_phys_alloc(size_t sz)
{

}
