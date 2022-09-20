/* 
 * SumiOS kernel memory layout
 * 
 * Copyright (c) 2022 arttnba3 <arttnba@gmail.com>
 */

#ifndef MM_LAYOUT_H
#define MM_LAYOUT_H

#include <sumios/kernel.h>

#define REALMODE_TEXT_MODE_ADDR 0xB8000
#define REALMODE_TEXT_MODE_END  0xB8FFF

#define KERNEL_IMAGE_PHYS_BASE 0x100000

#define KERNEL_PAGE_ARRAY 0xffff800000000000UL
#define KERNEL_PAGE_ARRAY_END 0xffff87ffffffffffUL
#define KERNEL_DIRECT_MAPPING_AREA 0xffff888000000000UL
#define KERNEL_BASE_ADDR 0xffffffff81000000UL

#define PHYS_TO_KERNEL_DIRECT_MAPPING_ADDR(x) \
        ((uint64_t) x + KERNEL_DIRECT_MAPPING_AREA)

#endif