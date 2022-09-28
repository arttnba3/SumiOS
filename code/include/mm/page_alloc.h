#ifndef MM_PAGE_ALLOC_H
#define MM_PAGE_ALLOC_H

#define MAX_PAGE_ORDER 11

struct page *alloc_pages(unsigned int order);
void free_pages(struct page *p, unsigned int order);

phys_addr_t mm_phys_alloc(size_t sz);
virt_addr_t mm_alloc(size_t sz);

#endif