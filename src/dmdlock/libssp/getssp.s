
	global	getssp
getssp:
	mov.l	_addrSys,%a0
	mov.l	4*1147(%a0),%a0
	jmp	(%a0)
