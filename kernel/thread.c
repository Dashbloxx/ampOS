#include <libc/stdint.h>
#include <kernel/thread.h>
#include <kernel/arch/i386/gdt.h>

static void thread_switch_to(struct thread * _thread, int mode)
{
    uint32_t kesp, eflags;
    uint16_t kss, ss, cs;

    _tss.ss0 = _thread->stack.ss0;
    _tss.esp0 = _thread->stack.esp0;

    ss = _thread->registers.ss;
    cs = _thread->registers.cs;
    eflags = (_thread->registers.eflags | 0x200) & 0xFFFFBFFF;

    if (mode == USERMODE)
    {
        kss = _thread->stack.ss0;
        kesp = _thread->stack.esp0;
    }
    else
    {
        kss = _thread->registers.ss;
        kesp = _thread->registers.esp;
    }

    //switch_task is in task.asm

    asm("   mov %0, %%ss; \
        mov %1, %%esp; \
        cmpl %[KMODE], %[mode]; \
        je nextt; \
        push %2; \
        push %3; \
        nextt: \
        push %4; \
        push %5; \
        push %6; \
        push %7; \
        ljmp $0x08, $switch_task"
        :: \
        "m"(kss), \
        "m"(kesp), \
        "m"(ss), \
        "m"(_thread->registers.esp), \
        "m"(eflags), \
        "m"(cs), \
        "m"(_thread->registers.eip), \
        "m"(_thread), \
        [KMODE] "i"(KERNELMODE), \
        [mode] "g"(mode)
        );
}
