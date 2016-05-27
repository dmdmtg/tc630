
	global	disableswapper
disableswapper:
	mov.l	_addrSys,%a0
	mov.l	4*1141(%a0),%a0
	jmp	(%a0)
