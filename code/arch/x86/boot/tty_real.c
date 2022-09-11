#include <graphics/vga.h>
#include <sumios/types.h>
#include <asm/io.h>

uint8_t *text_mem = (uint8_t*) REALMODE_TEXT_MODE_ADDR;

static inline uint16_t get_cursor_loc(void)
{
    uint16_t cursor_loc;
    uint8_t cursor_loc_low, cursor_loc_high;

    outb(CRTC_ADDRESS_REGISTER, CRTC_DATA_CURSOR_LOC_HIGH);
    cursor_loc_high = inb(CRTC_DATA_REGISTER);
    outb(CRTC_ADDRESS_REGISTER, CRTC_DATA_CURSOR_LOC_LOW);
    cursor_loc_low = inb(CRTC_DATA_REGISTER);

    cursor_loc = (cursor_loc_high << 8) | cursor_loc_low;

    return cursor_loc;
}

static inline void set_cursor_loc(uint16_t cursor_loc)
{
    outb(CRTC_ADDRESS_REGISTER, CRTC_DATA_CURSOR_LOC_HIGH);
    outb(CRTC_DATA_REGISTER, cursor_loc >> 8);
    outb(CRTC_ADDRESS_REGISTER, CRTC_DATA_CURSOR_LOC_LOW);
    outb(CRTC_DATA_REGISTER, cursor_loc & 0xFF);
}

void set_char(uint8_t color, uint8_t val, uint8_t loc)
{
    text_mem[loc * 2] = val;
    text_mem[loc * 2 + 1] = color;
}

static inline void roll_screen_mem(void)
{
    for (int i = 0; i < VGA_TEXT_SIZE - VGA_TEXT_COLUMNS; i++) {
        /* color */
        text_mem[2 * i] = text_mem[2 * (i + 1)];
        /* char */
        text_mem[2 * i + 1] = text_mem[2 * (i + 1) + 1];
    }

    for (int i = 0; i < VGA_TEXT_COLUMNS; i++) {
        set_char((VGA_TEXT_COLOR_BLACK << 4) & VGA_TEXT_COLOR_WHIGHT, ' ',
                VGA_TEXT_SIZE - VGA_TEXT_COLUMNS + i);
    }
}

void __putchar(uint8_t color, char ch)
{
    uint16_t cursor_loc;

    /* read cursor location from the CRT registers */
    cursor_loc = get_cursor_loc();

    /* write char on the text-mode graphic mem */
    switch (ch) {
    case '\n':
        cursor_loc += VGA_TEXT_COLUMNS;
        if (cursor_loc >= VGA_TEXT_SIZE) {
            roll_screen_mem();
            cursor_loc = VGA_TEXT_SIZE - VGA_TEXT_COLUMNS;
        } else {
            cursor_loc -= (cursor_loc % VGA_TEXT_COLUMNS);
        }
        break;
    case '\b':
        if (cursor_loc > 0) {
            cursor_loc--;
        }
        break;
    default:
        set_char(color, ch, cursor_loc);
        cursor_loc++;
        if (cursor_loc >= VGA_TEXT_SIZE) {
            roll_screen_mem();
            cursor_loc = VGA_TEXT_SIZE - VGA_TEXT_COLUMNS;
        }
        break;
    }

    /* write new cursor location back */
    set_cursor_loc(cursor_loc);
}

void putchar(char ch)
{
    __putchar((VGA_TEXT_COLOR_BLACK << 4) | VGA_TEXT_COLOR_WHIGHT, ch);
}

void printstr(char *str)
{
    for (int i = 0; str[i]; i++) {
        putchar(str[i]);
    }
}

void puts(char *str)
{
    printstr(str);
    putchar('\n');
}

void clear_screen(void)
{
    for (int i = 0; i < VGA_TEXT_SIZE; i++) {
        set_char((VGA_TEXT_COLOR_BLACK << 4) | VGA_TEXT_COLOR_WHIGHT, ' ', i);
    }
    set_cursor_loc(0);
}

void vga_init(void)
{
    
}
