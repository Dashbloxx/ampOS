#pragma once

#include <libc/stddef.h>
#include <libc/stdint.h>

typedef struct
{
    size_t size;
    uint8_t *data;
} bitmap_t;

#define PAGE_SIZE 4096