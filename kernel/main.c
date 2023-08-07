#include <libc/stddef.h>
#include <libc/stdint.h>
#include <libc/string.h>
#include <kernel/terminal.h>
#include <kernel/multiboot.h>
#include <kernel/arch/i386/gdt.h>
#include <kernel/arch/i386/idt.h>
#include <kernel/arch/i386/exceptions.h>
#include <kernel/arch/i386/pic.h>
#include <kernel/arch/i386/pagetable.h>
#include <kernel/page.h>
#include <kernel/malloc.h>

extern uint8_t start_of_kernel;
extern uint8_t end_of_kernel;

void kernel_main(multiboot_t *bootinfo) 
{
    /* Initialize all available terminals & print kernel boundaries. */
    terminal_initialize();
	terminal_printf(current_terminal, "Start of kernel: %x\nEnd of kernel: %x\nBootloader: %s\n", &start_of_kernel, &end_of_kernel, bootinfo->bootloader_name);

    /* Initialize global descriptor tables. */
    gdt_initialize();

    /* Initialize heap & page frame allocator */
    malloc_initialize();
    page_initialize(bootinfo);

    /* Set up interrupt-related routines. */
    idt_initialize();
    idt_register(0, IDT_INTGATE, &exception_divzero);

    /* Initialize the PIC. */
    pic_initialize();

    /* Initialize page tables. */
    pagetbl_initialize();

    terminal_printf(current_terminal, "Hello, world!\n");
}