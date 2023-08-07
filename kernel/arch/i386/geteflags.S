    .globl geteflags
geteflags:
    pushf
    popl %eax
    ret
