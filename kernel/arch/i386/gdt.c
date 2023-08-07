#include <libc/stdint.h>
#include <libc/string.h>
#include <kernel/arch/i386/gdt.h>
#include <kernel/thread.h>

struct tss _tss;

struct gdt_entry gdt_entries[7];
struct gdt_pointer gdt_pointer;

static void set_gdt_entry(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access = access;
}

void gdt_initialize()
{
    gdt_pointer.limit = (sizeof(struct gdt_entry) * 7) - 1;
    gdt_pointer.base  = (uint32_t)&gdt_entries;

    set_gdt_entry(0, 0, 0, 0, 0);
    set_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    set_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    set_gdt_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    set_gdt_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    memset((uint8_t*)&_tss, 0, sizeof(_tss));
    _tss.debug_flag = 0x00;
    _tss.io_map = 0x00;
    _tss.esp0 = 0;
    _tss.ss0 = 0x10;

    _tss.cs = 0x0B;
    _tss.ss = _tss.ds = _tss.es = _tss.fs = _tss.gs = 0x13;
    uint32_t tss_base = (uint32_t) &_tss;
    uint32_t tss_limit = sizeof(struct tss);
    set_gdt_entry(5, tss_base, tss_limit, 0xE9, 0x00);

    set_gdt_entry(6, 0, 0xFFFFFFFF, 0x80, 0xCF);

    flush_gdt((uint32_t)&gdt_pointer);
    flush_tss();
}