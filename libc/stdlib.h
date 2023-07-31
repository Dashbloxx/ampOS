#pragma once

#include <libc/stddef.h>
#include <libc/stdint.h>

#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))

static void bzero(void *s, size_t n) {
    uint8_t *ptr = s;
    while(n--)
        *ptr++ = 0;
}