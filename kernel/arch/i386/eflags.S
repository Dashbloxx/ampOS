    .global geteflags
geteflags:
    pushf
    popl %eax
    ret
