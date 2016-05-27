
	global	remaplayer
remaplayer:
	mov.l	_addrSys,%a0
	mov.l	4*1161(%a0),%a0
	jmp	(%a0)
