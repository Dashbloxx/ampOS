#include <libc/stddef.h>
#include <libc/stdint.h>
#include <libc/string.h>
#include <kernel/terminal.h>
#include <kernel/multiboot.h>
#include <kernel/heap.h>
#include <kernel/arch/i386/interrupt.h>
#include <kernel/file.h>
#include <kernel/tty.h>

extern uint8_t start_of_kernel;
extern uint8_t end_of_kernel;

uint32_t memlimit;

void kernel_main(multiboot_t *bootinfo) 
{
    /* Initialize all available terminals & print kernel boundaries. */
    terminal_initialize();
	terminal_printf(current_terminal, "Start of kernel: %x\nEnd of kernel: %x\nBootloader: %s\n", &start_of_kernel, &end_of_kernel, bootinfo->bootloader_name);

    /* Get limit of RAM. This will be given to the kernel's heap allocator. Then, print the results. */
    memlimit = bootinfo->mem_upper * 1024;
    terminal_printf(current_terminal, "Amount of detected memory in KiB: %d\n", memlimit);

    /* Initialize the kernel heap allocator. */
    kheap_initialize((void *)((uint32_t)&end_of_kernel + (uint32_t)8), (void *)memlimit);

    /* Set up interrupt-related routines. */
    idt_initialize();
    irq_initialize();

    tty_initialize();

    /* De-allocate the `stdout` character device, which is allocated by `tty_initialize`... */
    kfree(stdout);
}