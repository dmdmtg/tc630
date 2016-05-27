/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)outfont.c	1.1.1.2 88/02/10 17:14:02";

/* includes */
#include	<dmd.h>
#include	<dmdio.h>
#include	<font.h>

/* defines */
#define ISIZE(n)	((n+1)*sizeof(Fontchar))

/* procedures */


/*
 * writes a standard format font out to whatever file ouch is
 * associated with.
 */
myoutfont(f,ouch)
	register Font *f;
	register (*ouch)();
{
	register Bitmap *b = f->bits;
	register int i = 0;
	int dummy = 0;

	/*
	 * For some reason, the library getfont() decrements the number of
	 * characters in the font file by 1 when it builds the font struct.
	 * Previously, jf() maintained this inaccuracy, which caused one
	 * too few characters from being displayed in graphic displays.
	 * From now on, 'n' will be incremented after a font is loaded,
	 * so that 'n' reflects the true number of characters in the font.
	 * So there is no need to *incremnt* n before outfont() is
	 * simulated here.
	 *
	f->n++;
	 */

	/* write out font preliminary 8 byte header */
	if( nouch(8,f,ouch) )
		i = -1;
	else {
		/* write out the info array */
		for(i=0; i<=f->n; i++)
			if(nouch(sizeof(Fontchar),&f->info[i],ouch) ||
			   ((8-sizeof(Fontchar)) &&
			   nouch(8-sizeof(Fontchar), &dummy, ouch)))
			{
				i = -1;
				break;
			}
		/* write out bitmap */
		if(nouch((WORDSIZE/8)*f->height*b->width,b->base,ouch))
			i = -1;
	}

	/*
	 * Now that n is being kept in jf to truly reflect the number
	 * of characters in the font, don't (re)decrement.
	f->n--;
	 *
	 */

	return(i);
}

static
nouch(n,s,ouch)
	register n,(*ouch)();
	register unsigned char *s;
{
	do {
		if((*ouch)(*s++) == -1)
			return(-1);
	} while (--n > 0);
	return(0);
}
