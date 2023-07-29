#include <libc/stddef.h>
#include <libc/stdint.h>
#include <libc/string.h>
#include <kernel/heap.h>
#include <kernel/terminal.h>
#include <kernel/tty.h>

file_t *stdout = NULL;

static uint32_t terminal_write(file_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    terminal_puts(current_terminal, buffer);
    return size;
}

/**
 *  @brief Create some character devices that represent the terminals from `terminal.c` & `terminal.h`.
 */
void tty_initialize()
{
    /* Create a character device that represents the output of the current terminal. */
    stdout = (file_t *)kmalloc(sizeof(file_t));
    strcpy(stdout->name, "stdout");
    stdout->mask = 0;
    stdout->uid = 0;
    stdout->gid = 0;
    stdout->flags = FS_CHARACTER_DEVICE;
    stdout->inode = 0;
    stdout->length = 0;
    stdout->impl = 0;
    stdout->read = NULL;
    stdout->write = terminal_write;
    stdout->open = NULL;
    stdout->close = NULL;
    stdout->readdir = NULL;
    stdout->finddir = NULL;
    stdout->ptr = NULL;

    /* TODO: Implement stdin, which represents the keyboard ONLY when using any terminal that isn't serial. */
}