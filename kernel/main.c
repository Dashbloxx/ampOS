#include <libc/stddef.h>
#include <libc/stdint.h>
#include <libc/string.h>
#include <kernel/terminal.h>
#include <kernel/multiboot.h>
#include <kernel/arch/i386/idt.h>
#include <kernel/page.h>

extern uint8_t start_of_kernel;
extern uint8_t end_of_kernel;

void div_by_zero()
{
    terminal_printf(current_terminal, "Well, well well! Someone decided to divide by zero!\n");
}

void kernel_main(multiboot_t *bootinfo) 
{
    /* Initialize all available terminals & print kernel boundaries. */
    terminal_initialize();
	terminal_printf(current_terminal, "Start of kernel: %x\nEnd of kernel: %x\nBootloader: %s\n", &start_of_kernel, &end_of_kernel, bootinfo->bootloader_name);

    /* Set up interrupt-related routines. */
    idt_initialize();
    idt_register(0, IDT_INTGATE, &div_by_zero);

    /* Initialize page frame allocator. */
    page_initialize(bootinfo);

    int i = 12;
    int b = i / 0;
    b = i;

    terminal_printf(current_terminal, "Hello, world!\n");
}