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
#include "dmdps.h"
#include "defs.h"

#define sendshort(w)	(putc(w&255, filep),putc((w>>8)&255, filep))
#define WID (r.corner.x-r.origin.x)

static Bitmap *braster = BNULL;
static short ctype, count; 
static short *p1;
static FILE *filep;

sendbitmap(bp,r,fileptr)
Bitmap *bp; 
Rectangle r; 
FILE *fileptr;
{
	short nrasters, rastwid; 
	Rectangle rrast;

	extern flush;

	filep = fileptr;
	nrasters = r.corner.y-r.origin.y;
	rastwid =(WID+15)/16;
	sendshort(nrasters); 
	sendshort(rastwid);

	if ( braster == BNULL) {
		if ( ( braster= balloc(Rect(0, 0, MAXWID, 1))) == BNULL )
			return 0;
		}

	rectf(braster,braster->rect,F_CLR);

	rrast=r;
	rectf(bp,r,F_XOR);
	showbitmap(bp,r);

	for (; rrast.origin.y<r.corner.y; rrast.origin.y++) {
		rrast.corner.y = rrast.origin.y+1;
		rectf(bp,rrast,F_XOR);
		bitblt(bp, rrast, braster, Pt(0,0), F_STORE);
		showbitmap(bp,r);
		sendrast(braster, Pt(0,0), rastwid);
		if (own()&MOUSE && bttn2() ) {
			iocntrlmenu();
			if (flush) {
				flush=0;
				rectf(bp,Rect(rrast.origin.x,rrast.origin.y+1,
					r.corner.x,r.corner.y),F_XOR);
				return 0;
			} 
		}
	}
	return 1;
}

static
sendrast(bmap, pnt, rastwid)
Bitmap	  *bmap;
Point	pnt;
short	rastwid;
{
	short *p2, *endraster;

	p1=p2=(short *)addr(bmap, pnt);
	endraster = p1 + rastwid - 1;

	do {
		if (p1 >= p2) {
			p2=p1+1; 
			count=2;
			ctype=(*p1 == *p2);

		} else if ((*p2 == *(p2+1)) == ctype) {
			if (++count >= 127) {
				sendbits();
				p1=p2+2;
			} else p2++;

		} else if (ctype) {
			sendbits();
			p1=p2+1;
			ctype=0;

		} else {
			count--; 
			sendbits();
			p1=p2;
			ctype=1;
		}
	} while (p2<endraster);

	if (p1 > endraster) return;
	if (p2 > endraster) count--;
	sendbits();
	fflush(filep);
	sync();
}

static sendbits()
{
	short c,i;
	c=count; 
	if (ctype) { 
		c += 128; 
		count=1; 
	}
	putc(c, filep);

	fwrite((char *)p1, 2*count, 1, filep);
}
