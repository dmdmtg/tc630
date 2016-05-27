
	global	getcurrentssp
getcurrentssp:
	mov.l	_addrSys,%a0
	mov.l	4*1146(%a0),%a0
	jmp	(%a0)
