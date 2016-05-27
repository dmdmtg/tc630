
	global	createswapspace
createswapspace:
	mov.l	_addrSys,%a0
	mov.l	4*1138(%a0),%a0
	jmp	(%a0)
