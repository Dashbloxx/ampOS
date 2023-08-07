#include <libc/stdint.h>
#include <kernel/fs.h>

struct file * root = 0;

uint32_t fs_read(struct file * node, uint32_t offset, uint32_t size, uint8_t * buffer)
{
    if(node->read != 0)
        return node->read(node, offset, size, buffer);
    else
        return 0;
} 

uint32_t fs_write(struct file * node, uint32_t offset, uint32_t size, uint8_t * buffer)
{
    if(node->read != 0)
        return node->write(node, offset, size, buffer);
    else
        return 0;
}

void fs_open(struct file * node)
{
    node->open(node);
}

void fs_close(struct file * node)
{
    node->close(node);
}