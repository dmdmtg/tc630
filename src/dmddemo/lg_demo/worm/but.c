/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)but.c	1.1.1.1	(10/7/87)";


/* Button code	*/
#include <dmd.h>
#include <font.h>
#include "but.h"
#ifdef DMD630
#include <5620.h>
#define texture16 texture
#endif

#define	check(msg)

short	XPEL	= 5	;	/* x pixels bordering text	*/
short	YPEL	= 5	;	/* y pixels bordering text	*/

Texture16 dkgraymap = {
	0xEEEE, 0xBBBB, 0xEEEE, 0xBBBB, 0xEEEE, 0xBBBB, 0xEEEE, 0xBBBB,
	    0xEEEE, 0xBBBB, 0xEEEE, 0xBBBB, 0xEEEE, 0xBBBB, 0xEEEE, 0xBBBB
};
Texture16 ltgraymap = {
	0x1111, 0x0000, 0x4444, 0x0000, 0x1111, 0x0000, 0x4444, 0x0000,
	    0x1111, 0x0000, 0x4444, 0x0000, 0x1111, 0x0000, 0x4444, 0x0000
};

centerx (r)
Rectangle r;
{
	return r.origin.x + ((r.corner.x - r.origin.x)>>1);
}

centery (r)
Rectangle r;
{
	return r.origin.y + ((r.corner.y - r.origin.y)>>1);
}

height (r)
Rectangle r;
{
	return r.corner.y - r.origin.y;
}

width (r)
Rectangle r;
{
	return r.corner.x - r.origin.x;
}

Button *
newButton (label, pt, fixed, b)
char *label;
Point pt;
char *fixed;
Button *b;
{
	return newButXY (label, pt, fixed, XPEL, YPEL, b);
}

Button *
newButWH (label, pt, fixed, wid, hgt, b)
char *label;
Point pt;
char *fixed;
short wid, hgt;
Button *b;
{
	return newButXY (label, pt, fixed,
	    ((wid-jstrwidth (label))>>1),
	    ((hgt-defont.height)>>1), b);
}

Button *
newButXY (label, pt, fixed, xpel, ypel, b)
char *label;
Point pt;
char *fixed;
short xpel, ypel;
Button *b;
{
	Point	bo, bc;

	    b->flags = 0;
	    b->flags |= buwhite;
	    b->flags &= bunugae;
	    b->flags &= buselected;
	    b->font = &defont;
	    b->text = label;
	    b->icon [0] = (Bitmap *)0;
	    b->icon [1] = (Bitmap *)0;
	    b->icon [2] = (Bitmap *)0;
	    b->icon [3] = (Bitmap *)0;

	    if (fixed [0]=='c')
	    {
		b->flags |= bufc;
		    bo.x = pt.x - ((jstrwidth (label)>>1) + xpel);
		    bc.x = pt.x + ((jstrwidth (label)>>1) + xpel);
		    bo.y = pt.y - ((defont.height>>1) + ypel);
		    bc.y = pt.y + ((defont.height>>1) + ypel);
	}
	else
	{
		if (fixed [0]=='l')
		    {
			b->flags |= bufL;
			    bo.y = pt.y - (defont.height + (ypel<<1));
			    bc.y = pt.y;
		}
		else if (fixed [0]=='u')
		    {
			b->flags |= bufU;
			    bo.y = pt.y;
			    bc.y = pt.y + (defont.height + (ypel<<1));
		}
		if (fixed [1]=='r')
		    {
			b->flags |= bufr;
			    bo.x = pt.x - (jstrwidth (label) + (xpel<<1));
			    bc.x = pt.x;
		}
		else
		{
			b->flags |= bufl;
			    bo.x = pt.x;
			    bc.x = pt.x + (jstrwidth (label) + (xpel<<1));
		}
	}
	Pcpy (b->rect.origin, bo);
	    Pcpy (b->rect.corner, bc);
	    return b;
}

centeredText (p, s)
Point p;
char *s;
{
	string (&defont, s, &display, Pt (p.x - (jstrwidth (s)>>1),
	    p.y - (defont.height>>1)), F_XOR);
}

short
mouseinbu (bu)
Button *bu [];
{
	short	b	;
	    Button	*bnp	;
	    short	bwm	;	/* button under the cursor	*/
	short	bwom	;	/* button no longer under curs.	*/
	short	changed	;	/* index of button that flipped */
#	define sizeofbu 11	/* why doesn't this work?	*/

	bwm = bwom = changed = -1;
	    for (b=0; b<sizeofbu; b++) {
		bnp = bu [b];
		    if (bnp->flags & bunugae)
		    continue;	/* unreactive button	*/
		if (ptinrect (mouse.xy, bnp->rect))
		    bwm = b;
		    else if ((bnp->flags & bucolor)==bultgray)
		    bwom = b;
	}
	if (bttn1 ()) {
		if (0<=bwm && bwm<sizeofbu) {
			bnp = bu [bwm];
			    if (bnp->flags & buselected) {
				bnp->flags &= ~buselected;
				    shade (bnp, buwhite); 			}
			else {
				bnp->flags |= buselected;
				    shade (bnp, bublack);
			}
			changed = bwm;
			    while (bttn1 ()) ;
		}
		else check ("1 but bad bwm");
	}
	else if (bwm>=0) {
		if ((bu [bwm]->flags & bucolor)==buwhite)
		    shade (bu [bwm], bultgray);
	}
	if (bwom>=0) {
		if ((bu [bwom]->flags & bucolor)==bultgray)
		    shade (bu [bwom], buwhite);
	}
	return changed;
}

shade (bu, bushade)
Button *bu;
short bushade;
{
	switch (bu->flags & bucolor) {
	case buwhite:	
		break;
	    case bultgray:	
		texture16 (&display, bu->rect,
		    &ltgraymap, F_XOR); break;
	    case budkgray:	
		texture16 (&display, bu->rect,
		    &dkgraymap, F_XOR); break;
	    case bublack:	
		rectf (&display, bu->rect, F_XOR); break;
	};
	switch (bushade) {
	case buwhite:	
		break;
	    case bultgray:	
		texture16 (&display, bu->rect,
		    &ltgraymap, F_XOR); break;
	    case budkgray:	
		texture16 (&display, bu->rect,
		    &dkgraymap, F_XOR); break;
	    case bublack:	
		rectf (&display, bu->rect, F_XOR); break;
	};
	bu->flags &= ~bucolor;		/* clear color	*/
	bu->flags |= bushade;		/* set color	*/
}

/* SHOWBU places the image of a button into the screen's bitmap.
 * Formats:
 *	    icon
 *	centered text
 * or
 *	centered text
 * or
 *	    icon
 */
showbu (bu)
Button *bu;
{
	Point	ipt;		/* place of icon, if any */
	Point	tpt;		/* place of text, if any */

	rectf (&display, bu->rect, F_STORE);
	    rectf (&display, inset (bu->rect, 2), F_CLR);
	    if (bu->flags & buselected)
	    bu->flags |= bublack;
	    shade (bu, bu->flags & bucolor);
	    if (bu->icon [0])
	    {
		ipt.x = centerx (bu->rect)
		    - (width (bu->icon [0]->rect)>>1);
		    ipt.y = centery (bu->rect)
		    - (height (bu->icon [0]->rect)>>1);
		    bitblt (bu->icon [0], bu->icon [0]->rect, &display,
		    ipt, F_STORE);
		    tpt.y = centery (bu->rect);
		    tpt.x = centerx (bu->rect);
	}
	else
	{
		tpt.x = centerx (bu->rect);
		    tpt.y = centery (bu->rect);
	}
	if (bu->text) centeredText (tpt, bu->text);
}
