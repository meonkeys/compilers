L1:	i = i + 200
L3:	t1 = j - 5
	j = t1 - 42
L4:	iffalse i < 350 goto L5
L6:	i = 1 + i
L8:	t2 = i * 8
	t3 = a [ t2 ]
	if t3 < v goto L6
L7:	j = 1 + j
L10:	t4 = j * 8
	t5 = a [ t4 ]
	if t5 > v goto L7
L9:	iffalse i >= j goto L11
L12:	goto L5
L11:	t6 = i * 8
	x = a [ t6 ]
L13:	t7 = i * 8
	t8 = j * 8
	t9 = a [ t8 ]
	a [ t7 ] = t9
L14:	t10 = j * 8
	a [ t10 ] = x
	goto L4
L5:	j = 59
L15:	t11 = i * 5
	t12 = 200 - t11
	i = t12 + 11
L2:
