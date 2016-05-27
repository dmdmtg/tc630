
	global	destroyswapspace
destroyswapspace:
	mov.l	_addrSys,%a0
	mov.l	4*1140(%a0),%a0
	jmp	(%a0)
