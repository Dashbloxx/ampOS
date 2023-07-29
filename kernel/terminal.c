#include <stdarg.h>

#include <libc/stdint.h>
#include <libc/stddef.h>
#include <kernel/terminal.h>
#include <kernel/arch/i386/ports.h>
#include <kernel/serial.h>

terminal_t *current_terminal;
terminal_t terminals[32];

void terminal_initialize()
{
    terminals[0].buffer = (uint16_t *)0xB8000;
    terminals[0].columns = 25;
    terminals[0].width = 80;
    terminals[0].color = (15 << 4) | (0 & 0x0F);
    terminals[0].x = 0;
    terminals[0].y = 0;
    terminal_clear(&terminals[0]);

    terminals[1].buffer = NULL;
    terminals[1].columns = 0;
    terminals[1].width = 0;
    terminals[1].color = 0;
    terminals[1].x = 0;
    terminals[1].y = 0;
    terminals[1].putc = &com1_write;
    com1_initialize();
    
    current_terminal = &terminals[0];
}

void terminal_putc(terminal_t *terminal, char character)
{
    uint16_t attribute = terminal->color << 8;
    uint16_t *location;

    if (character == 0x08 && terminal->x)
    {
        terminal->x--;
    }
    else if (character == 0x09)
    {
       terminal->x = (terminal->x + 8) & ~(8 - 1);
    }
    else if (character == '\r')
    {
        terminal->x = 0;
    }
    else if (character == '\n')
    {
        terminal->x = 0;
        terminal->y++;
    }
    else if(character >= ' ')
    {
        location = terminal->buffer + (terminal->y * terminal->width + terminal->x);
        *location = character | attribute;
        terminal->x++;
    }

    if (terminal->x >= 80)
    {
        terminal->x = 0;
        terminal->y++;
    }

    uint8_t attribute_byte = terminal->color;
    uint16_t blank = 0x20 | (attribute_byte << 8);

    if(terminal->y >= terminal->columns)
    {
        int i;
        for (i = 0 * terminal->width; i < (terminal->columns - 1) * terminal->width; i++)
        {
            terminal->buffer[i] = terminal->buffer[i + terminal->width];
        }

        for (i = (terminal->columns - 1) * terminal->width; i < terminal->columns * terminal->width; i++)
        {
            terminal->buffer[i] = blank;
        }
        terminal->y = terminal->columns - 1;
    }

    uint16_t cursor_location = terminal->y * 80 + terminal->x;
    outb(0x3D4, 14);
    outb(0x3D5, cursor_location >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, cursor_location);
}

void terminal_puts(terminal_t *terminal, char *string)
{
    int i = 0;
    while (string[i])
        terminal_putc(terminal, string[i++]);
}

void terminal_clear(terminal_t *terminal)
{
    uint16_t blank = 0x20 | (terminal->color << 8);

    int i;
    for (i = 0; i < terminal->width * terminal->columns; i++)
    {
        terminal->buffer[i] = blank;
    }

    terminal->x = 0;
    terminal->y = 0;
}

void terminal_printf(terminal_t *terminal, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    while (*format)
    {
        if (*format == '%')
        {
            format++;
            
            switch (*format)
            {
                case 's':
                {
                    char* str = va_arg(args, char*);
                    terminal_puts(terminal, str);
                    break;
                }
                case 'd':
                {
                    int num = va_arg(args, int);
                    if (num == 0)
                    {
                        terminal_putc(terminal, '0');
                        return;
                    }

                    int32_t acc = num;
                    char c[32];
                    int i = 0;
                    while (acc > 0)
                    {
                        c[i] = '0' + acc%10;
                        acc /= 10;
                        i++;
                    }
                    c[i] = 0;

                    char c2[32];
                    c2[i--] = 0;
                    int j = 0;
                    while(i >= 0)
                    {
                        c2[i--] = c[j++];
                    }
                    terminal_puts(terminal, c2);
                    break;
                }
                case 'x':
                {
                    unsigned int num = va_arg(args, unsigned int);
                    
                    int32_t tmp;
                    terminal_puts(terminal, "0x");

                    char no_zeroes = 1;

                    int i;
                    for (i = 28; i > 0; i -= 4)
                    {
                        tmp = (num >> i) & 0xF;
                        if (tmp == 0 && no_zeroes != 0)
                        {
                            continue;
                        }
                    
                        if (tmp >= 0xA)
                        {
                            no_zeroes = 0;
                            terminal_putc(terminal, tmp-0xA+'a' );
                        }
                        else
                        {
                            no_zeroes = 0;
                            terminal_putc(terminal, tmp+'0' );
                        }
                    }
                
                    tmp = num & 0xF;
                    if (tmp >= 0xA)
                    {
                        terminal_putc(terminal, tmp-0xA+'a');
                    }
                    else
                    {
                        terminal_putc(terminal, tmp+'0');
                    }
                    break;
                }
                case 'c':
                {
                    int c = va_arg(args, int);
                    terminal_putc(terminal, (char)c);
                    break;
                }
                default:
                    terminal_putc(terminal, *format);
                    break;
            }
        }
        else {
            terminal_putc(terminal, *format);
        }
        
        format++;
    }

    va_end(args);
}