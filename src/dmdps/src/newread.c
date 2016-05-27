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
 *  read bitmap for painting program  --  brush/readmap.c
 *
 *	NOTE:	blitblt code courtesy of Tom Duff 
 *
 */

#include "dmdps.h"
#include "bit_h.h"

extern struct bithead bh;

#define mydebug(s)

#define SHORTSIZE	16
#define sendword(w, f)	(putc(w&255, f), putc((w>>8)&255, f))
Word readbuffer[1+MAXWID/WORDSIZE], raster[1+MAXWID/WORDSIZE];
Bitmap bbuffer={ readbuffer, (sizeof readbuffer)/(sizeof(Word)), 0, 0, MAXWID, 1};
int ctype, count;
short *p1, *endraster;

getword(stream)		/* snarfed from J5620 version */
FILE *stream;
{
	register l, h;

	if ((l=getc(stream)) == EOF)
		return (EOF); 
	if ((h=getc(stream)) == EOF)
		return(EOF);
	return (h&255)<<8 | (l&255);
}

newreadbitmap( f )
FILE *f;
{
	extern Bitmap *bp;
	register h, w, i, j;
	Rectangle r;

	bp=balloc(Rect(0, 0, SHORTSIZE*bh.rastwid, bh.nrasters));
	if( bp == (Bitmap *) NULL ) 
		return(0);

	rectf(&bbuffer, bbuffer.rect, F_CLR);
	showbitmap(bp,bp->rect);

	for(i=0;i!=bh.nrasters;i++){
		if(getrast(f, (short *)raster, bh.rastwid)){
			bfree(bp);
			return(-1);
		}
		for(j=0;j!=bp->width;j++)
			readbuffer[j]^=raster[j];
		bitblt(&bbuffer, bbuffer.rect, bp, Pt(0, i), F_STORE);
		if ( (MOUSE&bttn1()) || ((i%16) == 0)) 
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
	return(1);
}

static getrast(f, p, nwords)	/* receive single compressed raster */
FILE *f;
register short *p;
int nwords;
{
	register i, count;
	for(;nwords>0;nwords-=count){
		if((count=getc(f))<=0)
			return 1;
		if(count&128){
			count&=127;	/* count must not be zero, not checked */
			if(fread(p, sizeof(short), 1, f)!=1)
				return (EOF);
			for(i=1;i!=count;i++){
				*(p+1) = *p;
				p++;
			}
			p++;
		}
		else{
			if(fread(p, sizeof(short), count, f)!=count)
				return EOF;
			p+=count;
		}
	}
	return(nwords!=0);
}
