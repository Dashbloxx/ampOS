#pragma once

typedef int ssize_t;
typedef unsigned int size_t;

#define offsetof(type, member) ((size_t)(&((type *)0)->member))

#define container_of(ptr, type, member) ({ \
          const typeof(((type *)0)->member) *__mptr=(ptr); \
          (type *)((char*)__mptr-offsetof(type, member));})

#define NULL (void *)0