#include <kernel/arch/i386/pit.h>
#include <kernel/arch/i386/pic.h>
#include <kernel/arch/i386/timer.h>
#include <kernel/arch/i386/idt.h>
#include <kernel/arch/i386/ports.h>
#include <kernel/terminal.h>

#define PIT_CH0_DATA 0x40
#define PIT_CH1_DATA 0x41
#define PIT_CH2_DATA 0x42
#define PIT_MODE_CMD_REG 0x43

#define PIT_CNTMODE_BIN 0x0
#define PIT_OPMODE_RATE 0x4
#define PIT_OPMODE_SQUARE 0x6
#define PIT_LOAD16 0x30
#define PIT_CNT0 0x0

#define PIT_IRQ 0

#define CNT_100HZ 0x2e9b

void pit_isr() {
    terminal_printf(current_terminal, "Hello, world!\n");
    pic_sendeoi(PIT_IRQ);
}

void pit_initialize() {
    outb(PIT_MODE_CMD_REG, PIT_CNTMODE_BIN | PIT_OPMODE_RATE | PIT_LOAD16 | PIT_CNT0);
    outb(PIT_CH0_DATA, CNT_100HZ & 0xff);
    outb(PIT_CH0_DATA, CNT_100HZ >> 8);
    idt_register(IRQ_TO_INTVEC(PIT_IRQ), IDT_INTGATE, pit_isr);
    pic_clearmask(PIT_IRQ);
}
