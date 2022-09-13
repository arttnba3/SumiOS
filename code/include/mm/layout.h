/* 
 * SumiOS kernel memory layout
 * 
 * Copyright (c) 2022 arttnba3 <arttnba@gmail.com>
 */

#define REALMODE_TEXT_MODE_ADDR 0xB8000
#define REALMODE_TEXT_MODE_END  0xB8FFF

#define KERNEL_IMAGE_PHYS_BASE 0x100000

#define KERNEL_BASE_ADDR 0xffffffff81000000