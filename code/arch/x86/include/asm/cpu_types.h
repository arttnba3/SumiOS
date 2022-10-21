/*
 * X86 architecture related macros.
 * Note: because it's just my toy OS so only a little macros there :)
 */

#ifndef X86_CPU_TYPES_H
#define X86_CPU_TYPES_H

/* CR0 */
#define CR0_PG (1 << 31)

/* CR4 */
#define CR4_PAE (1 << 5)
#define CR4_PGE (1 << 7)

/* Segment selector */
#define SELECTOR_RPL (0)
#define SELECTOR_TI (1)
#define SELECTOR_INDEX (3)

#endif