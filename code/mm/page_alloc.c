#include <sumios/kernel.h>
#include <mm/layout.h>
#include <mm/page_types.h>
#include <mm/page_table.h>

struct page *pages = KERNEL_PAGE_ARRAY;


/* This is the `heart` of buddy system */
struct page *__alloc_pages(unsigned int order)
{

}