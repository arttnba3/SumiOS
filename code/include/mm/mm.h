#ifndef MM_MM_H
#define MM_MM_H

#include <mm/page_types.h>
#include <mm/page_table.h>
#include <mm/layout.h>
#include <mm/page.h>
#include <mm/types.h>

struct page *__alloc_pages(unsigned int order);
void __free_pages(struct page *p, unsigned int order);
void freelist_init(void);

#endif