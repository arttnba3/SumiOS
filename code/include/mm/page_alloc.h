#ifndef MM_PAGE_ALLOC_H
#define MM_PAGE_ALLOC_H

#define MAX_PAGE_ORDER 11

extern struct page *alloc_pages(int order);
extern void free_pages(struct page *p, int order);
extern phys_addr_t mm_phys_alloc(size_t sz);
extern virt_addr_t mm_alloc(size_t sz);

#endif