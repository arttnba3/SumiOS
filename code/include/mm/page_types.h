#ifndef MM_PAGE_TYPES_H
#define MM_PAGE_TYPES_H

/* page size */
#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE - 1))

/* page attributes */
#define PAGE_ATTR_P (1)
#define PAGE_ATTR_RW (1 << 1)

#endif