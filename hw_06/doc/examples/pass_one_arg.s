.text
main:   
	sw      $ra, ($sp)
	sw      $fp, -4($sp)
	add     $fp, $sp, -4
	add     $sp, $sp, -8
	lw      $v0, _framesize_main
	sub     $sp, $sp, $v0
_begin_main:
	li      $9, 29 			# line 8: int z = 29;
	sw      $9, -4($fp)     # line 8: store value of z on the stack
	li      $10, 7          # PASS AN INTEGER ARG to foo()
	sw		$10, ($sp)		# ...on the stack.
	add     $sp, $sp, -4    # space for 1 argument to foo
	jal		foo				# line 9
	move	$t3, $v0		# grab return value
_end_main:      			# line 11
	lw      $ra, 4($fp)
	add     $sp, $fp, 4
	lw      $fp, 0($fp)
	li      $v0, 10
	syscall 

foo:
	sw      $ra, ($sp)		# save return address on stack
	sw      $fp, -4($sp)	# save old frame pointer on stack
	add     $fp, $sp, -4	# move frame pointer to start of activation record
	add     $sp, $sp, -8	# move stack pointer to top of stack
	lw      $v0, _framesize_foo
	sub     $sp, $sp, $v0	# push new activation record on stack

_begin_foo:
	li      $8, 31			# line 2
	sw      $8, -4($fp)     # line 2
	li      $v0, 1
	lw      $a0, 8($fp)		# print first param to foo(), a
	syscall
	li      $v0, 4
	la      $a0, _sConst0
	syscall

_end_foo:       			# line 5
	lw      $fp, 0($fp)
	jr      $ra

.data   
	_framesize_foo: .word 4
	_sConst0: .asciiz "\n"
	_framesize_main: .word 4

# vim: ts=4
