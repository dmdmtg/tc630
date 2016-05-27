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

#ifndef VALSCREENCOLOR
#ifndef DMD630
#include <setup.h>
#else
#define VALSCREENCOLOR	1
#endif
#endif

#include "pfd.h"
#include "printer.h"
#include "defs.h"

#define isbitset(bp,x,y) ((*addr(bp,Pt(x,y)) >> (WORDSIZE-x%WORDSIZE-1)) & 1)

Bitmap *braster = (Bitmap *) NULL;

char str[256];	/* holds formatting info */

printbmap(printer,bp,r)
struct Printerdefs *printer;	/* printer format description */
Bitmap *bp; 	/* printer to bitmap */
Rectangle r;	/* rectangle in bp to print */
{
	register i,j,k,c;	/* loops and outgoing char */
	register ybits, xbits, slicesize; 
	register Rectangle prect;	/* print rectangle */
	int pass, lastpass, size;
	int lastdot, fromdot;
	int q;

	extern autores, flush, hold;
	extern Texture16 *menu2;
	extern formp();			/* display. */
	int sc;

	sc = VALSCREENCOLOR;	/* screen color. match it. */

	if (braster != (Bitmap *) NULL) {
		bfree(braster);
	}
	if ((braster=balloc(Rect(0,0,MAXWID,printer->slicesize)))==BNULL)
		return 0 ;

	title("\f*start*");

	ybits=r.corner.y-r.origin.y;
	xbits=r.corner.x-r.origin.x;

	Sprintf(str,"\r\nxbits=%x, ybits=%x",xbits,ybits);
	title(str);

	/* get set for bitmap, find size index */
	if ( !autores ) { /* use 1-1 only. */
		size=0;
		title("\r\ninitstr:");
		dumpnchar(Sprintf(str,printer->initstr[size]),str);
		xbits=min(xbits,printer->width[size]);
		r.corner.x=r.origin.x+xbits;
	} else 
		for (size=0 ; (i=printer->width[size]) != 0 ; size++)
			if ( xbits <= i) {
				title("\r\ninitstr:");
				dumpnchar(Sprintf(str,printer->initstr[size]),str);
				break;
			}
	/* show area to be printed */
	prect=r;
	rectf(bp,prect,F_XOR);  
	showbitmap(bp,r);

	lastpass=printer->passes;

	/* for every so many bits down, */
	for (i=0; i < ybits ; i+=printer->slicesize)
	    /* for each pass, */
	    for (pass=1; pass<=lastpass; pass++){ 
		/* find the real number of bits down to print */
		slicesize=min(ybits-i,printer->slicesize); 

		if (pass == 1) { 
		    fromdot = 0;
		    lastdot = (lastpass==2 && xbits&0200) ? xbits&0177 : xbits;
		    switch (printer->upORdown) {
		    case ACROSS:
			q=Sprintf(str,printer->rowinit[size],(xbits+7)/8);
			break;
		    case UP:
			q=Sprintf(str,printer->rowinit[size],(lastdot-fromdot));
			break;
		    case DOWN: /* swab for the silly epson */
			q=Sprintf(str,printer->rowinit[size],
				(lastdot-fromdot)&0x00ff,
				((lastdot-fromdot)&0xff00)>>8);
			break;
		    }
		    title("\r\nrow init:");
		    dumpnchar(q,str);

		    /* uninvert screen image of slice */
		    prect.origin.x=r.origin.x; 
		    prect.origin.y=r.origin.y+i;
		    prect.corner.x=r.origin.x+xbits; 
		    prect.corner.y=r.origin.y+i+slicesize;
		    rectf(bp,prect,F_XOR);
		    if ( slicesize != 1 || (i%8) == 0 )
		    	showbitmap(bp,r);

		    /* ensure output matches what is on the screen. */
		    rectf(braster, braster->rect, F_CLR );

		    /* get copy of slice */ 
		    bitblt(bp, prect, braster, Pt(0,0), (sc?F_STORE:F_OR));

		    title("\r\nbits:");
		} else {
		    /* get set for second pass on slice */
		    fromdot = lastdot+1;
		    lastdot = xbits;
		    if (lastdot > fromdot){
		    	switch (printer->upORdown) {
		    	case ACROSS:
				q=Sprintf(str,printer->rowinit[size],
					(xbits+7)/8);
			    	break;
		    	case UP:
			    	q=Sprintf(str,printer->rowinit[size],
					(lastdot-fromdot));
				break;
		    	case DOWN: /* swab for the silly epson */
				q=Sprintf(str,printer->rowinit[size],
					(lastdot-fromdot)&0x00ff,
					((lastdot-fromdot)&0xff00)>>8);
				break;
		    	}
		    	title("\r\npass2 row init:");
		    	dumpnchar(q,str);
		    } 
		}
		if (printer->upORdown==ACROSS) {
			if (printer->outform[0] == '%' 
			    && printer->outform[1]=='c' 
			    && printer->slicesize == 1)
				dumpnchar(((xbits+7)/8),(char *)(braster->base)); 
			else
				for ( i=0; i<= ((xbits+7)/8);i++)
				    for ( j=0; j< slicesize;j++) {
					dumpnchar(Sprintf(str,printer->outform,
				  		((char *)braster->base)[i+j*braster->width]),str);
				    }
			if ( own()&MOUSE && bttn2() )
				if (myiocntrl(printer,bp,r,i,xbits,xbits,
						printer->slicesize) == 0)
					return 0;	
		} else
		    /* for every bit across, */
		    for (j=fromdot;j<lastdot;j++){ 
			/* for every bit down in the slice, */
			for (c=0x00, k=0; k<slicesize; k++) 
				/* if bit is set, */
				if (isbitset(braster,j,k))
					switch (printer->upORdown) { 
					case DOWN:
					    c|=(1<<(printer->slicesize-1-k));
					    break;
					case UP: 
					    c|=(1<<k); 
					    break;
					}
			if ( own()&MOUSE && bttn2() )
				if (myiocntrl(printer,bp,r,i,j,lastdot) == 0)
					return 0;	
			dumpchar((char) c + printer->fudge ); 
		    }
		if (pass==lastpass) {
			title("\r\ngraphic CR:");
			dumpnchar(Sprintf(str,printer->graphicCR),str);
#ifndef PAR
			sleep(5*printer->flowticks);
#endif
		}
		wait (CPU); /* give everybody else a chance to run */
	} 
	title("\r\nreset printer:");
	dumpnchar(Sprintf(str,printer->resetstr),str); /* reset printer */
	title("\r\n*exit*\n\r");
	return 1;
}

myiocntrl(printer,bp,r,i,j, lastdot)
struct Printerdefs *printer;
Bitmap *bp;
Rectangle r;
int i,j,lastdot;
{
	int z;

	iocntrlmenu();	
	if ( flush ) { 
		formp(INULL,&menu2,INULL, "flushing...");
		flush=0; /* turn off. */
		/* uninvert the remaining rectangle. */
		r.origin.y=r.origin.y+i+printer->slicesize;
		rectf(bp,r,F_XOR);
		/* finish out this line */
		for ( z=j;z<lastdot;z++)
			shipchar('\000');
		dumpnchar(Sprintf(str,printer->resetstr),str); 
		return 0; 
	}
	if ( hold ) {
		formp(INULL,&menu2,INULL, "holding...");
		while ( hold ) { 
			wait(CPU);
			if ( own()&MOUSE && bttn2() )
				iocntrlmenu();
		}
		formp(INULL,&menu2,INULL, "printing...");
	}
	return 1;
}
