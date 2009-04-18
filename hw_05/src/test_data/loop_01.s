.text
exit:
	li $v0, 10
	syscall
.data
.text
main:
	sw	$ra, 0($sp)
	sw	$fp, -4($sp)
	add	$fp, $sp, -4
	add	$sp, $sp, -8
	lw	$2, _framesize_main
	sub	$sp, $sp, $s
	# start while loop
_Test1:
	lw	$t0, a
	beqz	$t0, _Lexit1
	# content of while loop
	## add 1 to i
	# end of while loop
	j	_Test1
_Lexit1:
	# continue with main()

_begin_main:
	li	 $8, 0
	sw	 $8, -4($fp)

_end_main:
	lw	$ra, 4($fp)
	add	$sp, $fp, 4
	lw	$fp, 0($fp)
	j	exit

.data
	_framesize_main: .word 4
