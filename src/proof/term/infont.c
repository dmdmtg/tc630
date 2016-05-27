/*	Copyright (c) 1987 AT&T	*/
/*	All Rights Reserved	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


static char _2Vsccsid[]="@(#)infont.c	1.1.1.7 90/06/21 13:31:45";

/* includes */
#include <dmd.h>
#ifdef DMD630
#	include <5620.h>
#else
#	include <setup.h>
#endif
#include <dmdio.h>
#include <font.h>
#include "../include/comm.h"
#ifdef lint
#	include <lintdefs.h>
#endif /* lint */

/* defines */
#define ISIZE(n)	((n+1)*sizeof(Fontchar))
#define FCHARSIZE	8	/* size of Fontchar on host */

/*
 * read a font from an input stream
 * 	<font header>
 *	<f->info>
 *	<f->bits>	no bitmap header!!
 *
 * WARNING! This code believes it knows what the Font structure looks like
 */
#undef infont

/* procedures */
#ifdef DMD630
	extern char *alloc ();
	extern void sleep ();
	extern int own ();
	extern Bitmap *balloc ();
#endif /* DMD630 */


/*
 * If the slave wants to get the new font, it sends a GETFONT.
 * Otherwise, if there's been an error or the font is already present
 * (the later condition being handled by ldfont()), it sends an ACK.
 *
 * Note that (*inch)() can be, presumably, an unknown routine, in this
 * implementation, it is only inchar() (5/6/87).
 */
Font *
infont(inch,fno)
	register char (*inch)();
	int fno;
{
	short n;
	register Font *f;
	register Bitmap *b;
	char *temp;
	register int i;
	char *dummyp;
	/* make it maximum size to keep compiler quiet */
	char dummy[FCHARSIZE];

	/*
	 * if there's more font data on the host than is support
	 * on the jerq, store the rest here (bit bucket).
	 */
	dummyp = dummy;

	/*
	 * make sure we can loafd the font before we have the host pump it
	 * over!
	 */
	if((f = (Font *) alloc(sizeof(Font)+ISIZE(256))) == (Font *)0){
		/*
		 * this is the same response that ldfont gives if it
		 * has the requested font in its ftab.  Basically,
		 * we're just saying, "don't worry about it, host"
		 */
		send(ACK);
		send(fno);	/* shouldn't this send back 0? */
		debmsg("Out of program memory");
		sleep(100);
		debmsg("Out of program memory");
		return(f);	/* f == (Font *)0 */
	}
	if((b = balloc(Rect(0, 0, 1800, 37))) == (Bitmap *)0){
		/* got to get rid of allocated space */
		free(f);	/* Not enough space to read it in! */

		/*
		 * this is the same response that ldfont gives if it
		 * has the requested font in its ftab.  Basically,
		 * we're just saying, "don't worry about it, host"
		 */
		send(ACK);
		debmsg("Out of screen memory ");
		sleep(60);
		debmsg("Out of screen memory ");
		send(fno);	/* shouldn't this send back 0? */
		return((Font *)0);
	}
	else{
		f->bits = b;

		/*
		 * Tell host to start sending the font over
		 */
		send(GETFONT);
		send(fno);

		/*
		 * if after all that, the host sends anything but an
		 * ACK, it means it couldn't find the font after all.
		 *
		 * if infont() returns 0, ldfont() assumes that there
		 * was no room and deallocates the ftab entry for this
		 * font.  If infont() returns -1, ldfont() apparently
		 * assumes some kind of disasterous error, and does
		 * weird things.  If, however, the host says it doesn't
		 * have the font ... well, presumably the default font
		 * should be used.  In order to save having to understand
		 * all the downloading code, I'm going to use a
		 * (hopefully) unique retcode to tell ldfont() that it
		 * must fudge the default font.
		 */
		if(inchar() != ACK){
#ifdef DEBUG
/* JRG - debugging */
fpf("didn't get an ACK\n");
#endif
			/* dry run: deallocate all storage */
			bfree(f->bits);
			free(f);

			debmsg("-- missing");
			sleep(100);

			/* erase the message */
			debmsg("-- missing");
			return((Font *)-2);
		}

		/*
		 * I guess the purpose of the proceding was just to
		 * ensure that there would be enough room for the 
		 * following download.  What would happen if some
		 * other layer grabbed up alot after the following
		 * free?  That's probably what results in a -1 retcode.
		 */
		bfree(f->bits);
		free(f);
	}

	/*
	 * read the font arriving from the host
	 */


	/* get the number of chars in font (n) */
	temp = (char *)&n;
	if(ninch(2, &temp, inch))
		return((Font *)0);

#ifdef DEBUG
fpf("font has %d characters\n", n);
#endif

	/*
	 * allocate storage for Font font descriptor and chars.
	 * ISIZE accounts for the fact that fonts always have an
	 * extra entry in the Fontchar array by internally
	 * using n+1.
	 */
	if((f = (Font *) alloc(sizeof(Font)+ISIZE(n))) == (Font *)0)
		return(f);

	/*
	 * The 'n' stored in the font file is actually one greater
	 * than the number of character bitmaps in the font.  However,
	 * each font contains one dummy Fontchar array entry,
	 * whose purpose is to support the algorithm that decides
	 * where the last column in the font is.
	 */
	f->n = n-1;

	/* download height and ascent right into font descriptor */
	temp = 2 + (char *)f;
	if(ninch(6, &temp, inch))	/* 6==sizeof(height+ascent+unused) */
		goto err_freeAlloc;

#ifdef DEBUG
fpf("height = %d, ascent = %d, unused = %ld\n", f->height, f->ascent, f->unused);
#endif

	/*
	 * got the various descriptor data: now start to download the
	 * Fontchar char data info.  Note that although n is the number
	 * of character bitmaps in the font, this downloads n+1 Fontchar
	 * elements.
	 */
	temp = (char *)f->info;
	for(i=0; i<=n; i++)	/* changed for 5630 */
		if(ninch(sizeof(Fontchar), &temp, inch))
			/* there's storage to deallocate */
			goto err_freeAlloc;
		else {
#ifdef DEBUG
fpf("%d: %d,%d,%d,%d,%d ", i, f->info[i].x, f->info[i].top, f->info[i].bottom,
   f->info[i].left, f->info[i].width);
#endif
			temp += sizeof(Fontchar);

			/* dispose of excess */
			if(ninch(FCHARSIZE - sizeof(Fontchar), &dummyp, inch))
				goto err_freeAlloc;
		}

	/*
	 * Allocate bitmap for the actual font char data (changed for 5630).
	 *
	 * (x + 31) & 0xFFE0 is equivalent to (x + 31) / 32, and rounds
	 * up to the smallest sufficient number of 32 pixel units (i.e.
	 * a long int's worth).
	 */
	if ((b = balloc (Rect (0, 0, (f->info[n].x + 31) & 0xFFE0, f->height)))
	    == (Bitmap *)0)
		goto err_freeAlloc;

#ifdef DEBUG
fpf("\nRect = {%d,%d,%d,%d}\n", b->rect.origin.x, b->rect.origin.y, b->rect.corner.x, b->rect.corner.y);
#endif

	/*
	 * Download the char data
	 * Note that the width field in a Bitmap is the total width
	 * of the font in Words.
	 * Note that the font file must be different from the Font
	 * structure: in the structure, the Bitmap is defined
	 * *before* the info array, but this file data is clearly
	 * the other way around.
	 */
	f->bits = b;
	if (ninch (sizeof (Word) * f->height * b->width,
	    (char **)&(b->base), inch))
		goto err_freeBalloc;

	return(f);

	err_freeBalloc:
		debmsg("No alloc space for real!");
		sleep(1000);
		debmsg("No alloc space for real!");
		bfree(f->bits);
	err_freeAlloc:
		debmsg("No real screen space -kill this layer!");
		sleep(10000);
		debmsg("No real screen space -kill this layer!");
		free(f);
	return((Font *)-1);
}


/*
 * ninch (probably stands for INput N CHars) reads n chars from
 * the host (represented by inch()), and shoves them onto the
 * array *base.
 *
 * Returns 1 on EOF, 0 if operations successful.
 */
static
ninch(n, base, inch)
	register n;
	register char **base, (*inch)();
{
	register x, i;

	i = 0;
	while (n-- > 0) {
		/* get a char from the host */
		x = (*inch)();

		/* shove it onto the array pointed to by *base */
		(*base)[i++] = x;

		/* if there's no more chars available, give up */
		if(x == -1)
			return(1);

	}
	return(0);
}

void
ffree(f)
	register Font *f;
{
	if (f != (Font *) NULL) {
		bfree(f->bits);
		free(f);
	}
}

#ifdef LATER
outfont(f,ouch)
	register Font *f;
	register (*ouch)();
{
	register Bitmap *b = f->bits;

	f->n++;
	if(
		nouch(8,f,ouch) ||
		nouch(ISIZE(f->n),f->info,ouch) ||
		nouch(2*f->height*b->width,b->base,ouch)
	)
	{
		f->n--;
		return(-1);
	}
	else
	{
		f->n--;
		return(0);
	}
}

static
nouch(n,s,ouch)
	register n,(*ouch)();
	register char *s;
{
	do {
		if((*ouch)(*s++) == -1)
			return(-1);
	} while (--n > 0);
	return(0);
}

#endif /* LATER */

