/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)defs.c	1.1.1.3	(11/13/87)";

int	
	mpx,			/* 0 if standalone, 1 if mpx */
	wantready,		/* 0 if blast ahead, 1 if wait till READY */
	tojerq,			/* fd to jerq tty */
	fromjerq,		/* fd from jerq tty */
	lastx,			/* last x coordinate */
	lasty,			/* last y coordinate */
	hex_mode		/* 1 if 6-bit path, 0 if 8-bit */ 
;

float 
	torigy,			/* term orig y */
	torigx,			/* term orig x */
	porigy,			/* plot orig y */
	porigx,			/* plot orig x */
	scalex,			/* scale factor x */
	scaley,			/* scale factor y */
	deltx,			/* length of screen x */
	delty			/* length of screen y */
;	
