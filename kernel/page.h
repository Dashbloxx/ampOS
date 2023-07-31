#pragma once

#include <libc/stdint.h>
#include <libc/stddef.h>
#include <kernel/multiboot.h>

#define PAGESIZE 4096

void page_initialize(multiboot_t *bootinfo);
int page_getnfree(void);
void *page_alloc(size_t, int);
void page_free(void *addr);
void *get_zeropage(size_t);