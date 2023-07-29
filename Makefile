TOOLPREFIX = i686-elf-
CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)as
NASM = nasm
NASMFLAGS = -felf
CFLAGS = -std=gnu99 -ffreestanding -I.
LDFLAGS = -Tlinker.ld -ffreestanding -nostdlib -nostdinc

SRCS := $(wildcard kernel/*.c)
ASMS := $(wildcard kernel/*.asm)
SSMS := $(wildcard kernel/*.S)
ARCH_SRCS := $(wildcard kernel/arch/i386/*.c)
ARCH_ASMS := $(wildcard kernel/arch/i386/*.asm)
ARCH_SSMS := $(wildcard kernel/arch/i386/*.S)
OBJS := $(ASMS:.asm=.o) $(SRCS:.c=.o) $(SSMS:.S=.o) $(ARCH_ASMS:.asm=.o) $(ARCH_SRCS:.c=.o) $(ARCH_SSMS:.S=.o)

all: kernel.bin

kernel.bin: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.asm
	$(NASM) $(NASMFLAGS) $< -o $@

%.o: %.S
	$(AS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f kernel/*.o kernel/arch/i386/*.o kernel.bin
