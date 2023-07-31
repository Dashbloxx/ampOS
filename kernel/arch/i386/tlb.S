    .global flushtlb
flushtlb:
    movl 4(%esp), %eax
    movl %eax, %cr3
    jmp .flush
.flush:
    ret
