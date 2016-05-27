/* */
/*									*/
/*	Copyright (c) 1987,1988,1989,1990,1991,1992   AT&T		*/
/*			All Rights Reserved				*/
/*									*/
/*	  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.		*/
/*	    The copyright notice above does not evidence any		*/
/*	   actual or intended publication of such source code.		*/
/*									*/
/* */
#define NOFLOW

#define MAXWID	5000

#define EXIT	1

#define TRUE	1
#define FALSE	0

#define OP	0
#define SEL	1
#define RES	2

#define UP 0
#define DOWN 1
#define ACROSS 2

#define XMIN	8
#define YMIN	8

#define BNULL	(Bitmap *)0
#define PNULL	(struct Proc *)0
#ifdef DMD630
#define TNULL	(Texture16 *)0
#else
#define TNULL	(Texture *)0
#endif
#define INULL	(Texture16 *)0

#define LASTPOWER	12

#define HALTED	0x0400

#define NCHFILE	80
#define NCHPIPE	80
#define MAXRES  5

#define READFILE 	0
#define WRITEFILE	1
#define READPIPE	2
#define WRITEPIPE	3

#define VANISH	0x1
#define STIPPLE 0x2
#define REVVID	0x4
#define	STOPPED	0x8

#ifndef min
#define min(a, b) (a <= b ? a : b)
#endif
#define drstore(r) rectf(&display,r,F_STORE)
#define drflip(r) rectf(&display,r,F_XOR)
#define drclr(r) rectf(&display,r,F_CLR)
#define drstring(s,p) string(&defont,s,&display,p,F_XOR)
#define isbitset(bp,x,y) ((*addr(bp,Pt(x,y)) >> (WORDSIZE-x%WORDSIZE-1)) & 1)
#define LINE(place) add(place.origin,Pt(2,2))

#define Reshape() if(P->state&RESHAPED)redraw()

#ifndef DMD630
#define Sprintf sprintc
#endif
