    .globl flush_gdt
flush_gdt:
    movl 4(%esp), %eax
    lgdt (%eax)

    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    ljmp $0x08, $.flush
.flush:
    ret


    .globl flush_idt
flush_idt:
    movl 4(%esp), %eax
    lidt (%eax)
    ret

    .globl flush_tss
flush_tss:
    movw $0x2B, %ax
    ltr %ax
    ret
