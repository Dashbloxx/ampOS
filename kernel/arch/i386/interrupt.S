    .global lidt
lidt:
    pushl %ebp
    movl %esp, %ebp
    movl 8(%ebp), %eax
    lidt (%eax)
    popl %ebp
    ret

    .global int_0
    .extern exception_divzero
int_0:
    call exception_divzero
    ret

    .global int_32
    .extern pit_tick
int_32:
    call pit_tick
    ret