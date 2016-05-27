/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)charsubs.c	1.1.1.4 88/02/24 13:44:07";

/* includes */
#include "jf.h"

/* procedures */


bitfunc(fcode)		/* perform bitblt operation between characters */
int fcode;
{
	int dstc; Point psrc, pdst, pprev;
	Rectangle rsrc, charect();
	Font *srcfp, *dstfp; Fontdisp *dstfdp; Editdisp *srcedp, *dstedp;

	if (fcode < 0) return 0;

	cursswitch(&target);
	if (buttons(DOWN) == 3) mousetrack();
	else { buttons(UP); cursswitch(TNULL); return 0; }
	if (mtk.disp == DNULL) return 0;
	if (mtk.disp->Class != EditClass) return 0;
	srcedp=mtk.disp->Disp.TopEdit; psrc=mouse.xy;

	pprev=psrc;
	for (;wait(MOUSE);nap(2)) {
		if (!ptinrect(mouse.xy,Drect)) continue;
		pdst=mouse.xy;
		if (!eqpt(pprev,pdst)) {
			segment(&display,psrc,pprev,F_XOR);
			pprev=pdst;
			segment(&display,psrc,pprev,F_XOR);
		}
		if (!button3()) { mousetrack(); break; }
	}
	segment(&display,psrc,pdst,F_XOR);
	cursswitch(TNULL);
	if (mtk.disp == DNULL) return 0;
	if (mtk.disp->Class != EditClass) return 0;
	dstedp=mtk.disp->Disp.TopEdit;

	srcfp=srcedp->fdp->fp; rsrc=charect(srcfp,srcedp->c);

	dstc=dstedp->c; dstfdp=dstedp->fdp; dstfp=dstfdp->fp;
	pdst=charect(dstfp,dstc).origin;

	bitblt(srcfp->bits,rsrc,dstfp->bits,pdst,fcode);
	fdispstr(dstfdp,dstc); fdispflp(dstfdp,dstc);
	editdisp(dstedp);
	return 1;
}

shiftfunc(rcode)		/* shift/rotate character bitwise */
{
	int bselect, c; Rectangle r, charect();
	Fontdisp *fdp; Font *fp; Editdisp *edp; 

	if (rcode < 0) return 0;

	cursswitch(arrows[rcode]); rcode *= 2;

	for (;;) {
		bselect=buttons(DOWN)-1; buttons(UP);
		if (bselect > 1) break;
		mousetrack();
		if (mtk.disp == DNULL) continue;
		if (mtk.disp->Class != EditClass) continue;
		edp = mtk.disp->Disp.TopEdit;

		fdp=edp->fdp; fp=fdp->fp; c=edp->c;
		r=charect(fp,c);

		switch (bselect+rcode) {
			case 0:
				brol(fp->bits,r); break;
			case 1:
				bror(fp->bits,r); break;
			case 2:
				brou(fp->bits,r); break;
			case 3:
				brod(fp->bits,r); break;
			case 4:
				bhflip(fp->bits,r); break;
			case 5:
				bvflip(fp->bits,r); break;
			case 6:
				bitblt (fp->bits, Rect(fp->info[c].x, 
					    fp->bits->rect.origin.y,
				            fp->info[c+1].x,
					    fp->bits->rect.corner.y),
				    fp->bits,
				    add (fp->bits->rect.origin, Pt(1,1)),
				    F_STORE);
			case 7:
				bitblt (fp->bits, Rect(fp->info[c].x, 
					    fp->bits->rect.origin.y,
				            fp->info[c+1].x,
					    fp->bits->rect.corner.y),
				    fp->bits,
				    sub (fp->bits->rect.origin, Pt(1,1)),
				    F_STORE);
				/*
				btrans(fp->bits,r); break;
				*/
		}

		fdispstr(fdp,c); fdispflp(fdp,c);
		editdisp(edp);
	}
	return 1;
}

EXTERN Point DrectOrigin;

charop (fdp, c)		/* open character for editing */
	Fontdisp *fdp;
	int c;
{
	Rectangle MoveFrame();
	Editdisp *newedp();

	Editdisp *edp, *prevedp;
	Rectangle r;
	Font *fp;
	Fontchar *ich;
	Point center;

	/*
	 * move down font display's open edit cell list, looking for 'c'.
	 */
	for (prevedp=EDNULL, edp=fdp->edp;
	     edp != EDNULL;
	     prevedp=edp, edp=edp->edp)
		if (c == edp->c) break;

	/*
	 * if the character is already open, we're finished
	 */
	if (edp!=EDNULL)
	    return 0;

	/* a couple of useful local variables */
	fp=fdp->fp;
	ich=fp->info+c;

	/*
	 * allocate an edisp table entry for a new edit display graphic.
	 * edp.r is initialized to {0,0,0,0}.
	 */
	if ((edp = newedp ()) == EDNULL)
	    return 0;

	/*
	 * Fill in some of the fields of edp.
	 * Terminate linked list of character edit cells.
	 */
	edp->edp = EDNULL;

	edp->fdp = fdp;
	edp->c = c;

	/*
	 * grow the font to ensure room for maxwidth bits,
	 * normalize any positive left, and determine the
	 * size of the edit cell (I think).
	 */
	if (!editsetrect (edp)) {
	    edp->fdp = FDNULL;
	    return 0;
	}

	/* invert the tiny square within the font display graphic */
	fdispflp(fdp,c);

	/* wait for the user to release after character selection */
	while (button123())
	    /* empty */;

	/*
	 * the edit cell graphic will be deposited so that it is centered
	 * over the spot where that the user selects.
	 */
        center = div(sub(edp->r.corner,edp->r.origin),2);

	while (!ptinrect (mouse.xy, Drect))
		wait (MOUSE);

	/*
	 * move the new edit cell graphic into position.
	 */
	edp->r = MoveFrame (raddp(edp->r, sub(mouse.xy, center)), 0x00);

	/*
	 * basically, I think this just does the display of the
	 * edit cell itself, without messing with the bitmap.
	 */
	editdisp(edp);

	/*
	 * put the new display graphic on the list of edit cells for
	 * this font.
	 */
	if (prevedp == EDNULL)
	    fdp->edp = edp;
	else
	    prevedp->edp=edp;

	/* put the new display graphic on the list of display graphics */
	Pool = NewEditPool(edp, Pool);

	return 1;
}

charcl (fdp, c)		/* close character for editing */
	Fontdisp *fdp;
	int c;
{
	Rectangle charect(), rsupport();
	Editdisp *newedp();
	Disp *FindEditInPool();

	Editdisp *edp, *prevedp;
	Rectangle r;
	Font *fp;
	Fontchar *ich;
	Point center;

	for (prevedp=EDNULL, edp=fdp->edp;
	     edp != EDNULL;
	     prevedp=edp, edp=edp->edp)
		if (c == edp->c)
		    break;

	if (edp==EDNULL)
	    return 0;

	fp=fdp->fp;
	ich=fp->info+c;

	r=rsupport (fp->bits, charect (fp, c), (Bitmap *)0);
	ich->top=r.origin.y;
	ich->bottom=r.corner.y;

	if (prevedp == EDNULL)
	    fdp->edp = edp->edp;
	else
	    prevedp->edp = edp->edp;

	fdispflp (edp->fdp, edp->c);

	Pool = RemoveFromPool (FindEditInPool (edp, Pool), Pool);

	edp->fdp=FDNULL;
	Redraw();

	return 1;
}

Point
edisphit(edp,pc)	/* return selected pixel, or -1 if none */
Editdisp *edp;
Point pc;
{
	Point p;

	if (ptinrect (pc, Rpt (
			edp->r.origin,
			sub (edp->r.corner, Pt (1, edp->size))))) {
		p=div(sub(pc,edp->r.origin),edp->size);
	} else {
		p.x = -1;
		p.y = -1;
	}
	return(p);
}

edispset(edp,pxl,bit)	/* set value of pixel */
    Editdisp *edp;		/* selected character cell */
    Point pxl;		/* location of mouse when called */
    int bit;		/* state of button when called */
{
	int c, *waddr, wbit, row, col;
	Point p;
	Font *fp;
	Fontdisp *fdp;

	c=edp->c;	/* character to be editted */
	fdp=edp->fdp;	/* owning font display graphic */
	fp=fdp->fp;	/* owning font */

	p.x=fp->info[c].x + pxl.x;
	p.y=pxl.y;

	waddr=addr(fp->bits,p);
	wbit = 1<<(WORDSIZE - 1 - p.x%WORDSIZE);

	if (((*waddr & wbit) != 0) == bit)
	    return 0;

	*waddr ^= wbit;

	col=c%fdp->ncpl;
	row=c/fdp->ncpl;
	p=add(fdp->r.origin,Pt(WMARG+col*fdp->cbx,WMARG+row*fdp->cby));
	p=add(p,pxl);
	drflip(Rpt(p,add(p,Pt(1,1))));

	p=add(edp->r.origin,mul(pxl,edp->size));
	drflip(Rpt(p,add(p,Pt(edp->size,edp->size))));

	return 1;
}

editsetrect (edp)	/* set the rectangle of a character */
	Editdisp *edp;
{
	Fontdisp *fdp;
	int c, size;
	Font *fp;
	Fontchar *ich;

	fdp  = edp->fdp;
	fp   = fdp->fp;
	size = edp->size;	/* expansion factor */
	c    = edp->c;
	ich  = fp->info + c;

	/*
	 * fontgrow() ensures that the full font character width is
	 * available for the new character.  It must be closed up
	 * again to the necessary character size when charcl()
	 * closes the edit cell.
	 */
	if (!fontgrow (fp, c, fdp->mwidth))
	    return 0;

	/*
	 * if there is a positive "left", normalize it by shifting
	 * the character,  in its owning font, to the right by the
	 * amount of the "left".
	 *
	 * (This could have been done more efficiently as:
	 *	(ich->left & 0x7f) != 0
	 */
	if (ich->left && (ich->left & 0x80) == 0) {
		bitblt (fp->bits,
			Rect (ich->x, 0, (ich+1)->x - ich->left, fp->height),
			fp->bits,
			Pt (ich->x + ich->left, 0),
			F_STORE);
		rectf (fp->bits,
			Rect (ich->x, 0, ich->x + ich->left, fp->height),
			F_CLR);
		ich->left = 0;
	}

	/*
	 * determine the rectangle containing the edit cell.
	 * edp->r.origin is set to the nullrect in newedp().
	 * mwidth is calculated in maxwidth() (called by fontsetrect())
	 * by checking each character in the font to find the widest one.
	 * Size is set from edp->size, which is set in Resize().
	 *
	 * I don't yet understand exactly why 1 is add to height below,
	 * and to r.corner.x, subsequently, but I know that when you
	 * leave the former +1 off, a bottom line is left behind when
	 * you close the edit cell, and when you leave the latter +1
	 * off, a right line is left behind when you close the edit cell.
	 */
	edp->r.corner = add (edp->r.origin,
			     mul (Pt (fdp->mwidth, 1+fp->height), size));

	edp->r.corner.x += 1;

	return 1;
}

editdisp(edp)		/* display character in large format */
	register Editdisp *edp;
{
	Fontdisp *fdp;
	int c,size;
	int i;
	Rectangle r, charect();
	Font *fp;
	Fontchar *ich;

	fdp  = edp->fdp;
	fp   = fdp->fp;
	size = edp->size;
	c    = edp->c;
	ich  = fp->info + c;

	/* clear the space to be used for the edit cell */
	drclr (edp->r);

	magnify (fp->bits,
		charect (fp, c),
		&display,
		edp->r.origin,
		Pt (size, size));

	r.origin = edp->r.origin;
	r.corner = add (r.origin, Pt (fdp->mwidth*size, 1));

	for (i = 0; i<=fp->height; i++) {
		drflip(r);
		r = raddp(r,Pt(0,size));
	}

	r.origin = edp->r.origin;
	r.corner = add (r.origin, Pt (1, fp->height*size));

	for (i = 0; i<=fdp->mwidth; i++) {
		drflip(r);
		r = raddp (r, Pt (size, 0));
	}

	r.origin = add(edp->r.origin, Pt(0, ((2*fp->ascent+1)*size-WBORD)/2));
	if (ich->left & 0x80)
		r.origin.x += (0x100 - ich->left)*size;
	r.corner = add(r.origin, Pt(max(ich->width*size, size/2),WBORD));

	drflip(r);
}
