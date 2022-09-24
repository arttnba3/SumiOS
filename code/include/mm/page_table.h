#ifndef MM_PAGE_TABLE_H
#define MM_PAGE_TABLE_H

#include <sumios/types.h>
#include <mm/types.h>

/* page table types */
typedef size_t pgd_t;
typedef size_t pud_t;
typedef size_t pmd_t;
typedef size_t pte_t;
typedef size_t page_attr_t;

#define PTE_OFFSET 12
#define PMD_OFFSET 21
#define PUD_OFFSET 30
#define PGD_OFFSET 39

#define PT_ENTRY_MASK 0b111111111UL
#define PTE_MASK (PT_ENTRY_MASK << PTE_OFFSET)
#define PMD_MASK (PT_ENTRY_MASK << PMD_OFFSET)
#define PUD_MASK (PT_ENTRY_MASK << PUD_OFFSET)
#define PGD_MASK (PT_ENTRY_MASK << PGD_OFFSET)

#define PTE_ENTRY(addr) ((addr >> PTE_OFFSET) & PT_ENTRY_MASK)
#define PMD_ENTRY(addr) ((addr >> PMD_OFFSET) & PT_ENTRY_MASK)
#define PUD_ENTRY(addr) ((addr >> PUD_OFFSET) & PT_ENTRY_MASK)
#define PGD_ENTRY(addr) ((addr >> PGD_OFFSET) & PT_ENTRY_MASK)

phys_addr_t mm_pgtable_get_va_pte(phys_addr_t pgtable, virt_addr_t va);
void mm_pgtable_map(phys_addr_t pgtable, virt_addr_t va, phys_addr_t pa, 
                    page_attr_t attr);

#endif