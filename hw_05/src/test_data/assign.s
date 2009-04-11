.text
main:
    li $t0, 1  # int lid = 1
    b exit
exit:
    li $v0, 10
    syscall
