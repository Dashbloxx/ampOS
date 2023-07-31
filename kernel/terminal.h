#pragma once

#include <libc/stdint.h>

void terminal_initialize();

typedef struct {
    void (*putc)(char character);
    uint16_t *buffer;
    uint8_t width, columns;
    uint8_t x, y;
    uint8_t color;
} terminal_t;

extern terminal_t *current_terminal;
extern terminal_t terminals[32];

void terminal_putc(terminal_t *terminal, char character);
void terminal_puts(terminal_t *terminal, char *string);
void terminal_printf(terminal_t *terminal, const char* format, ...);
void terminal_clear(terminal_t *terminal);