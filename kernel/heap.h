#pragma once

#include <libc/stdint.h>

typedef struct _kheap_block
{
    struct {
        uint32_t size;
        uint8_t is_free;
    } metadata;
    struct _kheap_block *next; 
    void *data;
} __attribute__((packed)) kheap_block;

int kheap_initialize(void *start_addr, void *end_addr);
void *kbrk(int size);
void kheap_print_blocks();
void *kmalloc(int size);
void *kcalloc(int n, int size);
void *krealloc(void *ptr, int size);
void kfree(void *addr);