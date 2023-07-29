#include <kernel/page.h>
#include <kernel/heap.h>

/**
 *  @brief Allocate page, using kernel heap allocator.
 */
bitmap_t *palloc()
{
    bitmap_t *bitmap = kmalloc(sizeof(bitmap_t));
    bitmap->data = kmalloc(PAGE_SIZE);
    bitmap->size = PAGE_SIZE;
    return bitmap;
}

/**
 *  @brief De-allocate a page, using kernel heap allocator.
 *  @param bitmap The pointer to the bitmap struct that should be de-allocated, including it's data.
 */
void pfree(bitmap_t *bitmap)
{
    kfree(bitmap->data);
    kfree(bitmap);
}