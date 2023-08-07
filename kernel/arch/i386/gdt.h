#pragma once

#include <libc/stdint.h>

extern struct tss _tss;

extern void flush_gdt(uint32_t);
extern void flush_idt(uint32_t);
extern void flush_tss();

void gdt_initialize();

struct gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_pointer
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));