    .global lidt
lidt:
    pushl %ebp
    movl %esp, %ebp
    movl 8(%ebp), %eax
    lidt (%eax)
    popl %ebp
    ret
