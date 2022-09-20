#ifndef SUMIOS_STDARG_H
#define SUMIOS_STDARG_H

typedef __builtin_va_list va_list;
#define va_start(v, l)  __builtin_va_start(v, l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v, T)    __builtin_va_arg(v, T)
#define va_copy(d, S)   __builtin_va_copy(d, S)

#endif