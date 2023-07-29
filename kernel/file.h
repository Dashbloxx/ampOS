#pragma once

#include <libc/stdint.h>

typedef struct
{
    char name[128];
    uint32_t ino;
} dirent_t;

typedef enum
{
    FS_FILE = 1,
    FS_CHARACTER_DEVICE = 2,
    FS_BLOCK_DEVICE = 3,
    FS_PIPE = 4,
    FS_SYMBOLIC_LINK = 5,
    FS_SOCKET = 6,
    FS_DIRECTORY = 128,
    FS_MOUNT_POINT = 256
} filetype_t;

typedef struct file file_t;

struct file
{
    char name[128];
    uint32_t mask;
    uint32_t uid;
    uint32_t gid;
    uint32_t flags;
    uint32_t inode;
    uint32_t length;
    uint32_t impl;
    uint32_t (*read)(file_t *, uint32_t, uint32_t, uint8_t *);
    uint32_t (*write)(file_t *, uint32_t, uint32_t, uint8_t *);
    void (*open)(file_t *);
    void (*close)(file_t *);
    dirent_t *(*readdir)(file_t *, uint32_t);
    file_t *(*finddir)(file_t *, char *name);
    file_t *ptr;
};

extern file_t *fs_root;