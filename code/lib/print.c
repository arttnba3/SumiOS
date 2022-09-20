#include <sumios/kernel.h>
#include <sumios/print.h>
#include <sumios/stdarg.h>

void kprintstr(const char *str)
{
    for (int i = 0; str[i]; i++) {
        kputchar(str[i]);
    }
}

void kputs(const char *str)
{
    kprintstr(str);
    kputchar('\n');
}

void kprintnum(int64_t n)
{
    /* for 64-bit num it's enough*/
    char n_str[30];
    int idx = 30;

    n_str[--idx] = '\0';

    if (n < 0) {
        kputchar('-');
        n = -n;
    }

    do {
        n_str[--idx] = (n % 10) + '0';
        n /= 10;
    } while (n);
    
    kprintstr(n_str + idx);
}

void kprinthex(uint64_t n)
{
    /* for 64-bit num it's enough*/
    char n_str[30];
    int idx = 30;

    n_str[--idx] = '\0';

    do {
        uint64_t curr = n % 16;
        if (curr >= 10) {
            n_str[--idx] = curr - 10 + 'a';
        } else {
            n_str[--idx] = curr + '0';
        }
        n /= 16;
    } while (n);
    
    kprintstr(n_str + idx);
}

enum {
    PRINT_STATUS_CHAR,
    PRINT_STATUS_MID_SYMBOL,
};

void kprintf(const char *format, ...)
{
    va_list args;
    char ch;
    int status = PRINT_STATUS_CHAR;

    va_start(args, format);

    while ((ch = *format++) != '\0') {
        if (ch != '%') {
            kputchar(ch);
            status = PRINT_STATUS_CHAR;
            continue;
        }

repeat:
        ch = *format++;

        if (ch == '\0') {
            break;
        }

        if (status == PRINT_STATUS_MID_SYMBOL) {
            switch (ch) {
                case 'd':
                    kprintnum(va_arg(args, int64_t));
                    break;
                case 'u':
                    kprintnum(va_arg(args, uint64_t));
                    break;
                case 'x':
                    kprinthex(va_arg(args, uint64_t));
                    break;
                default:
                    break;
            }
        } else {
            switch (ch) {
                case 'c':
                    kputchar(va_arg(args, char));
                    break;
                case 's':
                    kprintstr(va_arg(args, char *));
                    break;
                case 'l':
                    status = PRINT_STATUS_MID_SYMBOL;
                    goto repeat;
                case 'd':
                    kprintnum(va_arg(args, int32_t));
                    break;
                case 'u':
                    kprintnum(va_arg(args, uint32_t));
                    break;
                case 'x':
                    kprinthex(va_arg(args, uint32_t));
                    break;
                case 'p':
                    kprintstr("0x");
                    kprinthex(va_arg(args, uint64_t));
                    break;
                default:
                    break;
            }
        }

        status = PRINT_STATUS_CHAR;
    }

    va_end(args);
}