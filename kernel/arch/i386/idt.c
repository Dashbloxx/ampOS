#include <libc/stdint.h>
#include <kernel/terminal.h>
#include <kernel/arch/i386/interrupt.h>

static idt_descriptor_t idt[IDT_ENTRIES];
static idt_pointer_t idtr;
interrupt_handler_t interrupt_handlers[IDT_ENTRIES];

void idt_register_interrupt_handler(uint8_t num, interrupt_handler_t handler) {
    interrupt_handlers[num] = handler;
}

void isr_handler(registers_t* regs)
{
    interrupt_handler_t handler = interrupt_handlers[regs->num];
    if (handler)
    {
        handler(regs);
        return;
    }

    terminal_printf(current_terminal, "Unhandled interrupt: %d\n", regs->num);
    while(1) { asm volatile("hlt"); }
}

void idt_set_gate(uint8_t idx, uint32_t base, uint16_t segment_selector, uint8_t gate_type, uint8_t dpl)
{
    idt_descriptor_t* entry = idt + idx;

    entry->base_lo = base & 0xffff;
    entry->base_hi = (base >> 16) & 0xffff;

    entry->segment_selector = segment_selector;
    entry->gate_type = gate_type & 0xf;
    entry->dpl = dpl & 3;
    entry->present = true;

    entry->reserved1 = entry->reserved2 = 0;
}

void idt_set_gate_user_callable(uint8_t idx)
{
    idt_descriptor_t* entry = idt + idx;
    entry->gate_type = TRAP_GATE32;
    entry->dpl = 3;
}

void idt_flush() { __asm__ volatile("lidt %0" ::"m"(idtr) : "memory"); }

#define DEFINE_ISR_WITHOUT_ERROR_CODE(num)                                     \
    void isr##num(void);                                                       \
    __asm__("isr" #num ":\n"                                                   \
            "pushl $0\n"                                                       \
            "pushl $" #num "\n"                                                \
            "jmp isr_common_stub");

#define DEFINE_ISR_WITH_ERROR_CODE(num)                                        \
    void isr##num(void);                                                       \
    __asm__("isr" #num ":\n"                                                   \
            "pushl $" #num "\n"                                                \
            "jmp isr_common_stub");

#define DEFINE_EXCEPTION(num, msg)                                             \
    static void handle_exception##num(registers_t* regs) {                     \
        terminal_printf(current_terminal, "Divide-by-zero error..." msg "\n"); \
        while(1) { asm volatile("hlt"); }                                      \
    }

#define DEFINE_EXCEPTION_WITHOUT_ERROR_CODE(num, msg)                          \
    DEFINE_ISR_WITHOUT_ERROR_CODE(num)                                         \
    DEFINE_EXCEPTION(num, msg)

#define DEFINE_EXCEPTION_WITH_ERROR_CODE(num, msg)                             \
    DEFINE_ISR_WITH_ERROR_CODE(num)                                            \
    DEFINE_EXCEPTION(num, msg)

DEFINE_ISR_WITHOUT_ERROR_CODE(0)
static void handle_exception0(registers_t* regs) {
    terminal_printf(current_terminal, "Divide-by-zero error...\n");
    while(1) { asm volatile("hlt"); }
}

DEFINE_EXCEPTION_WITHOUT_ERROR_CODE(1, "Debug")
DEFINE_EXCEPTION_WITHOUT_ERROR_CODE(2, "Non-maskable interrupt")
DEFINE_EXCEPTION_WITHOUT_ERROR_CODE(3, "Breakpoint")
DEFINE_EXCEPTION_WITHOUT_ERROR_CODE(4, "Overflow")
DEFINE_EXCEPTION_WITHOUT_ERROR_CODE(5, "Bound range exceeded")

DEFINE_ISR_WITHOUT_ERROR_CODE(6)
static void handle_exception6(registers_t* regs) {
    terminal_printf(current_terminal, "Invalid opcode\n");
    while(1) { asm volatile("hlt"); }
}

DEFINE_EXCEPTION_WITHOUT_ERROR_CODE(7, "Device not available")
DEFINE_EXCEPTION_WITH_ERROR_CODE(8, "Double fault")
DEFINE_EXCEPTION_WITHOUT_ERROR_CODE(9, "Coprocessor segment overrun")
DEFINE_EXCEPTION_WITH_ERROR_CODE(10, "Invalid TSS")
DEFINE_EXCEPTION_WITH_ERROR_CODE(11, "Segment not present")
DEFINE_EXCEPTION_WITH_ERROR_CODE(12, "Stack-segment fault")

DEFINE_ISR_WITH_ERROR_CODE(13)
static void handle_exception13(registers_t* regs) {
    terminal_printf(current_terminal, "General protection fault\n");
    while(1) { asm volatile("hlt"); }
}

DEFINE_ISR_WITH_ERROR_CODE(14)
static void handle_exception14(registers_t* regs) {
    uint32_t present = regs->err_code & 0x1;
    uint32_t write = regs->err_code & 0x2;
    uint32_t user = regs->err_code & 0x4;

    uint32_t cr2;
    __asm__("mov %%cr2, %%eax" : "=a"(cr2));

    terminal_printf(current_terminal, "Page fault (%s%s%s) at 0x%x\n",
            present ? "page-protection " : "non-present ",
            write ? "write " : "read ", user ? "user-mode" : "kernel-mode",
            cr2);
    while(1) { asm volatile("hlt"); }
}

DEFINE_EXCEPTION_WITHOUT_ERROR_CODE(15, "Unknown")
DEFINE_EXCEPTION_WITHOUT_ERROR_CODE(16, "x87 floating-point exception")

ENUMERATE_ISR_STUBS(DEFINE_ISR_WITHOUT_ERROR_CODE)

void idt_initialize()
{
    idtr.limit = IDT_ENTRIES * sizeof(idt_descriptor_t) - 1;
    idtr.base = (uint32_t)&idt;

#define REGISTER_ISR(num)                                                      \
    idt_set_gate(num, (uint32_t)isr##num, 0x8, INTERRUPT_GATE32, 0);

#define REGISTER_EXCEPTION(num)                                                \
    REGISTER_ISR(num);                                                         \
    idt_register_interrupt_handler(num, handle_exception##num);

    REGISTER_EXCEPTION(0);
    REGISTER_EXCEPTION(1);
    REGISTER_EXCEPTION(2);
    REGISTER_EXCEPTION(3);
    REGISTER_EXCEPTION(4);
    REGISTER_EXCEPTION(5);
    REGISTER_EXCEPTION(6);
    REGISTER_EXCEPTION(7);
    REGISTER_EXCEPTION(8);
    REGISTER_EXCEPTION(9);
    REGISTER_EXCEPTION(10);
    REGISTER_EXCEPTION(11);
    REGISTER_EXCEPTION(12);
    REGISTER_EXCEPTION(13);
    REGISTER_EXCEPTION(14);
    REGISTER_EXCEPTION(15);
    REGISTER_EXCEPTION(16);

    ENUMERATE_ISR_STUBS(REGISTER_ISR)

    idt_flush();
}