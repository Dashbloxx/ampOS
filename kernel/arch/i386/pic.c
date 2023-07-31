#include <libc/stdint.h>
#include <kernel/arch/i386/pic.h>
#include <kernel/arch/i386/ports.h>

enum regs {
    MASTER_CMD = 0x20,
    MASTER_STAT = 0x20,
    MASTER_IMR = 0x21,
    MASTER_DATA = 0x21,
    SLAVE_CMD = 0xa0,
    SLAVE_STAT = 0xa0,
    SLAVE_IMR = 0xa1,
    SLAVE_DATA = 0xa1,
};

enum mask {
    MASK_IRQ0 = 0x01,
    MASK_IRQ1 = 0x02,
    MASK_IRQ2 = 0x04,
    MASK_IRQ3 = 0x08,
    MASK_IRQ4 = 0x10,
    MASK_IRQ5 = 0x20,
    MASK_IRQ6 = 0x40,
    MASK_IRQ7 = 0x80,
    MASK_ALL = 0xff,
};

static uint8_t master_imr;
static uint8_t slave_imr;

void pic_initialize()
{
    outb(MASTER_CMD, 0x11);
    outb(SLAVE_CMD, 0x11);
    outb(MASTER_DATA, 0x20);
    outb(SLAVE_DATA, 0x28);
    outb(MASTER_DATA, 0x04);
    outb(SLAVE_DATA, 0x02);
    outb(MASTER_DATA, 0x01);
    outb(SLAVE_DATA, 0x01);

    master_imr = ~MASK_IRQ2;
    slave_imr = MASK_ALL;
    outb(MASTER_IMR, master_imr);
    outb(SLAVE_IMR, slave_imr);
}

void pic_setmask(int irq)
{
    if(irq < 8)
    {
        master_imr |= 1 << irq;
        outb(MASTER_IMR, master_imr);
    }
    else
    {
        slave_imr |= 1 << (irq - 8);
        outb(SLAVE_IMR, slave_imr);
    }
}

void pic_clearmask(int irq)
{
    if(irq < 8)
    {
        master_imr &= ~(1 << irq);
        outb(MASTER_IMR, master_imr);
    }
    else
    {
        slave_imr &= ~(1 << (irq - 8));
        outb(SLAVE_IMR, slave_imr);
    }
}

void pic_sendeoi(int irq)
{
    if(irq >= 8)
        outb(SLAVE_CMD, 0x20);
    outb(MASTER_CMD, 0x20);
}
