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
 *	bitmap convert filter  
 *
 *	convert a bitmap from DMDPS format to BITFILE.
 *
 *
 */

#include <stdio.h>
#include "udmd.h"
#include "bit_h.h"

#define SHORTSIZE	16

int readtype, writetype;

short 	readbuffer[MAXWID*2]; 	/* in the new format, we keep a historical */
short 	writebuffer[MAXWID*2]; 	/* account of the previous rasters that we xor in. */

writehdr( out, bhp )
FILE *out;
struct bithead *bhp;
{
	int i;

	if (bhp->type == NEWFMT) {
		putw( 0,out );	/* write initial 0		*/
		putw( bhp->r.origin.x,out );/* write rectangle origin*/
		putw( bhp->r.origin.y,out );
		putw( bhp->r.corner.x,out );/* write rectangle corner*/
		putw( bhp->r.corner.y,out );
	} else {
		putw( bhp->nrasters,out );	/* write height		*/
		putw( bhp->rastwid,out );	/*  & width		*/
	}	
	writetype=bhp->type;
	for( i=0 ; i<MAXWID ; i++ ) writebuffer[i]=0;
}

fillhdr(bhp)
struct bithead *bhp;
{
	if ( (bhp->nrasters == 0) || (bhp->rastwid == 0) ) {
		bhp->nrasters = bhp->r.corner.y - bhp->r.origin.y;
		bhp->rastwid = bhp->r.corner.x - bhp->r.origin.x;
		bhp->rastwid = (bhp->rastwid+SHORTSIZE-1)/SHORTSIZE;
	} else {
		bhp->r.origin.x=0;
		bhp->r.origin.y=0;
		bhp->r.corner.x=bhp->rastwid*16;
		bhp->r.corner.y=bhp->nrasters;
	}
}

readhdr( in, bhp )
FILE *in;
struct bithead *bhp;
{
	int i;

	if ( (bhp->type = getw( in )) == EOF ) return EOF;
	if ( bhp->type == NEWFMT ) { 
		if ( (bhp->r.origin.x = getw( in )) == EOF ) return EOF;
		if ( (bhp->r.origin.y = getw( in )) == EOF ) return EOF;
		if ( (bhp->r.corner.x = getw( in )) == EOF ) return EOF;
		if ( (bhp->r.corner.y = getw( in )) == EOF ) return EOF;
		bhp->rastwid=bhp->nrasters=0;
		fillhdr(bhp);
	} else {
		bhp->nrasters = bhp->type ; 
		bhp->type = OLDFMT;
		if ( (bhp->rastwid  = getw( in )) == EOF ) return EOF;
		fillhdr(bhp);
	}
	for( i=0 ; i<MAXWID ; i++ ) readbuffer[i]=0;
	return(readtype=bhp->type);
}

putrast(in, buff,nw)
FILE	*in;
short	*buff;
int	nw;
{
	int i;

	if (writetype == NEWFMT) {
		for (i=0;i<nw;i++)
			writebuffer[i] ^= buff[i];
		putrerast(in, (short *)writebuffer, nw);
		for (i=0;i<nw;i++)
			writebuffer[i] = buff[i];
	} else
		putrerast(in, buff, nw);
}

getrast(in, buff,nw)
FILE	*in;
short	*buff;
int	nw;
{
	int i;

	if (readtype == NEWFMT) {
		if ( getrerast(in, buff, nw) == EOF) return EOF;
		for (i=0;i<nw;i++)
			buff[i] = (readbuffer[i] ^= buff[i]);
		return(1);
	} else
		return getrerast(in, buff, nw);
}

putw(i, out)
short	i;
FILE	*out;
	{
	putc(i & 0xff, out);		/* put out low byte of value */
	putc((i >> 8) & 0xff, out);	/* high byte of value */
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

putrerast( f, buff,wid )
FILE *f;
short *buff;
int wid;
{
	int ctype, count;
	short *p1;
	register short *p2;
	p1=p2=buff;
	do{
		if(p1>=p2){
			p2=p1+1;
			count=2;
			ctype = *p1==*p2;

		}
		else if((*p2==*(p2+1))==ctype){
			if(++count>=127){
				putbits(f,count,ctype,p1);
				p1=p2+2;
			}
			else
				p2++;

		}
		else if(ctype){
			putbits(f,count,ctype,p1);
			p1=p2+1;
			ctype=0;

		}
		else{
			count--;
			putbits(f,count,ctype,p1);
			p1=p2;
			ctype=1;
		}
	} while ( p2 < buff+wid-1 );
	if ( p1 <= buff+wid-1 ){
		if( p2 > buff+wid-1 )
			count--;
		putbits(f,count,ctype,p1);
	}
}

static putbits(f,count,ctype,p1)
FILE *f;
int count, ctype;
short *p1;
{
	int c;
	c=count;
	if(ctype){
		c+=128;
		count=1;
	}
	putc(c, f);
	fwrite((char *)p1, 2, count, f);
}


getrerast( f,p,nwords)	/* receive single BITFILE raster */
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
				return 1;
			for(i=1;i!=count;i++){
				*(p+1) = *p;
				p++;
			}
			p++;
		}
		else{
			if(fread(p, sizeof(short), count, f)!=count)
				return 1;
			p+=count;
		}
	}
	return(nwords!=0);
}
