#include <libc/stddef.h>
#include <libc/stdint.h>
#include <libc/string.h>
#include <kernel/terminal.h>
#include <kernel/multiboot.h>
#include <kernel/arch/i386/interrupt.h>

extern uint8_t start_of_kernel;
extern uint8_t end_of_kernel;

uint32_t memlimit;

void kernel_main(multiboot_t *bootinfo) 
{
    /* Initialize all available terminals & print kernel boundaries. */
    terminal_initialize();
	terminal_printf(current_terminal, "Start of kernel: %x\nEnd of kernel: %x\nBootloader: %s\n", &start_of_kernel, &end_of_kernel, bootinfo->bootloader_name);

    /* Set up interrupt-related routines. */
    idt_initialize();
    irq_initialize();

    /* Initialize page frame allocator. */
    page_initialize(bootinfo);
    
    /* Allocate a page. */
    void *a = page_alloc(1, 0);
    
    /* Print the address of the page. */
    terminal_printf(current_terminal, "Got this: %x...\n", (uint32_t)a);
}