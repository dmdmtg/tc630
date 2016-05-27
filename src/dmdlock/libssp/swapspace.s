
	global	swapspace
swapspace:
	mov.l	_addrSys,%a0
	mov.l	4*1164(%a0),%a0
	jmp	(%a0)
