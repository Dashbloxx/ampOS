#include <libc/stdint.h>
#include <libc/stddef.h>
#include <libc/string.h>
#include <libc/stdlib.h>
#include <kernel/list.h>
#include <kernel/common.h>
#include <kernel/page.h>
#include <kernel/terminal.h>

#define SIZE_BASE 8
#define MAX_BIN (((PAGESIZE - sizeof(struct chunkhdr)) >> 1) / SIZE_BASE)
#define USE_BIN_THRESHOLD (SIZE_BASE * MAX_BIN)

struct chunkhdr {
    struct list_head link;
    size_t size;
    void *freelist;
    int nobjs;
    int nfree;
};

#define GET_CHUNKHDR(p) ((struct chunkhdr *)pagealign((uint32_t)(p)))

struct list_head bin[MAX_BIN + 1];

void malloc_initialize()
{
    for(unsigned int i = 0; i <= MAX_BIN; i++)
        list_init(&bin[i]);
}

static struct chunkhdr *getnewchunk(size_t objsize)
{
    if(objsize < 4 || objsize > PAGESIZE - sizeof(struct chunkhdr))
    {
        return NULL;
    }

    struct chunkhdr *newchunk = (struct chunkhdr *)page_alloc(PAGESIZE, 0);
    if(newchunk == NULL)
    terminal_printf(current_terminal, "Heap: page_alloc failed.");

    newchunk->size = objsize;
    newchunk->freelist = NULL;
    int nobjs = (PAGESIZE - sizeof(struct chunkhdr)) / objsize;
    newchunk->nobjs = newchunk->nfree = nobjs;
    uint8_t *obj = (uint8_t *)(newchunk + 1);
    for(int i = 0; i < nobjs; i++)
    {
        *(void **)obj = newchunk->freelist;
        newchunk->freelist = obj;
        obj += objsize;
    }

    return newchunk;
}

static void *getobj(int binindex)
{
    struct list_head *p;
retry:
    list_foreach(p, &bin[binindex])
    {
        struct chunkhdr *ch = list_entry(p, struct chunkhdr, link);
        if(ch->nfree > 0) {
            void *obj = ch->freelist;
            ch->freelist = *(void **)obj;
            ch->nfree--;
            return obj;
        }
    }

    struct chunkhdr *newch = getnewchunk(binindex * SIZE_BASE);
    if(newch == NULL)
        return NULL;

    list_pushfront(&newch->link, &bin[binindex]);

    goto retry;
}

void *malloc(size_t request)
{
    void *m = NULL;

    if (request == 0)
        return NULL;

IRQ_DISABLE
    size_t size = (request + (SIZE_BASE-1)) & ~(SIZE_BASE-1);

    if(size > USE_BIN_THRESHOLD)
    {
        size_t allocsz =  (size + sizeof(struct chunkhdr) + (PAGESIZE-1)) & ~(PAGESIZE-1);
        struct chunkhdr *ch = page_alloc(allocsz , 0);
        if(ch == NULL)
            terminal_printf(current_terminal, "Warning: malloc failed...\n");
        ch->size = size;
        m = (void *)(ch + 1);
    }
    else
    {
        m = getobj(size / SIZE_BASE);
        if(m == NULL)
            terminal_printf(current_terminal, "Warning: malloc failed...\n");
    }
IRQ_RESTORE
    return m;
}

void free(void *addr) {
    if (addr == NULL)
        return;

IRQ_DISABLE
    struct chunkhdr *ch = GET_CHUNKHDR(addr);
    if (ch->size > USE_BIN_THRESHOLD)
    {
        page_free(ch);
    }
    else
    {
        ch->nfree++;
        if (ch->nfree == ch->nobjs)
        {
            list_remove(&ch->link);
            page_free(ch);
        }
        else
        {
            *(void **)addr = ch->freelist;
            ch->freelist = addr;
        }
    }
IRQ_RESTORE
}

void *realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
    {
        return malloc(size);
    }
    else if (size == 0)
    {
        free(ptr);
        return NULL;
    }

    struct chunkhdr *ch = GET_CHUNKHDR(ptr);
    void *new = malloc(size);
    memcpy(new, ptr, MIN(ch->size, size));
    free(ptr);

    return new;
}