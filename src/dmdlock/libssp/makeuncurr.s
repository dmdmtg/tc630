
	global	makeuncurrent
makeuncurrent:
	mov.l	_addrSys,%a0
	mov.l	4*1153(%a0),%a0
	jmp	(%a0)
