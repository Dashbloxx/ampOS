#pragma once

#define KERNELMODE 0
#define USERMODE 1

struct tss
{
    uint16_t previous_task, __previous_task_unused;
    uint32_t esp0;
    uint16_t ss0, __ss0_unused;
    uint32_t esp1;
    uint16_t ss1, __ss1_unused;
    uint32_t esp2;
    uint16_t ss2, __ss2_unused;
    uint32_t cr3;
    uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint16_t es, __es_unused;
    uint16_t cs, __cs_unused;
    uint16_t ss, __ss_unused;
    uint16_t ds, __ds_unused;
    uint16_t fs, __fs_unused;
    uint16_t gs, __gs_unused;
    uint16_t ldt_selector, __ldt_sel_unused;
    uint16_t debug_flag, io_map;
} __attribute__((packed));

extern struct tss _tss;

struct thread;
struct process;

struct process
{
    struct thread * threads;
};

struct thread
{
    uint32_t thread_id;
    
    struct
    {
        uint32_t eax, ecx, edx, ebx;
        uint32_t esp, ebp, esi, edi;
        uint32_t eip, eflags;
        uint32_t cs : 16, ss : 16, ds : 16, es : 16, fs : 16, gs : 16;
        uint32_t cr3;
    } registers __attribute__((packed));

    struct
    {
        uint32_t esp0;
        uint16_t ss0;
        uint32_t stack_start;
    } stack __attribute__ ((packed));

    struct thread * next;
    struct process * owner;
};