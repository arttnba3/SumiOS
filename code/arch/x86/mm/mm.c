#include <sumios/kernel.h>
#include <mm/mm.h>
#include <mm/layout.h>

phys_addr_t virt_to_phys(virt_addr_t pgtable, virt_addr_t va)
{
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    int pgd_i = PGD_ENTRY(va);
    int pud_i = PUD_ENTRY(va);
    int pmd_i = PMD_ENTRY(va);
    int pte_i = PTE_ENTRY(va);


}