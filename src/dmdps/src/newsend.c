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

/*
 *
 *  write bitmap for painting program  --  brush/writemap.c
 *  newsend bitmap for dmdps program  --  dmdps/src/newsend.c
 *
 *	NOTE:	dmdps (J5620) code courtesy of Andy Schnable
 *		blitblt (JERQ) code courtesy of Tom Duff
 *
 */

#include "dmdps.h"

#define SHORTSIZE	16
#define sendword(w, f)	(putc(w&255, f), putc((w>>8)&255, f))

extern Word readbuffer[], raster[];
extern Bitmap bbuffer;
extern int ctype, count;
extern short *p1, *endraster;

newsendbitmap(bp, r, f)
Bitmap *bp;
Rectangle r;
FILE *f;
{
	register i, j, nrasters, rastwid, nrword;
	Rectangle rrast;
	Bitmap *compl;
	
	compl=bp;

	nrasters=r.corner.y-r.origin.y;
	i=r.corner.x-r.origin.x;
	rastwid=(i+SHORTSIZE-1)/SHORTSIZE;
	nrword=(i+WORDSIZE-1)/WORDSIZE;
	endraster=(short *)raster+rastwid-1;

	sendword( 0,f );		/* 0 1st word for some silly reason	*/
	sendword( r.origin.x,f );
	sendword( r.origin.y,f );
	sendword( r.corner.x,f );
	sendword( r.corner.y,f );

	rectf(&bbuffer, bbuffer.rect, F_CLR);
	for(i=0;i<nrword;i++)
		raster[i]=0;
	rrast=r;
	if(compl)
		rectf(compl, r, F_XOR);
	showbitmap(bp,r);
	for(;rrast.origin.y<r.corner.y;rrast.origin.y++){
		rrast.corner.y=rrast.origin.y+1;
		if(compl)
			rectf(compl, rrast, F_XOR);
		if ( (MOUSE&bttn1()) || (( rrast.origin.y%16) == 0) ) 
			showbitmap(bp,r);
		bitblt(bp, rrast, &bbuffer, Pt(0, 0), F_STORE);
		for(i=0;i<nrword;i++)
			raster[i]^=readbuffer[i];
		putrast(f);
		for(i=0;i<nrword;i++)
			raster[i]=readbuffer[i];
		if (own()&MOUSE && bttn2() ) {
			extern flush;

			iocntrlmenu();
			if (flush) {
				rectf(bp,Rect(rrast.origin.x,rrast.origin.y+1,
					r.corner.x,r.corner.y),F_XOR);
				return 0;
			}
		}
	}
	return 1;
}

static putrast(f)
FILE *f;
{
	register short *p2;
	p1=p2=(short *)raster;
	do{
		if(p1>=p2){
			p2=p1+1;
			count=2;
			ctype = *p1==*p2;

		}
		else if((*p2==*(p2+1))==ctype){
			if(++count>=127){
				putbits(f);
				p1=p2+2;
			}
			else
				p2++;

		}
		else if(ctype){
			putbits(f);
			p1=p2+1;
			ctype=0;

		}
		else{
			count--;
			putbits(f);
			p1=p2;
			ctype=1;
		}
	}while(p2<endraster);
	if(p1<=endraster){
		if(p2>endraster)
			count--;
		putbits(f);
	}
}

static putbits(f)
FILE *f;
{
	int c, i;
	c=count;
	if(ctype){
		c+=128;
		count=1;
	}
	putc(c, f);
	fwrite((char *)p1, sizeof *p1, count, f);
}
