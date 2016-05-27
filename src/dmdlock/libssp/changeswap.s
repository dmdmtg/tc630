
	global	changeswapspace
changeswapspace:
	mov.l	_addrSys,%a0
	mov.l	4*231(%a0),%a0
	jmp	(%a0)
