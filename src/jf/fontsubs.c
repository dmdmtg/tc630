/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)fontsubs.c	1.1.1.4 88/02/24 13:44:08";

/* includes */
#include "jf.h"

/* procedures */


fdisphit(fdp, pc)	/* return selected character code, or -1 if none */
Fontdisp *fdp;		/* display graphic to check */
Point pc;		/* current mouse location */
{
	int c, row, col;

	/* if the mouse isn't inside this graphic, return -1 */
	if (!ptinrect(pc,fdp->r))
	    return -1;

	/*
	 * figure out exactly where in the graphic (i.e., over
	 * which character) the mouse is.
	 */
	col=(pc.x-fdp->r.origin.x)/fdp->cbx;
	row=(pc.y-fdp->r.origin.y)/fdp->cby;
	c=col + row*fdp->ncpl;

	/*
	 * If c turns out to be greater than the number of chars
	 * in the font, the user depressed the button whilst in
	 * the lower-right hand cavity of the graphic.  That's
	 * the same as not being in the graphic at all.  Otherwise
	 * return the selected character.
	 */
	return (c >= fdp->fp->n ? -1 : c);
}

fdispflp(fdp, c)	/* invert video at character c in display fdp */
    Fontdisp *fdp;
    int c;
{
	int row, col;
	Point pc;

	if (c < 0 || c >= fdp->fp->n)
	    return;

	col = c % fdp->ncpl;
	row = c / fdp->ncpl;

	/*
	 * add the window coordinates of the origin of the font display
	 * graphic to the relative position of 'c' in the font display
	 * graphic (in two dimensions).  This is the origin of the tiny
	 * square to be inverted.
	 */
	pc = add (fdp->r.origin, Pt(col*fdp->cbx, row*fdp->cby));

	/*
	 * This is going to actually do the invertion.  I guess cbx and
	 * cby include WBORD.  Thus, the multiplication above yielded
	 * a point below the top border and to the right of the left
	 * boarder, and the subtraction below will yield a point above
	 * the bottom border, and to the left of the right border.
	 */
	drflip(Rpt(pc, add(pc, Pt(fdp->cbx-WBORD, fdp->cby-WBORD))));
}

fdispstr (fdp, c)		/* redraw character c in display fdp */
    Fontdisp *fdp;
    int c;
{
	int row, col;
	Point pc;
	Rectangle r, charect();
	Font *fp;

	fp=fdp->fp;
	if (c < 0 || c > fp->n)
	    return;

	col = c % fdp->ncpl;
	row = c / fdp->ncpl;
	pc = add (fdp->r.origin, Pt(col*fdp->cbx, row*fdp->cby));

	drclr (Rpt (pc, add (pc, Pt (fdp->cbx-WBORD, fdp->cby-WBORD))));

	r = charect (fp, c);
	bitblt (fp->bits, r, &display, add (pc, Pt (WMARG, WMARG)), F_OR);
}

/*
 * determine the dimensions of the font display graphic.
 */
fontsetrect (fdp, r)
    register Fontdisp *fdp;	/* structure describing loaded font */
    Rectangle r;		/* jf window */
{
	int nl;		/* number of lines */
	int cdx, cdy;
	Font *fp;

	fp = fdp->fp;
	fdp->mwidth = maxwidth(fp);

	/*
	 * Apparently, the character is centered in the cell, bounded
	 * by a margin of WMARG pixels on each side.  Since the cell
	 * will be adjacent to the cells on either side of it, add the
	 * width (height) of *one* border post.
	 */
	cdx=fdp->mwidth+2*WMARG;
	fdp->cbx=cdx+WBORD;

	cdy=fp->height +2*WMARG;
	fdp->cby=cdy+WBORD;

	/*
	 * now take the total width of the window, reduce it by the
	 * width of the single remaining border post (naturally, there
	 * is n+1 seperaters to the graphic), and divide the result
	 * by the total width of each single character.
	 * This will be the maximum of character cells that will fit
	 * inside of the font display graphic.
	 * Reduce it by one more so it's not too tight.
	 */
	fdp->ncpl=(r.corner.x - r.origin.x - WBORD)/fdp->cbx - 1;

	/* make sure really big fonts can still be displayed */
	if (fdp->ncpl <= 0)
	    fdp->ncpl = 1;

	/* figure out how many lines are necessary (round up) */
	nl=(fp->n + fdp->ncpl - 1)/fdp->ncpl;

	/*
	 * now figure out where the corner point of the font display
	 * graphic will lie.  The origin point was presumably passed
	 * to fontsetrect() in the Fontdisp structure rectangle r
	 * field.  That was presumably known from the position of the
	 * mouse... no, it appears that this rectangle is in window
	 * coordinates, so the origin point is simply 0,0.
	 * But the corner is determined below.  Note that this corner
	 * is probably of the enclosing rectangle, even though the
	 * font display graphic is usually not a rectangle.
	 * Note that ncpl, the number of characters per line, can be
	 * greater - for small fonts - than the number of characters
	 * in the font.  Also for fonts that contain few characters.
	 * That is the reason that when the number of lines needed
	 * is not greater than 1, the actually n from the Font structure
	 * is used, instead of ncpl.
	 */
	fdp->r.corner =
	    add (fdp->r.origin,
		Pt ((nl>1? fdp->ncpl : fp->n)*fdp->cbx, nl*fdp->cby));
}

fontdisp(fdp)	/* display font */
register Fontdisp *fdp;
{
	int nl, c, ic, cdx, cdy; Rectangle r;
	Point pc; Rectangle rc; Font *fp; Fontchar *fc;

	fp = fdp->fp;
	r = fdp->r;

	cdx=fdp->mwidth+2*WMARG; fdp->cbx=cdx+WBORD;
	cdy=fp->height +2*WMARG; fdp->cby=cdy+WBORD;

	drclr(r);
	drstore(
	  Rect(r.origin.x-WBORD,r.origin.y-WBORD,r.corner.x,r.origin.y));
	drstore(
	  Rect(r.origin.x-WBORD,r.origin.y-WBORD,r.origin.x,r.corner.y));

	pc=r.origin;
	ic=0;
	rc.origin.y=0; rc.corner.y=fp->height;

	for (c = fp->n, fc = fp->info; c > 0; c--, fc++) {

		drstore(Rect(pc.x    ,pc.y+cdy,pc.x+     cdx,pc.y+fdp->cby));
		drstore(Rect(pc.x+cdx,pc.y    ,pc.x+fdp->cbx,pc.y+fdp->cby));

		rc.origin.x=fc->x; rc.corner.x=(fc+1)->x;
		bitblt(fp->bits,rc,&display,
		    add(pc,Pt((fc->left&0x80 ? 0 : fc->left)+WMARG,WMARG)),
		    F_STORE);

		if (++ic < fdp->ncpl) {
			pc.x += fdp->cbx;
		} else {
			pc.x  = r.origin.x;
			pc.y += fdp->cby;
			ic    = 0;
		}
	}

	return 1;
}

maxwidth(fp)	/* returns width of widest character */
Font *fp;
{
	register Fontchar *ich, *nch;
	register mw, w;

	/* nch points to last Fontchar */
	nch = fp->info + fp->n;

	mw = nch->width;

	for (ich=fp->info; ich < nch; ich++) {
		w = ich->width;

		/*
		 * if left is negative, subtract it from width.
		 * This will *increase* the size of width.
		 */
		if ((ich->left & 0x80) != 0)
#			ifdef DMD630
				w -= ich->left;
#			else
				w -= ich->left - 0x100;
#			endif

		if (mw < w)
			mw = w;

		/*
		 * This is unique.
		 * Apparently, repeat the process, but this time, use
		 * the width as implied in x, the "left edge of bits".
		 * If this is wider than the value stored explicitly
		 * stored in Fontchar.width, use it instead.
		 */
		w = (ich+1)->x - ich->x;

		/*
		 * This is what is really interesting.  When using
		 * the *implied* width, if left is positive, then it
		 * should be added to the width.  Does this mean that
		 * when left is negative, the character extends off
		 * the baseline to the left, and when it is positive,
		 * it extends off the baseline to the right?
		 */
		if ((ich->left & 0x80) == 0)
			w += ich->left;

		if (mw < w)
			mw = w;
	}
	return mw;
}

fontgrow (fp, c, width)	/* adjust physical width of character in font */
	Font *fp;
	int c;
	int width;	/* maximum character width */
{
	int n;
	int newx;		/* new corner.x of font? */
	int dwidth;		/* diff between max-width and width of c */
	int xc;			/* x of c */
	int xcp;		/* x of character following c */
	Rectangle r;		/* rectangle associated with font */
	Fontchar *fc;
	Bitmap *newbits;

	n = fp->n;
	if (c<0 || c>=n) {
	    return 0;
	}

	fc = fp->info + c;
	xc = (fc++)->x;
	xcp = fc->x;		/* fc now points to info[c+1] */

	/*
	 * Determines the amount the font will have to be widened.
	 * xcp - xc is the width of c.  "width" is really mwidth
	 * because it's only ever called from editsetrect() with
	 * fdp->mwidth.
	 */
	if ((dwidth = width - xcp + xc) == 0) {
	    return 1;		/* character position already at widest */
	}

	r = fp->bits->rect;

	/*
	 * get a new bitmap that better fits the changed font.
	 *
	 * The Font data structure maintains a value called 'n' that
	 * represents the number of characters in the bitmap.  The font
	 * file also maintains a number called 'n', but this 'n'
	 * represents the number of info array elements.  There is one
	 * more info array element than characters in the bitmap.
	 *
	 * The font file does not maintain the width of bitmap scanlines.
	 * This information is derived from the 'x' field of the last
	 * info array element.
	 */
	newx = fp->info[n].x + dwidth;
	newbits = balloc(Rpt(r.origin,Pt((newx+31)&0xFFE0,r.corner.y)));

	if (newbits == (Bitmap *)0) {
	    return 0;
	}

	/* clear the new bitmap. */
	rectf (newbits, newbits->rect, F_CLR);

	/*
	 * the boundary between the edited character, and the following
	 * character will partition the bitmap in two parts.  The part
	 * to the left of this boundary will be left adjusted in the
	 * bitmap, and the part to the right will be right adjust,
	 * leaving a gap of dwidth bits.
	 */
	xc = xcp;

	if (dwidth < 0) {
	    xc += dwidth;
	}

	/*
	 * move the left part of the font - including the bits pertaining
	 * to the original character to be edited - into the expanded
	 * bitmap, left-adjusted.
	 */
	bitblt(fp->bits, Rpt (r.origin, Pt (xc, r.corner.y)),
		newbits, r.origin, F_OR);

	/*
	 * now move the right part of the font - starting with the first
	 * character to the right of the edited character - into the new
	 * font, right-adjusted.
	 */
	bitblt (fp->bits, Rpt(Pt (xcp, r.origin.y), r.corner),
		newbits, Pt (xcp+dwidth, r.origin.y), F_OR);

	/* replace the old bitmap with the new one in the font structure */
	bfree(fp->bits);
	fp->bits = newbits;

	/*
	 * now modify all info array elements to the right of the edited
	 * character so that their 'x' corresponds to their new positions
	 */
	for (; c <= n; c++) {
	    (fc++)->x += dwidth;
	}

	return 1;
}

fontprune(fp)	/* squeeze font to physical char width */
Font *fp;
{
	int nch, wch, wbits;
	Rectangle r, rch, rsupport(); Bitmap *bp, *bscratch; Fontchar *ich;

	bscratch=balloc(Rect(0,0,maxwidth(fp),fp->height));
	if (bscratch == (Bitmap *)0) return 0;
	rch.origin.y=0; rch.corner.y=fp->height;

	wbits=0;
	for (nch=fp->n,ich=fp->info; nch>0; nch--,ich++) {
		rch.origin.x= ich   ->x;
		rch.corner.x=(ich+1)->x;
		r=rsupport(fp->bits,rch,bscratch);
		ich->width = r.corner.x-r.origin.x;
		ich->top   =r.origin.y;
		ich->bottom=r.corner.y;
		wbits += r.corner.x-r.origin.x;
	}
	if (wbits >= fp->bits->rect.corner.x) return 1;

	bp=balloc(Rect(0,0,max(1,(wbits+31)&0xFFE0),fp->height));
	if (bp == (Bitmap *)0) return 0;
	rectf(bp,bp->rect,F_CLR);

	wbits=0;
	for (nch=fp->n,ich=fp->info; nch>0; nch--,ich++) {
		rch.origin.x= ich   ->x;
		rch.corner.x=(ich+1)->x;
		r=rsupport(fp->bits,rch,bscratch);
		ich->x = wbits;
		if ((wch=r.corner.x-r.origin.x) > 0) {
			ich->left += r.origin.x-rch.origin.x;
			rch.origin.x=r.origin.x;
			rch.corner.x=r.corner.x;
			bitblt(fp->bits,rch,bp,Pt(wbits,0),F_STORE);
			wbits += wch;
		} else {
			ich->left = 0;
		}
	}

	ich->x = wbits;
	bfree(fp->bits); fp->bits=bp; bfree(bscratch);

	return 1;
}

Font *
fontrange(fp, newn)	/* set ascii range of font */
	Font *fp;
	int newn;
{
	Font *newfp;
	int i, oldn, smalln, w;
	Bitmap *newb;

	/* ensure that the new value is legitimate */
	newn=max(1, min(256, newn));

	/* subscript of important, dummy, last info array element */
	oldn=fp->n;

	/*
	 * save the old maxwidth value (stored in the last,
	 * dummy info array element for safe keeping) for
	 * later installation in the new font.
	 */
	w = fp->info[oldn].width;

	/*
	 * The purpose of this step is apparently to allow the dummy
	 * element to be copied to the new font, even if it'll be
	 * longer than the old one.  In that case, all fields of the
	 * dummy font will be zero, except for the 'x' field, whose
	 * value has to be propogated through all the new, empty cells
	 * anyway.
	 */
	fp->info[oldn].width = 0;

	/* allocate a font for the new size */
	newfp=(Font *)alloc(sizeof(Font) + newn*sizeof(Fontchar));
	if (newfp == FNULL)
	    return FNULL;

	/*
	 * if the new font will be shorter than the original font,
	 * than a new bitmap must be generated.  This is apparently
	 * not so in the other case.  In that case, when a character
	 * is editted, editsetrect() calls fontgrow(), which ensures
	 * that the bitmap is large enough to handle the full allowable
	 * character width.  There is no "fontshrink()" routine.
	 */
	if (newn < oldn) {
		newb = balloc(Rect(0, 0, max(1, (fp->info[newn].x+31)&0xFFE0), fp->height));
		if (newb == 0) {
		    free(newfp);
		    return FNULL;
		}

		/* copy as much as possible from the old font to the new */
		bitblt(fp->bits, newb->rect, newb, Pt(0,0), F_STORE);

		/* release the old bitmap */
		bfree(fp->bits);

		/*
		 * store the pointer to the new bitmap in the old font
		 * in preparation for the subsequent structure assignment.
		 */
		fp->bits = newb;
	}

	/*
	 * This is a structure assignment. The following fields
	 * will be assigned:
	 *	short n;		/* number of chars in font
	 *	char height;		/* height of bitmap
	 *	char ascent;		/* top of bitmap to baseline
	 *	long unused;		/* in case we think of more stuff
	 *	Bitmap *bits;		/* where the characters are
	 *	Fontchar info[1];	/* n+1 character descriptors
	 *
	 * Notes:
	 * - The new n will be subsequently inserted.
	 * - If the new n is less than the old n, then the new bit map
	 *   was inserted in fp before the assignment, so newfp will
	 *   have the correct value.  If the new n is greater than the
	 *   old n, than the old bit map will continue to be used(!), so
	 *   in that case fp-> is also correct.
	 * - That apparently the storage for the old info array is lost.
	 * - One element of the info array will also be assigned, the
	 *   rest in a subsequent loop.
	 */
	*newfp = *fp;
	newfp->n = newn;

	/*
	 * move the remaining info array elements from the old font
	 * to the new one, or as many as will fit, including
	 * the once or future dummy element.
	 *
	 * if the new font is longer than the old, the dummy element
	 * of the old will be copied to the new font.  This is okay
	 * because only the 'x' field is non-zero at this point.  This
	 * is because the 'width' field was zero-ed in an earlier step.
	 * The 'x' field must be propogated in that case through all
	 * the remaining new cells.
	 */
	smalln = min(oldn, newn);
	for (i=1; i<=smalln; i++)
	    newfp->info[i] = fp->info[i];

	/*
	 * if the new font is longer than the old, than the unused
	 * characters must have their 'x' fields set to that of the
	 * last, dummy, info array element.
	 */
	for (   ; i<=newn; i++)
	    newfp->info[i].x = fp->info[oldn].x;

	/*
	 * the last, dummy entry contains interesting values only
	 * in the 'x' and 'width' fields.  The 'x' field is used
	 * to determine the real width of the last character, and
	 * the 'width' field is used to carry the maxwidth value,
	 * I think.
	 */
	newfp->info[newn].top=0;
	newfp->info[newn].bottom=0;
	newfp->info[newn].left=0;
	newfp->info[newn].width=w;

	/* dispose of the old font */
	free((char *) fp);

	return newfp;
}

fontheight(fp,h)	/* alter height of font */
Font *fp; int h;
{
	Bitmap *bp;

	h=max(1,min(255,h)); bp=balloc(Rect(0,0,fp->bits->rect.corner.x,h));
	if (bp == (Bitmap *)0) return 0;
	rectf(bp,bp->rect,F_CLR);
	bitblt(fp->bits,fp->bits->rect,bp,Pt(0,0),F_STORE);
	bfree(fp->bits); fp->bits=bp;
	fp->height=h;
	fp->ascent=min(fp->ascent,h);
	return 1;
}

Rectangle
charect(fp, c)	/* return rectangle enclosing character */
    Font *fp;
    int c;
{
	Fontchar *ich;
	Rectangle r;

	ich=fp->info + c;
	r.origin.x= ich   ->x; r.origin.y=0;
	r.corner.x=(ich+1)->x; r.corner.y=fp->height;

	return r;
}
