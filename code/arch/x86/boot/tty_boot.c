#include <graphics/vga.h>
#include <sumios/types.h>
#include <asm/io.h>

uint16_t *boot_text_mem;
uint16_t boot_cursor_loc;
uint16_t boot_display_mode;
int boot_serial_exists;

void boot_putchar(char ch);

static inline uint16_t boot_get_cursor_loc_blackwhite(void)
{
    uint16_t cursor_loc;
    uint8_t cursor_loc_low, cursor_loc_high;

    outb(CRTC_ADDRESS_REGISTER_BLACKWHITE, CRTC_DATA_CURSOR_LOC_HIGH);
    cursor_loc_high = inb(CRTC_DATA_REGISTER_BLACKWHITE);
    outb(CRTC_ADDRESS_REGISTER_BLACKWHITE, CRTC_DATA_CURSOR_LOC_LOW);
    cursor_loc_low = inb(CRTC_DATA_REGISTER_BLACKWHITE);

    cursor_loc = (cursor_loc_high << 8) | cursor_loc_low;

    return cursor_loc;
}

static inline void boot_set_cursor_loc_blackwhite(uint16_t cursor_loc)
{
    outb(CRTC_ADDRESS_REGISTER_BLACKWHITE, CRTC_DATA_CURSOR_LOC_HIGH);
    outb(CRTC_DATA_REGISTER_BLACKWHITE, cursor_loc >> 8);
    outb(CRTC_ADDRESS_REGISTER_BLACKWHITE, CRTC_DATA_CURSOR_LOC_LOW);
    outb(CRTC_DATA_REGISTER_BLACKWHITE, cursor_loc & 0xFF);
}

static inline uint16_t boot_get_cursor_loc_text(void)
{
    uint16_t cursor_loc;
    uint8_t cursor_loc_low, cursor_loc_high;

    outb(CRTC_ADDRESS_REGISTER_TEXT, CRTC_DATA_CURSOR_LOC_HIGH);
    cursor_loc_high = inb(CRTC_DATA_REGISTER_TEXT);
    outb(CRTC_ADDRESS_REGISTER_TEXT, CRTC_DATA_CURSOR_LOC_LOW);
    cursor_loc_low = inb(CRTC_DATA_REGISTER_TEXT);

    cursor_loc = (cursor_loc_high << 8) | cursor_loc_low;

    return cursor_loc;
}

void boot_set_cursor_loc_text(uint16_t cursor_loc)
{
    outb(CRTC_ADDRESS_REGISTER_TEXT, CRTC_DATA_CURSOR_LOC_HIGH);
    outb(CRTC_DATA_REGISTER_TEXT, cursor_loc >> 8);
    outb(CRTC_ADDRESS_REGISTER_TEXT, CRTC_DATA_CURSOR_LOC_LOW);
    outb(CRTC_DATA_REGISTER_TEXT, cursor_loc & 0xFF);
}

static inline void boot_set_cursor_loc(uint16_t cursor_loc)
{
    if (boot_display_mode == DISPLAY_MODE_BLACKWHITE) {
        boot_set_cursor_loc_blackwhite(cursor_loc);
    } else if (boot_display_mode == DISPLAY_MODE_TEXT) {
        boot_set_cursor_loc_text(cursor_loc);
    }
}

void boot_set_char(uint8_t color, uint8_t val, uint64_t loc)
{
    boot_text_mem[loc] = (color << 8) | val;
}

static inline void boot_roll_screen_mem(void)
{
    for (int i = CGA_TEXT_COLUMNS; i < CGA_TEXT_SIZE; i++) {
        boot_text_mem[i - CGA_TEXT_COLUMNS] = boot_text_mem[i];
    }

    for (int i = 0; i < CGA_TEXT_COLUMNS; i++) {
        boot_set_char((CGA_TEXT_COLOR_BLACK << 4) | CGA_TEXT_COLOR_WHIGHT, 
                      ' ', CGA_TEXT_SIZE - CGA_TEXT_COLUMNS + i);
    }
}

/**
 * I/O delay routine necessitated by historical PC design flaws
 * 
 * This function refers to xv6.
 */
static inline void __boot_putchar_delay(void)
{
    inb(0x84);
    inb(0x84);
    inb(0x84);
    inb(0x84);
}

/**
 * serial port output
 * 
 * This function refers to xv6.
 */
static void __boot_putchar_serial(uint8_t color, char ch)
{
    uint16_t chr = (color << 8) | ch;

    for (int i = 0; 
        !(inb(COM_SERIAL_LSR) & COM_SERIAL_LSR_TX_READY) && i < 12800;
        i++) {
        __boot_putchar_delay();
    }

    outb(COM_SERIAL_TX, chr);
}

/**
 * Parallel port output
 * 
 * This function refers to xv6.
 */
static void __boot_putchar_lpt(uint8_t color, char ch)
{
    uint16_t chr = (color << 8) | ch;

    for (int i = 0; !(inb(0x378+1) & 0x80) && i < 12800; i++) {
        __boot_putchar_delay();
    }

    outb(0x378 + 0, chr);
    outb(0x378 + 2, 0x8 | 0x4 | 0x1);
    outb(0x378 + 2, 0x8);
}

static void __boot_putchar_cga(uint8_t color, char ch)
{
    /* write char on the text-mode graphic mem */
    switch (ch) {
        case '\n':
            boot_cursor_loc += CGA_TEXT_COLUMNS;
            /* fall through there, so that \n == \r + \n */
        case '\r':
            boot_cursor_loc -= (boot_cursor_loc % CGA_TEXT_COLUMNS);
            break;
        case '\b':
            if (boot_cursor_loc % CGA_TEXT_COLUMNS) {
                boot_cursor_loc--;
            }
            break;
        case '\t':
            boot_putchar(' ');
            boot_putchar(' ');
            boot_putchar(' ');
            boot_putchar(' ');
            break;
        default:
            boot_set_char(color, ch, boot_cursor_loc);
            boot_cursor_loc++;
            break;
    }

    /* check for rolling screen */
    if (boot_cursor_loc >= CGA_TEXT_SIZE) {
        boot_roll_screen_mem();
        boot_cursor_loc = CGA_TEXT_SIZE - CGA_TEXT_COLUMNS;
    }

    /* write new cursor location back */
    boot_set_cursor_loc(boot_cursor_loc);
}

void __boot_putchar(uint8_t color, char ch)
{
    /** 
     * For historical reson, we have to read and write to specific port
     * before we write the char on the vram, so that cursor can work correctly.
    */
    __boot_putchar_serial(color, ch);
    __boot_putchar_lpt(color, ch);
    __boot_putchar_cga(color, ch);
}

void boot_putchar(char ch)
{
    __boot_putchar(CGA_TEXT_COLOR_DEFAULT, ch);
}

void boot_printstr(char *str)
{
    for (int i = 0; str[i]; i++) {
        boot_putchar(str[i]);
    }
}

void boot_puts(char *str)
{
    boot_printstr(str);
    boot_putchar('\n');
}

void boot_printnum(int64_t n)
{
    /* for 64-bit num it's enough*/
    char n_str[30];
    int idx = 30;

    n_str[--idx] = '\0';

    if (n < 0) {
        boot_putchar('-');
        n = -n;
    }

    do {
        n_str[--idx] = (n % 10) + '0';
        n /= 10;
    } while (n);
    
    boot_printstr(n_str + idx);
}

void boot_printhex(uint64_t n)
{
    /* for 64-bit num it's enough*/
    char n_str[30];
    int idx = 30;

    n_str[--idx] = '\0';

    do {
        uint64_t curr = n % 16;
        if (curr > 10) {
            n_str[--idx] = curr - 10 + 'a';
        } else {
            n_str[--idx] = curr + '0';
        }
        n /= 16;
    } while (n);
    
    boot_printstr(n_str + idx);
}

void boot_clear_screen(void)
{
    for (int i = 0; i < CGA_TEXT_SIZE; i++) {
        boot_set_char((CGA_TEXT_COLOR_BLACK << 4) | CGA_TEXT_COLOR_WHIGHT,
                      ' ', i);
    }

    if (boot_display_mode == DISPLAY_MODE_BLACKWHITE) {
        boot_set_cursor_loc_blackwhite(0);
    } else if (boot_display_mode == DISPLAY_MODE_TEXT) {
        boot_set_cursor_loc_text(0);
    }
    boot_cursor_loc = 0;
}

/**
 * serial init
 * 
 * This function refers to xv6.
 */
static void boot_tty_serial_init(void)
{
    /* Turn off the FIFO */
    outb(COM_SERIAL_FCR, 0);

    /* Set speed; requires DLAB latch */
    outb(COM_SERIAL_LCR, COM_SERIAL_LCR_DLAB);
    outb(COM_SERIAL_DLL, (uint8_t) (115200 / 9600));
    outb(COM_SERIAL_DLM, 0);

    /* 8 data bits, 1 stop bit, parity off; turn off DLAB latch */
    outb(COM_SERIAL_LCR, COM_SERIAL_LCR_WLEN8 & ~COM_SERIAL_LCR_DLAB);

    /* No modem controls */
    outb(COM_SERIAL_MCR, 0);

    /* Enable rcv interrupts */
    outb(COM_SERIAL_IER, COM_SERIAL_IER_RDI);

    /**
     * Clear any preexisting overrun indications and interrupts
     * Serial port doesn't exist if COM_LSR returns 0xFF
     */
    boot_serial_exists = (inb(COM_SERIAL_LSR) != 0xFF);
    inb(COM_SERIAL_IIR);
    inb(COM_SERIAL_RX);
}

void boot_tty_init(void)
{
    /**
     * There're two kind of Display, so we need to check there
     * Because this physical address is hard-wired to VRAM,
     * so I choose the way by simply writing data on it to check
     * (this's also what xv6 did)
     */
    boot_text_mem = (uint16_t*) CGA_TEXT_VRAM_START;
    boot_display_mode = DISPLAY_MODE_TEXT;

    *(uint64_t*) boot_text_mem = *(uint64_t*) "arttnba3";
    if (*(uint64_t*) boot_text_mem != *(uint64_t*) "arttnba3") {
        boot_text_mem = (uint16_t*) CGA_BLACKWHITE_VRAM_START;
        boot_display_mode = DISPLAY_MODE_BLACKWHITE;
    }

    boot_clear_screen();
    boot_tty_serial_init();
    if (!boot_serial_exists) {
        boot_puts("[x] NO serial port, you cannot input anything!\n");
    }
}
