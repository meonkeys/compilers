.text
exit:
	li $v0, 10
	syscall
.data
	noval: .word
	car: .word 1
	jar: .word 13
	lid: .word 78
	flag: .float 17.280001
	stag: .float 7.000000
.text
main:
    # TODO: load 5 into a local floating-point variable, "cat"
    b exit
