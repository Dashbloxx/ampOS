#include <kernel/arch/i386/exceptions.h>
#include <kernel/terminal.h>

void exception_divzero()
{
    terminal_printf(current_terminal, "Exception triggered: Division by zero!\n");
    while(1)
    {
        asm volatile("hlt");
    }
}