#
# BE CAREFUL to only use relative instructions, and no data.
#  You can check it with 'mc68dump -hr'.
#
# This could be a very simple program loaded at the beginning of downloaded
#   code except for one problem: putting code at the beginning causes the
#   P->text pointer to not point to the beginning of the user's code and
#   thus process exceptions and dmdpi addresses would be off.
# Therefore this complex approach is taken instead:
#   1. This patch is loaded at the beginning of the download IN THE PLACE OF
#	the code that normally would be downloaded there.
#   2. The code that would normally be at the beginning of .text is instead
#	placed at the end of .data in an extra chunk allocated by dmdld.
#	Two pointers are also placed at the end by dmdld: one to where the
#	beginning of .bss is supposed to be and one to where the end of .bss
#	is supposed to be.
#   3. When this patch starts up, it does what it's purpose is: send a char
#	back to dmdld so dmdld knows the download is done.  A newline is used
#	as the handshake character because for some reason dmdld on SunOS
#	over TCP non-layers does not receive anything until a newline is sent.
#   4. After it is done with it's important work, it does cleanup work to
#	get itself out of the way:
#	a. Copies the remainder of itself onto the end of .bss and jumps
#	    there.  There is extra space at the end of .bss because the
#	    extra at the end of .data that dmdld had allocated will become
#	    part of .bss by the time we're through.  Also, dmdld makes sure
#	    that the size of .bss is at least as large as the extra stuff
#	    at the end of .data so the end of .bss won't overlap with it.
#	b. Copies the real code from the end of .data to the beginning of
#	    .text where it belongs.
#	c. Zeros out the space at the end of .data because it will become
#	     part of .bss which must be clear.
#	d. Moves P->bss back to where it belongs.
#	e. Jumps back to the beginning of .text.
#
# - Dave Dykstra, 8/6/93
#
patchpart1:
	bsr	gethere
	mov.l	%a0,%a4		# save pointer to beginning in %a4
#
# Send a character to mark the end of a download
#
	mov.l	176,%d0		# find the address of the vector table
	add.l	&6,%d0		#  (_addrSys). How this is done is documented
	mov.l	%d0,%a0		#  in $Fw/xvt.s in the firmware.
	mov.l	(%a0),%a0

	mov.l	-4*38(%a0),%a5	# save 'P' pointer for later in %a5

	mov.l	4*113(%a0),%a0	# get address of 'sendchar'
 	mov.w	&10,-(%sp)	# send newline to mark end of download
 	jsr	(%a0)
 	add.l	&2,%sp	
#
# Copy patchpart2 to end of .bss
#
	mov.l	4*3(%a5),%a0	# %a0 = P->bss
	mov.l	-4(%a0),%a1	# get pointer of where to copy to
	mov.l	%a1,%a2		# save in %a2 for jump
	mov.l	-8(%a0),%a3	# get beginning of original code, save in %a3
	mov.l	%a0,%d0
	sub.l	%a3,%d0		# calculate length of whole patch + pointers
	mov.l	%d0,%d2		# save in %d2 for part 2
	sub.l	&4*2,%d0	# subtract the extra for the 2 pointers
	bsr	getpatchpart2	# get address of where to copy from into %a0

	mov.l	%a0,%d1		# subtract the length of patchpart1
	sub.l	%a4,%d1
	sub.l	%d1,%d0
	
	bsr	blkcpy		# copy the block

	jmp	(%a2)		# jump to new patchpart2

#
# gethere - return pointer to bsr instruction that called this in %a0
#
gethere:
	mov.l	(%sp),%a0	# get addr of instruction after bsr
	sub.l	&4,%a0		# subtract back over bsr
	rts

# ---------------------------------------------------------------------
#
# Part 2 of the patch - part relocated to end of .bss
#

patchpart2:
#
# Copy original .text back to the beginning
#
	mov.l	%a3,%a0		# where to copy from
	mov.l	%a4,%a1		# copy to beginning
	mov.l	%d2,%d0		# length
	sub.l	&4*2,%d0	# not the 2 pointers
	bsr	blkcpy
#
# Zero beginning of .bss
#
	mov.l	%a3,%a0		# where to zero
	mov.l	%d2,%d0		# length
	bsr	zero

#
# Fix up P->bss
#
	sub.l	%d2,4*3(%a5)	# P->bss -= %d2
#
# Finished!  Jump back to the beginning
#
	jmp	(%a4)
	

#
# Return address of patchpart2
# It has to be after the 'patchpart2' label even though it isn't used in
#   part2 because the assembler won't allow access to the 'patchpart2'
#   value before it is defined
#
getpatchpart2:
	bsr	gethere
	sub.l	&getpatchpart2-patchpart2,%a0
	rts

#
# blkcpy - copy %d0 bytes from %a0 to %a1
# THIS MUST BE in patchpart2 so it gets copied along with the second part
blkcpyloop:
        mov.b   (%a0)+,(%a1)+
blkcpy:
        sub.w   &1,%d0
        bge     blkcpyloop
        rts
#
# zero - zero %d0 bytes at %a0
# THIS MUST BE in patchpart2 so it gets copied along with the second part
zeroloop:
        clr.b   (%a0)+
zero:
        sub.w   &1,%d0
        bge     zeroloop
        rts
