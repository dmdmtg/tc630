/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)rsupport.c	1.1.1.2 88/02/10 17:14:03";

/* includes */
#include <dmd.h>

/* procedures */


/*
 * This routine returns the minimal rectangle bounding the object
 * in Bitmap bits at rectangle r.
 *
 * Note that r selects a subset of the domain represented by bits.rect.
 * bits is the bitmap associated with the font - NOT the bitmap of an
 * edit cell (i.e. Not the enlarged version).  r is a rectangle of the
 * owning font which corresponds to the object.
 */
Rectangle
rsupport(bits, r, bscratch)	/* returns "support" of rectangle in bitmap */
	Bitmap *bits, *bscratch;
	Rectangle r;		/* rectangle that encloses char in font */
{
	Rectangle rsup;
	Bitmap *bproj;
	int xsize, ysize;
	unsigned d;
	Word *w;

	/*
	 * start with an infinitely small square with origin at
	 * the origin of the character cell in the font's bitmap.
	 */
	rsup.corner = rsup.origin = r.origin;

	/*
	 * figure out the size, apparently in pixels, of the left
	 * half of the charater cell (r).  Round up.  My question is,
	 * when is (r.corner.y - r.origin.y) != r.corner.y?
	 */
	xsize = (r.corner.x-r.origin.x+1)/2;
	ysize =  r.corner.y-r.origin.y;

	/*
	 * if the edit cell is only one dimensional, return the
	 * infinitely small - essentially null - enclosing
	 * rectangle.
	 */
	if (xsize == 0 || ysize == 0)
	    return rsup;

	/*
	 * apparently, bscratch is bitmap space that can be used
	 * if it's available.  Otherwise, rsupport() allocates
	 * working space here.  Note that this looks like an area
	 * that may be a source of problems if bitmaps of the
	 * wrong size are used.
	 */
	bproj = bscratch;
	if (bproj == (Bitmap *)0)
	    bproj=balloc(Rect(0, 0, xsize, ysize));

	/*
	 * if there's no work space, return the original edit
	 * cell.  This is proabably the worst case storage
	 * utilization situation.
	 */
	if (bproj == (Bitmap *)0)
	    return r;

	/*
	 * Sqeeze all vertical bits into first column.
	 */

	/*
	 * move the left half of the character cell into the work space.
	 *
	 *	   r.origin.y	 r.corner.y
	 *		|             |
	 *		V 01234 5678  |
	 * ------------> +-----+----+ |
	 * r.origin.x	1|     |    | |
	 *		2|     |    | |
	 *		3|     |    | |
	 *		4|     |    | |
	 *		5|     |    | |
	 *		 +-----+----+ V
	 * -------------------------->
	 * r.corner.x
	 * -------------------->
	 * r.corner.x + xsize
	 */
	bitblt (
	    bits,
	    Rpt (r.origin, Pt (r.origin.x + xsize, r.corner.y)),
	    bproj,
	    Pt (0, 0),
	    F_STORE);

	/*
	 * now OR the right half of the character cell into the
	 * same workspace.
	 */
	bitblt (
	    bits,
	    Rpt (Pt (r.origin.x + xsize, r.origin.y), r.corner),
	    bproj,
	    Pt (0, 0),
	    F_OR);

	/*
	 * now continue dividing the workspace into two, and OR
	 * the right side onto the left until just one column is left.
	 */
	while (xsize>1) {
		d=(xsize+1)/2;
		bitblt (
		    bproj,
		    Rect (d, 0, xsize, ysize),
		    bproj,
		    Pt (0, 0),
		    F_OR);
		xsize=d;
	}

	/*
	 * Look for bottom of character to set rsup.corner.y
	 */

	/*
	 * w will point to the word that contains the lowest bit position
	 * (along the y axis) of column 1 in the squeezed version 
	 * of the input bitmap.  Since it is the leftmost bit of its
	 * scanline, it is left-adjusted in the word.  This is because
	 * the bitmap itself is left-adjusted (i.e. the top-leftmost
	 * bit is), and every scanline is always an integral number of
	 * words.  Thus, the leftmost bit of every scanline is left-
	 * adjusted.
	 *
	 * But if r is a subset of bits.rect, there's no reason to
	 * believe, however, that the bit is left-adjusted.  So how
	 * does this work? ... Ah, but bproj is a *new* bitmap that
	 * IS left-adjusted - the bits of rectangle r were not left-
	 * adjusted in bits, but they are in bproj.
	 */
	w = addr (bproj, Pt (0, ysize));

	/*
	 * with regards to my earlier question:
	 *	when is (r.corner.y - r.origin.y) != r.corner.y?
	 * note that the following assumes that there is no difference.
	 * The bitmap bproj was created as (0, 0, xsize, ysize), but
	 * r.corner.x and r.corner.y are used to address the scanlines.
	 */
	for (d = r.corner.y; d > r.origin.y; d--) {
		w -= bproj->width;
		/*
		 * the first "on" bit found from the bottom marks the
		 * lowest bound of the character.
		 */
		if (*w & FIRSTBIT) {
			rsup.corner.y=d;
			break;
		}
	}

	/*
	 * infinitely small rectangle.  Once again, I'd like to point
	 * out that it seems that to be consistant, this check should
	 * be for rsup.corner.y == rsup.origin.y.  Instead, the assumption
	 * is made that rsup.origin.y is 0.  If that is a valid
	 * assumption, why was ysize set at the beginning?
	 */
	if (rsup.corner.y == 0)
	    return rsup;

	/*
	 * Look for top of character to set rsup.origin.y
	 */

	/*
	 * w will point to the word that contains the highest bit position
	 * (along the y axis) of column 1 of the squeezed version of the
	 * input bitmap.
	 */
	w = bproj->base;

	/*
	 * now look for the highest "on" bit in the squeezed rectangle.
	 * Note that the model has changed again: rsup and r coordinates
	 * are used instead of 0, xsize, and ysize.
	 */
	for (rsup.origin.y = r.origin.y;
	     rsup.origin.y < r.corner.y;
	     rsup.origin.y++) {
		if (*w & FIRSTBIT)
		    break;
		w += bproj->width;
	}

	/*
	 * we're apparently done with the bitmap now.  If it was
	 * allocated locally, free it.
	 */
	if (bscratch == (Bitmap *)0)
	    bfree(bproj);

	/*
	 * now we're apparently going to repeat the process, this time
	 * for the x axis.
	 */

	/*
	 * the y coordinates of rsup have been calculated.  Now
	 * initialize the x coordintes.  Of course, they were already
	 * initialized at the first line of rsupport(), but there's
	 * apparently extra cycles that need to be burned up.
	 *
	 * No, that may have been unduly harsh.  The initialization
	 * at the beginning sets both rsup.origin and rsup.corner to
	 * r.origin.  The following clause sets rsup.corner.x to
	 * r.corner.x.  Why this wasn't done at the beginning is still
	 * a puzzle to me.
	 */
	rsup.origin.x=r.origin.x;
	rsup.corner.x=r.corner.x;

	/* figure the dimensions of the workspace bitmap */
	xsize =  r.corner.x-r.origin.x;
	ysize = (r.corner.y-r.origin.y+1)/2;

	bproj = bscratch;
	if (bproj == (Bitmap *)0)
	    bproj=balloc(Rect(0,0,xsize,ysize));

	if (bproj == (Bitmap *)0)
	    return rsup;

	/*
	 * move the top half of the character cell into the work space.
	 *
	 *	   r.origin.y	 r.corner.y   r.corner.y + ysize
	 *		|             |		|
	 *		V 01234 5678  |		|
	 * ------------> +----------+ |		|
	 * r.origin.x	0|          | |		|
	 *		1|          | |		|
	 *		2|          | |		V
	 *		3+----------+ |
	 *		4|          | |
	 *		5|          | |
	 *		 +----------+ V
	 * -------------------------->
	 * r.corner.x
	 */
	bitblt (
	    bits,
	    Rpt (r.origin, Pt (r.corner.x, r.origin.y+ysize)),
	    bproj,
	    Pt (0, 0),
	    F_STORE);

	/* now OR the bottom half onto the workspace */
	bitblt (
	    bits,
	    Rpt (Pt (r.origin.x, r.origin.y+ysize), r.corner),
	    bproj,
	    Pt (0, 0),
	    F_OR);

	/* now squeeze the bitmap down to one row */
	while (ysize>1) {
		d=(ysize+1)/2;
		bitblt (
		    bproj,
		    Rect(0, d, xsize, ysize),
		    bproj,
		    Pt (0, 0),
		    F_OR);
		ysize=d;
	}

	/*
	 * look for right edge of the character to set rsup.corner.x
	 */

	/*
	 * w will point to the word that contains the rightmost bit
	 * of the workspace.
	 */
	w = addr (bproj, Pt (xsize, 0));

	/*
	 * d will be a bitmask to select the rightmost bit of the
	 * bitmap from the word that contains it.
	 * Note that it starts out corresponding to the first bit
	 * to the right of the bitmap.  It is shifted left before
	 * it is used.
	 */
	d = FIRSTBIT >> (xsize % WORDSIZE);

	/*
	 * starting from the righthand side of the bit map, shift
	 * the bitmask to the left - over word boundaries - until
	 * the first "on" bit is encounted.
	 */
	for (rsup.corner.x=r.corner.x;
	     rsup.corner.x>r.origin.x;
	     rsup.corner.x--) {
		if ((d<<=1) == 0) {
		    w--;
		    d=LASTBIT;
		}
		if (*w & d)
		    break;
	}

	/*
	 * look for left edge of the character to set rsup.origin.x
	 */
	w=bproj->base;

	d=FIRSTBIT;

	for (
	    rsup.origin.x = r.origin.x;
	    rsup.origin.x < r.corner.x;
	    rsup.origin.x++) {
		if (*w & d)
		    break;

		if ((d>>=1) == 0) {
		    w++;
		    d=FIRSTBIT;
		}
	}

	/*
	 * again, if the second workspace was allocated locally,
	 * free it here.
	 */
	if (bscratch == (Bitmap *)0)
	    bfree(bproj);

	return rsup;
}
