#include <sumios/kernel.h>
#include <mm/mm.h>
#include <mm/layout.h>

phys_addr_t kern_pgtable;

/* Get the PTE of a specific virtual address */
phys_addr_t mm_pgtable_get_va_pte(phys_addr_t pgtable, virt_addr_t va)
{
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    int pgd_i = PGD_ENTRY(va);
    int pud_i = PUD_ENTRY(va);
    int pmd_i = PMD_ENTRY(va);
    int pte_i = PTE_ENTRY(va);

    pgd = PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(pgtable);
    if (!pgd[pgd_i]) {
        return -1;
    }
    pud = PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(pgd[pgd_i] & PAGE_MASK);
    if (!pud[pud_i]) {
        return -1;
    }
    pmd = PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(pud[pud_i] & PAGE_MASK);
    if (!pmd[pmd_i]) {
        return -1;
    }
    pte = PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(pmd[pmd_i] & PAGE_MASK);
    /* we ONLY map this address to zero page */
    if (!(pte[pte_i] & PAGE_MASK) && !KERNEL_VA_IS_PHYS_ZERO(va)) {
        return -1;
    }

    return pte[pte_i] & PAGE_MASK;
}

/* map for single page */
void mm_pgtable_map(phys_addr_t pgtable, virt_addr_t va, phys_addr_t pa, 
                    page_attr_t attr)
{
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    int pgd_i = PGD_ENTRY(va);
    int pud_i = PUD_ENTRY(va);
    int pmd_i = PMD_ENTRY(va);
    int pte_i = PTE_ENTRY(va);

    /**
     * don't FORGET to set the attr for each level's entry!
     * don't FORGET to access it by the virtual address!
     */
    pgd = PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(pgtable);
    if (!pgd[pgd_i]) {
        pgd[pgd_i] = mm_phys_alloc_linear(PAGE_SIZE);
        memset(PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(pgd[pgd_i]), 0, PAGE_SIZE);
        pgd[pgd_i] |= attr;
    }
    pud = PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(pgd[pgd_i] & PAGE_MASK);
    if (!pud[pud_i]) {
        pud[pud_i] = mm_phys_alloc_linear(PAGE_SIZE);
        memset(PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(pud[pud_i]), 0, PAGE_SIZE);
        pud[pud_i] |= attr;
    }
    pmd = PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(pud[pud_i] & PAGE_MASK);
    if (!pmd[pmd_i]) {
        pmd[pmd_i] = mm_phys_alloc_linear(PAGE_SIZE);
        memset(PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(pmd[pmd_i]), 0, PAGE_SIZE);
        pmd[pmd_i]|= attr;
    }
    pte = PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(pmd[pmd_i] & PAGE_MASK);
    pte[pte_i] = pa | attr;
}