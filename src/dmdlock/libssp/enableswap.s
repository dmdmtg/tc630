
	global	enableswapper
enableswapper:
	mov.l	_addrSys,%a0
	mov.l	4*1145(%a0),%a0
	jmp	(%a0)
