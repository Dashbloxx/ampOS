    .globl switch_task
switch_task:
    movl (%esp), %esi
    popl %eax

    pushl 4(%esi)
    pushl 8(%esi)
    pushl 12(%esi)
    pushl 16(%esi)
    pushl 24(%esi)
    pushl 28(%esi)
    pushl 32(%esi)
    pushl 48(%esi)
    pushl 50(%esi)
    pushl 52(%esi)
    pushl 54(%esi)

    movb $0x20, %al
    outb %al, $0x20

    movl 56(%esi), %eax
    movl %eax, %cr3

    popw %gs
    popw %fs
    popw %es
    popw %ds
    popl %edi
    popl %esi
    popl %ebp
    popl %ebx
    popl %edx
    popl %ecx
    popl %eax

    iret
