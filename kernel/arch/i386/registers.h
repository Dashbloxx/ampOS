#pragma once

typedef struct
{
    uint32_t ss, gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t num, err_code;
    uint32_t eip, cs, eflags, user_esp, user_ss;
} __attribute__((packed)) registers_t;

extern void dump_registers(registers_t* registers);
extern void load_registers(registers_t* regs);