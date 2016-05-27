/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


/* static char _filename_sccsid[]="@(#)but.h	1.1.1.1	(10/7/87)"; */


#define	bublack		0xC
#define bucolor 	0xC
#define	budkgray 	0x8
#define	bufixedpt	0x1C0
#define	bufc		0x100
#define	bufL		0x000
#define	bufl		0x040
#define	bufr		0x080
#define	bufU		0x000
#define	buicup	 	0x30
#define bultgray 	0x4
#define	bunugae		0x2
#define buselected 	0x1
#define buwhite 	0x0

typedef 	struct	Button {
	short	flags;		/* state of button	*/
	Font	*font;		/* font for text	*/
	Bitmap	*icon	[4];	/* pictures for button	*/
	Point	ior;		/* origin of icon	*/
	Rectangle rect;		/* area of button	*/
	char	*text;		/* text for button	*/
}	Button	;
Button	*newButton ()	;
Button	*newButWH ()	;
Button	*newButXY ()	;
#define	Pcpy(p,q)	p.x = q.x;  p.y = q.y
#define	Rcpy(r,s)	Pcpy (r.origin, s.origin);  Pcpy (r.corner, s.corner)
