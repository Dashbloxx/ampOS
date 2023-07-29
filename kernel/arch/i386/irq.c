#include <kernel/arch/i386/interrupt.h>
#include <kernel/arch/i386/registers.h>
#include <kernel/arch/i386/ports.h>
#include <kernel/terminal.h>
#include <kernel/message.h>
#include <libc/stdint.h>
#include <libc/string.h>

#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xa0
#define PIC2_DATA 0xa1

#define PIC_EOI 0x20

#define ICW1_ICW4 0x01
#define ICW1_INIT 0x10
#define ICW4_8086 0x01

#define NUM_IRQS_PER_PIC 8

static void remap_pic()
{
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, IRQ(0));
    outb(PIC2_DATA, IRQ(NUM_IRQS_PER_PIC));
    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);
    outb(PIC1_DATA, 0);
    outb(PIC2_DATA, 0);
}

#define ENUMERATE_IRQS(F)                                                      \
    F(0)                                                                       \
    F(1)                                                                       \
    F(2)                                                                       \
    F(3)                                                                       \
    F(4)                                                                       \
    F(5)                                                                       \
    F(6)                                                                       \
    F(7)                                                                       \
    F(8)                                                                       \
    F(9)                                                                       \
    F(10)                                                                      \
    F(11)                                                                      \
    F(12)                                                                      \
    F(13)                                                                      \
    F(14)                                                                      \
    F(15)

#define DEFINE_IRQ(num)                                                        \
    void irq##num(void);                                                       \
    __asm__("irq" #num ":\n"                                                   \
            "pushl $0\n"                                                       \
            "pushl $" STRINGIFY(IRQ(num)) "\n"                                 \
                                          "jmp irq_common_stub");
ENUMERATE_IRQS(DEFINE_IRQ)
#undef DEFINE_IRQ

extern interrupt_handler_t interrupt_handlers[256];

void irq_handler(registers_t* regs) {
    ASSERT(!interrupts_enabled());

    if (regs->num >= NUM_IRQS_PER_PIC)
        outb(PIC2_CMD, PIC_EOI);

    outb(PIC1_CMD, PIC_EOI);

    interrupt_handler_t handler = interrupt_handlers[regs->num];
    if (handler)
        handler(regs);
}

void irq_initialize() {
    remap_pic();

#define REGISTER_IRQ(num)                                                      \
    idt_set_gate(IRQ(num), (uint32_t)irq##num, 0x8, INTERRUPT_GATE32, 0);
    ENUMERATE_IRQS(REGISTER_IRQ)
#undef REGISTER_IRQ

    idt_flush();
}