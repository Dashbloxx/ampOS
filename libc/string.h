#pragma once

#include <libc/stdint.h>
#include <libc/stddef.h>

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

static void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;
 
    for (size_t i = 0; i < n; i++)
    {
        pdest[i] = psrc[i];
    }
 
    return dest;
}
 
static void *memset(void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *)s;
 
    for (size_t i = 0; i < n; i++)
    {
        p[i] = (uint8_t)c;
    }
 
    return s;
}
 
static void *memmove(void *dest, const void *src, size_t n)
{
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;
 
    if (src > dest)
    {
        for (size_t i = 0; i < n; i++)
        {
            pdest[i] = psrc[i];
        }
    } else if (src < dest)
    {
        for (size_t i = n; i > 0; i--)
        {
            pdest[i-1] = psrc[i-1];
        }
    }
 
    return dest;
}
 
static int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;
 
    for (size_t i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
        {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }
 
    return 0;
}

static char* strcpy(char* destination, const char* source)
{
    if (destination == NULL)
    {
        return NULL;
    }
 
    char *ptr = destination;

    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }
 
    *destination = '\0';
 
    return ptr;
}