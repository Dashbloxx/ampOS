#pragma once

#include <kernel/terminal.h>

/**
 *  @brief Print out an error and then halt the system.
 *  @param string String that is to be printed before halting the system.
 */
static inline void panic(char *string)
{
    terminal_printf(current_terminal, string);
    while(1) { asm volatile("hlt"); }
}

#define ASSERT(cond) ((cond) ? (void)0 : panic("Assertion failed: " #cond))
