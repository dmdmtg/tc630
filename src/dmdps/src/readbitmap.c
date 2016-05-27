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
#include "bit_h.h"
#define SHORTSIZE 16

extern Bitmap *bp;
struct bithead bh;

readhdr(stream)
FILE *stream;
{
	if ( (bh.type=getw(stream)) == 0 ) {
		bh.r.origin.x = getword(stream);
		bh.r.origin.y = getword(stream);
		bh.r.corner.x = getword(stream);
		bh.r.corner.y = getword(stream);
		bh.nrasters = bh.r.corner.y - bh.r.origin.y;
		bh.rastwid = bh.r.corner.x - bh.r.origin.x;
		bh.rastwid = (bh.rastwid+SHORTSIZE-1)/SHORTSIZE;
	} else {
		bh.nrasters=bh.type; 
		bh.rastwid=getw(stream);
		bh.type=1;
		bh.r.origin.x = 0;
		bh.r.origin.y = 0;
		bh.r.corner.x=bh.rastwid*SHORTSIZE;
		bh.r.corner.y=bh.nrasters;
	}
}	

readbitmap(stream)
FILE *stream;
{

	readhdr(stream);
	if ( bh.type == 0 )
		return newreadbitmap(stream);
	else
		return oldreadbitmap(stream);
}

oldreadbitmap(stream)
FILE	*stream;
{
	register i;
	short rc, nrasters, rastwid;

	if ( (bp=balloc(Rect(0,0,bh.rastwid*16,bh.nrasters))) == (Bitmap *) NULL )
		return 0;
	showbitmap(bp,bp->rect);
	for ( i =0 ; (i <nrasters) 
		     && ((rc=readrast(bp,i,bh.rastwid,stream)) !=  EOF); i++) {
		if (MOUSE&bttn1() || ( (i%16) == 0))
			showbitmap(bp,bp->rect);
		if (own()&MOUSE && bttn2() ) {
			extern int flush;

			iocntrlmenu();
			if (flush) {
				flush=0;
				return 2;
			} 
		}
	}
	return rc;
}

readrast(bp,i,nw,in)
Bitmap *bp;
int i;
int	nw;
FILE	*in;
{
	int	count, ctype;
	char	*bits;

	bits=(char *)addr(bp,Pt(0,i));
	while (nw>0) {
		count=getc(in);
		count &= 255;
		ctype=count&128;
		count &= 127;
		nw -= count;
		count *= 2;

		if (ctype) {
			if (fread(bits,2,1,in) != 1)
				return (EOF);
			for (count-=2; count>0; count--) {
				*(bits+2) = *bits;
				bits += 1;
			}
			bits += 2;
		} else {
			if (fread(bits,count,1,in) != 1)
				return (EOF);
			bits += count;
		}
	}
	return (1);
}

getw(stream)
FILE *stream;
{
	register l, h;

	if ((l=getc(stream)) == EOF)
		return (EOF); 
	if ((h=getc(stream)) == EOF)
		return(EOF);
	return (h&255)<<8 | (l&255);
}

