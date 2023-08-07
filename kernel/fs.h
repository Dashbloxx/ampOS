#pragma once

#include <libc/stdint.h>
#include <kernel/fs.h>

struct file;

extern struct file *fs_root;

typedef uint32_t (* read_type_t)(struct file *, uint32_t, uint32_t, uint8_t *);
typedef uint32_t (* write_type_t)(struct file *, uint32_t, uint32_t, uint8_t *);
typedef void (* open_type_t)(struct file *);
typedef void (* close_type_t)(struct file *);
typedef struct dirent * (* readdir_type_t)(struct file *, uint32_t);
typedef struct file * (* finddir_type_t)(struct file *,char *name); 

struct file
{
    char name[128];
    uint32_t mask; 
    uint32_t uid;  
    uint32_t gid;  
    uint32_t flags;
    uint32_t length;
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
};

enum
{
    FS_FILE = 0x01,
    FS_DIRECTORY = 0x02,
    FS_CHARDEVICE = 0x03,
    FS_BLOCKDEVICE = 0x04,
    FS_PIPE = 0x05,
    FS_SYMLINK = 0x06,
    FS_MOUNTPOINT = 0x08
};