/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


/* static char _filename_sccsid[]="@(#)jplot.h	1.1.1.2	(11/13/87)"; */

/* types */
typedef struct Point {
	short	x;
	short	y;
} Point;

/* externals */
extern int	
	mpx,			/* 0 if standalone, 1 if mpx */
	wantready,		/* 0 if blast ahead, 1 if wait till READY */
	tojerq,			/* fd to jerq tty */
	fromjerq,		/* fd from jerq tty */
	lastx,			/* last x coordinate */
	lasty,			/* last y coordinate */
	hex_mode		/* 1 if 6-bit path, 0 if 8 */
;

extern void
	alpha(),		/* output a character in alphanumeric mode */
	arc(),			/* draw an arc */
	circle(),		/* draw a circle */
	closepl(),		/* close the plot */
	cont(),			/* draw to a point */
	delay(),		/* pause for 1 second */
	erase(),		/* erase the screen */
	fill(),			/* fill with a texture */
	finish(),		/* flush buffer, turn off READYs */
	graphic(),		/* output a character in graphics mode */
	label(),		/* output text */
	line(),			/* draw a line */
	linemod(),		/* change line drawing mode */
	move(),			/* move to a point */
	openpl(),		/* open a plot */
	space(),		/* define the user coordinates */
	start(),		/* turn on READYs */
	xysc()			/* scale, pack and output x and y coordinates */
;

extern float 
	torigy,			/* screen (term's) orig y */
	torigx,			/* screen (term's) orig x */
	porigy,			/* user's (plot's) orig y */
	porigx,			/* user's (plot's) orig x */
	scalex,			/* scale factor x */
	scaley,			/* scale factor y */
	deltx,			/* length of screen x */
	delty			/* length of screen y */
;
