#pragma once

typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

#define KERN_VMEM_ADDR ((vaddr_t)0xc0000000u)
#define KERN_VMEM_TO_PHYS(v) ((paddr_t)((((vaddr_t)(v)) - KERN_VMEM_ADDR)))
#define PHYS_TO_KERN_VMEM(p) ((vaddr_t)(((paddr_t)(p)) + KERN_VMEM_ADDR))
#define KERN_STRAIGHT_MAP_SIZE ((size_t)0x38000000)