#ifndef SUMIOS_PRINT_H
#define SUMIOS_PRINT_H

#include <sumios/types.h>

void kputchar(char ch);
void kprintstr(char *str);
void kputs(char *str);
void kprintnum(int64_t n);
void kprinthex(uint64_t n);
void clear_screen(void);

void tty_init(void);

#endif