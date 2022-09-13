#ifndef SUMIOS_ALIGN_H
#define SUMIOS_ALIGN_H

/* align up, @n should be a pow of 2 */
#define ALIGN(x, n) ((x+(n-1))&(~(n-1)))

#endif