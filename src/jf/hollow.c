/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)hollow.c	1.1.1.2 88/02/10 17:13:58";

/* includes */
#include "jf.h"

/* procedures */


pixel (b, p)
Bitmap *b;
Point p;
{
  Word *w;
  unsigned int bit;

  w = addr (b, p);
  bit = FIRSTBIT >> (p.x%WORDSIZE);
  return (*w&bit) == bit;
}

showpix(fdp,c,pxl,bit)	/* set value of pixel */
Fontdisp *fdp;
Point pxl; int bit;
{
	register int *waddr, wbit, row, col;
	register Point p; 
	Font *fp;

	fp=fdp->fp;

	p.x=fp->info[c].x + pxl.x; p.y=pxl.y;

	waddr=addr(fp->bits,p);
	wbit = 1<<(WORDSIZE - 1 - p.x%WORDSIZE);

	*waddr ^= wbit;

	col=c%fdp->ncpl;
	row=c/fdp->ncpl;
	p=add(fdp->r.origin,Pt(WMARG+col*fdp->cbx,WMARG+row*fdp->cby));
	p=add(p,pxl);
	point (&display, p, F_XOR);

	return 1;
}

hollow ()
{
  Fontdisp *fdp;
  Font *f;
  register int i, x, y;
  Rectangle r;
  Bitmap *b;

  if (mtk.disp == DNULL) {
    return (0);
  }
  if (mtk.disp->Class != FontClass) {
    return (0);
  }
  fdp = mtk.disp->Disp.TopFont;
  f = fdp->fp;
  r = f->bits->rect;
  r.corner.x = fdp->mwidth;
  if ((b = balloc (r)) == (Bitmap *) NULL) {
    return (0);
  }
  
  /* For each character, */
  for (i=0; i<f->n; i++) {
    if (f->info[i].width > 0) {
      rectf (b, b->rect, F_CLR);
      /* Copy the character, */
      bitblt (f->bits, 
	      Rect (f->info[i].x, b->rect.origin.y,
		    f->info[i+1].x, b->rect.corner.y),
	      b,
	      b->rect.origin,
	      F_STORE);
      /* Test surrounding pixels, */
      for (y=1; y<b->rect.corner.y-1; y++) {
	for (x=1; x<b->rect.corner.x-1; x++) {
	  if (pixel (b, Pt(x-1, y-1)) &&
	      pixel (b, Pt(x-1, y  )) &&
	      pixel (b, Pt(x-1, y+1)) &&
	      pixel (b, Pt(x  , y-1)) &&
	      pixel (b, Pt(x  , y  )) &&
	      pixel (b, Pt(x  , y+1)) &&
	      pixel (b, Pt(x+1, y-1)) &&
	      pixel (b, Pt(x+1, y  )) &&
	      pixel (b, Pt(x+1, y+1))) {
	    /* And reset current pixel if totally surrounded. */
	    showpix(fdp,i,Pt(x, y),0);	/* set value of pixel */
	  }
	  wait (CPU);
	}
      }
    }
  }
  Redraw ();
}
