#include <libc/stdint.h>
#include <kernel/terminal.h>
#include <kernel/arch/i386/idt.h>

#define IDT_PRESENT 0x80
#define IDT_GATE32 0x8
#define IDT_DPL3 0x60
#define IDTSIZE 256

static struct gatedesc
{
    uint16_t baselo;
    uint16_t selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t basehi;
} __attribute__((packed)) idt[IDTSIZE];

static struct idtr
{
    uint16_t limit;
    struct gatedesc *base;
} __attribute__((packed)) idtr; 


void idt_register(uint8_t vecnum, uint8_t gatetype, void (*base)(void))
{
    struct gatedesc *desc = &idt[vecnum];
    desc->selector = 1 * 8;
    desc->baselo = (uint32_t)base & 0xffff;
    desc->basehi = (uint32_t)base >> 16;
    desc->reserved = 0;
    desc->flags = gatetype | IDT_PRESENT | IDT_GATE32 | IDT_DPL3;

    terminal_printf(current_terminal, "IDT: Registered %d\n", vecnum);
}

void idt_unregister(uint8_t vecnum)
{
    idt[vecnum].flags = 0;
}

void idt_initialize()
{
    for(int i = 0; i < IDTSIZE; i++)
        idt_unregister(i);

    idtr.limit = IDTSIZE * sizeof(struct gatedesc);
    idtr.base = idt;
    lidt(&idtr);
}