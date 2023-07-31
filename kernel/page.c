#include <libc/stdint.h>
#include <libc/stddef.h>
#include <libc/stdlib.h>
#include <kernel/page.h>
#include <kernel/terminal.h>
#include <kernel/list.h>
#include <kernel/multiboot.h>
#include <kernel/mem.h>

#define PAGE_ALLOCATED 0x1
#define PAGE_RESERVED  0x2
#define PAGE_NOTFREE (PAGE_ALLOCATED | PAGE_RESERVED)

#define PAGE_ALLOC_ZEROPAGE 0x1

#define MAX_ORDER 10

#define BUDDY(i) ((i) ^ (1 << pageinfo[(i)].order))
#define BUDDY_OF_ORDER(i, ord) ((i) ^ (1 << (ord)))

extern void *start_of_kernel;
extern void *end_of_kernel;

struct page
{
    struct list_head link;
    uint32_t flags;
    uint32_t order;
};

typedef uint32_t pageindex_t;

static size_t memsize;
struct page *pageinfo = NULL;
static uint32_t page_total;
static uint32_t protmem_freearea_addr;

struct list_head buddy_list[MAX_ORDER];
uint32_t buddy_count[MAX_ORDER];

int page_getnfree()
{
    int sum = 0;
    for (int i = 0; i < MAX_ORDER; i++)
        sum += buddy_count[i] * (1 << i);

    return sum;
}

int is_valid_page_index(pageindex_t idx)
{
    if (idx >= page_total)
        return 0;

    return 1;
}

int is_free_page_index(pageindex_t idx)
{
    if (!is_valid_page_index(idx))
        return 0;

    if (pageinfo[idx].flags & PAGE_NOTFREE)
        return 0;

    return 1;
}

static void return_to_freelist(struct page *page)
{
    list_pushback(&page->link, &buddy_list[page->order]);
    buddy_count[page->order] += 1;
}

static void take_from_freelist(struct page *page)
{
    list_remove(&page->link);
    buddy_count[page->order] -= 1;
}

static int try_merge_buddy(pageindex_t this_idx, pageindex_t *merged_idx, int is_init)
{
    struct page *this = &pageinfo[this_idx];
    if (!is_free_page_index(this_idx) || this->order+1 == MAX_ORDER)
        return -1;

    pageindex_t buddy_idx = BUDDY(this_idx);
    if (!is_free_page_index(buddy_idx))
        return -1;

    struct page *buddy = &pageinfo[buddy_idx];
    if (!is_init && this->order != buddy->order)
        return -1;

    if (is_init)
    {
        for (pageindex_t i = 0; i < (1 << this->order); i++)
            if (!is_free_page_index(buddy_idx + i))
                return -1;
    }

    if (this > buddy)
    {
        struct page *temp = buddy;
        buddy = this;
        this = temp;

        pageindex_t temp2 = buddy_idx;
        buddy_idx = this_idx;
        this_idx = temp2;
    }

    if (!is_init)
    {
        take_from_freelist(this);
        take_from_freelist(buddy);

        this->order++;
        this->flags = 0;
        return_to_freelist(this);
    }
    else
    {
        this->order++;
    }

    if (merged_idx)
        *merged_idx = this_idx;

    return 0;
}

static int split_to_buddy(pageindex_t this_idx)
{
    struct page *this = &pageinfo[this_idx];
    if (!is_free_page_index(this_idx) || this->order == 0)
        return -1;

    struct page *buddy = &pageinfo[BUDDY_OF_ORDER(this_idx, this->order-1)];

    take_from_freelist(this);

    this->order--;
    buddy->order = this->order;
    buddy->flags = 0;
    return_to_freelist(this);
    return_to_freelist(buddy);

    return 0;
}

void show_buddyinfo() {
    terminal_printf(current_terminal, "buddy:");
    for (int i = 0; i < MAX_ORDER; i++)
    {
        terminal_printf(current_terminal, " %d", buddy_count[i]);
    }
    terminal_printf(current_terminal, "\n");
}

static void buddy_initialize(paddr_t start, size_t size)
{
    pageindex_t startindex = (start + (PAGESIZE - 1)) / PAGESIZE;
    pageindex_t endindex = startindex + size / PAGESIZE;

    for (int i = 0; i < MAX_ORDER; i++)
    {
        list_init(&buddy_list[i]);
        buddy_count[i] = 0;
    }

    for (pageindex_t i = startindex; i < endindex; i++)
    {
        pageinfo[i].flags &= ~PAGE_RESERVED;
        pageinfo[i].order = 0;
    }

    for (pageindex_t i = startindex; i < endindex; )
    {
        pageindex_t merged_idx = i;
        while(try_merge_buddy(merged_idx, &merged_idx, 1) == 0);
        struct page *merged = &pageinfo[merged_idx];
        return_to_freelist(merged);
        i = merged_idx + (1 << merged->order);
    }

    show_buddyinfo();
}

void *page_alloc(size_t request, int flags)
{
    if (request == 0)
        return NULL;

    size_t req_pages = (request + (PAGESIZE - 1)) / PAGESIZE;
    int msb = 0;
    for (int i = 0; i < MAX_ORDER; i++)
    {
        if (req_pages & (1 << i))
            msb = i;
    }

    int req_order = msb;
    if (req_order >= MAX_ORDER)
        return NULL;

    int free_order;
alloc_try:
    for (free_order = req_order; free_order < MAX_ORDER; free_order++)
    {
        if(buddy_count[free_order] > 0)
            break;
    }

    if (free_order == MAX_ORDER)
    {
        //if(thread_yield_pages() == 0)
        //    goto alloc_try;
        //else
        //    return NULL;
        terminal_printf(current_terminal, "Tried calling unimplemented function thread_yield_pages...\n");
    }

    struct page *allocated = list_entry(list_first(&buddy_list[free_order]), struct page, link);

    pageindex_t allocated_idx = allocated - pageinfo;
    while (free_order-- != req_order)
    {
        split_to_buddy(allocated_idx);
    }

    allocated->flags |= PAGE_ALLOCATED;
    take_from_freelist(allocated);

    paddr_t paddr = (paddr_t)(allocated_idx * PAGESIZE);
    void *vaddr = (void *)PHYS_TO_KERN_VMEM(paddr);
    if (flags & PAGE_ALLOC_ZEROPAGE)
        bzero(vaddr, PAGESIZE << req_order);

    return vaddr;
}

void page_free(void *addr) {
    paddr_t paddr = KERN_VMEM_TO_PHYS(addr);
    pageindex_t this_idx = paddr / PAGESIZE;

    struct page *this = &pageinfo[this_idx];
    this->flags &= ~PAGE_ALLOCATED;
    return_to_freelist(this);

    while(try_merge_buddy(this_idx, &this_idx, 0) == 0);
}

void page_initialize(multiboot_t *bootinfo)
{
    if (bootinfo->flags & 0x40)
    {
        struct multiboot_mmap_entry *mmap = (uint32_t *)PHYS_TO_KERN_VMEM((char *)bootinfo->mmap_addr);
        struct multiboot_mmap_entry *mmap_end = (struct multiboot_mmap_entry *)((uint8_t *)mmap + bootinfo->mmap_length);
        terminal_printf(current_terminal, "Memory map: %x / %x\n", mmap, bootinfo->mmap_length);
        for (; mmap < mmap_end; mmap = (uint8_t *)mmap + mmap->size + sizeof(mmap->size))
        {
            if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
            memsize = MAX(memsize, (uint32_t)(mmap->addr & 0xffffffff) + (uint32_t)(mmap->length & 0xffffffff));
        }
    }
    else if (bootinfo->flags & 0x2)
    {
        memsize = bootinfo->mem_upper * 1024;
    }
    else
    {
        terminal_printf(current_terminal, "Can't detect memory info...\n");
        while(1)
        {
            asm volatile("hlt");
        }
    }

    memsize = MIN(memsize, KERN_STRAIGHT_MAP_SIZE);
    memsize = memsize & ~(PAGESIZE-1);
    pageinfo = (struct page *)&end_of_kernel;
    page_total = memsize / PAGESIZE;
    for (uint32_t i=0; i<page_total; i++)
        pageinfo[i].flags = PAGE_RESERVED;

    protmem_freearea_addr = (uint32_t)&pageinfo[page_total];

    buddy_initialize(KERN_VMEM_TO_PHYS(protmem_freearea_addr), memsize - (((uint32_t)KERN_VMEM_TO_PHYS(protmem_freearea_addr))));

    terminal_printf(current_terminal, "Page: %d MB(%d pages) free\n", (page_getnfree() * 4) / 1024, page_getnfree());
}

void *get_zeropage(size_t request)
{
    return page_alloc(request, PAGE_ALLOC_ZEROPAGE);
}
