#pragma once

#include <kernel/mem.h>

extern void flushtlb(void *addr);
void pagetbl_initialize();
paddr_t pagetbl_new();
void pagetbl_free(paddr_t pdt);
void pagetbl_add_mapping(uint32_t *pdt, vaddr_t vaddr, paddr_t paddr);
void pagetbl_remove_mapping(uint32_t *pdt, vaddr_t vaddr);
paddr_t pagetbl_dup_for_fork(paddr_t oldtbl);
