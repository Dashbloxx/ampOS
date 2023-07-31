#include <libc/stdint.h>
#include <kernel/arch/i386/pagetable.h>
#include <kernel/common.h>
#include <kernel/page.h>

#define PDE_PRESENT 0x1
#define PDE_RW 0x2
#define PDE_USER 0x4
#define PDE_WRITETHRU 0x8
#define PDE_CACHE_DISABLE 0x10
#define PDE_ACCESS 0x20
#define PDE_SIZE_4MB 0x80
#define PDE_GLOBAL 0x100

#define PTE_PRESENT 0x1
#define PTE_RW 0x2
#define PTE_USER 0x4
#define PTE_WRITETHRU 0x8
#define PTE_CACHE_DISABLE 0x10
#define PTE_ACCESS 0x20
#define PTE_DIRTY 0x40
#define PTE_GLOBAL 0x100

#define TOTAL_NUM_PDE (PAGESIZE >> 2)
#define TOTAL_NUM_PTE (PAGESIZE >> 2)

static uint32_t KERN_PDE_START;

static uint32_t *kernspace_pdt;

void pagetbl_initialize()
{
    kernspace_pdt = get_zeropage(PAGESIZE);
    int st_start_index = KERN_VMEM_ADDR / 0x400000;
    KERN_PDE_START = st_start_index;
    int st_end_index = st_start_index + (KERN_STRAIGHT_MAP_SIZE/0x400000);
    paddr_t addr = 0x0;

    for(int i = st_start_index; i < st_end_index; i++, addr += 0x400000)
    {
        kernspace_pdt[i] = addr | PDE_PRESENT | PDE_RW | PDE_SIZE_4MB;
    }

    for(int i = st_end_index; i < TOTAL_NUM_PDE; i++)
    {
        uint32_t *pt = get_zeropage(PAGESIZE);
        kernspace_pdt[i] = KERN_VMEM_TO_PHYS((vaddr_t)pt) | PDE_PRESENT | PDE_RW;
    }

    flushtlb(KERN_VMEM_TO_PHYS(kernspace_pdt));
}


paddr_t pagetbl_new()
{
    uint32_t *pdt = get_zeropage(PAGESIZE);
    for(int i = 0; i < TOTAL_NUM_PDE; i++)
        pdt[i] = kernspace_pdt[i];

    return KERN_VMEM_TO_PHYS(pdt);
}

void pagetbl_add_mapping(uint32_t *pdt, vaddr_t vaddr, paddr_t paddr)
{
    uint32_t *v_pdt = (uint32_t *)PHYS_TO_KERN_VMEM(pdt);
    int pdtindex = vaddr >> 22;
    int ptindex = (vaddr >> 12) & 0x3ff;
    if((v_pdt[pdtindex] & PDE_PRESENT) == 0)
    {
        v_pdt[pdtindex] = KERN_VMEM_TO_PHYS((uint32_t)get_zeropage(PAGESIZE)) | PDE_PRESENT | PDE_RW | PDE_USER;
    }

    uint32_t *pt = (uint32_t *)(PHYS_TO_KERN_VMEM(v_pdt[pdtindex] & ~0xfff));
    pt[ptindex] = (paddr & ~0xfff) | PTE_PRESENT | PTE_RW | PTE_USER;
}

void pagetbl_remove_mapping(uint32_t *pdt, vaddr_t vaddr)
{
    uint32_t *v_pdt = (uint32_t *)PHYS_TO_KERN_VMEM(pdt);
    int pdtindex = vaddr >> 22;
    int ptindex = (vaddr >> 12) & 0x3ff;
    if((v_pdt[pdtindex] & PDE_PRESENT) == 0)
        return;

    uint32_t *pt = (uint32_t *)(PHYS_TO_KERN_VMEM(v_pdt[pdtindex] & ~0xfff));
    pt[ptindex] &= ~PTE_PRESENT;
}

void pagetbl_free(paddr_t pdt)
{
    uint32_t *v_pdt = (uint32_t *)PHYS_TO_KERN_VMEM(pdt);
    for(int i = 0; i < KERN_PDE_START; i++)
    {
        uint32_t ent = v_pdt[i];
        if((ent & PDE_PRESENT))
        {
            page_free(PHYS_TO_KERN_VMEM(ent & ~0xfff));
        }
    }

    page_free(v_pdt);
}

static vaddr_t pagetbl_dup_one(vaddr_t oldpt)
{
    uint32_t *newpt = get_zeropage(PAGESIZE);

    for(int i = 0; i < TOTAL_NUM_PTE; i++)
    {
        uint32_t oldent = ((uint32_t *)oldpt)[i];
        if(oldent & PTE_PRESENT)
        {
            newpt[i] = (oldent & ~0xfff) | PTE_PRESENT | PTE_USER;
            ((uint32_t *)oldpt)[i] &= ~PTE_RW;
        }
    }
    return (vaddr_t)newpt;
}

paddr_t pagetbl_dup_for_fork(paddr_t oldpdt)
{
    uint32_t *pdt = page_alloc(PAGESIZE, 0);
    uint32_t *v_oldpdt = (uint32_t *)PHYS_TO_KERN_VMEM(oldpdt);

    for(int i = 0; i < TOTAL_NUM_PDE; i++)
        pdt[i] = kernspace_pdt[i];

    for(int i = 0; i < KERN_PDE_START; i++)
    {
        uint32_t oldent = v_oldpdt[i];
        if(oldent & PDE_PRESENT)
        {
            vaddr_t newpt = pagetbl_dup_one(PHYS_TO_KERN_VMEM(oldent & ~0xfff));
            pdt[i] = KERN_VMEM_TO_PHYS(newpt) | PDE_PRESENT | PDE_RW | PDE_USER;
        }
    }

    return KERN_VMEM_TO_PHYS(pdt);
}