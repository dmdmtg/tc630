/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)discture.c	1.1.1.2	(11/11/87)";

#include <dmd.h>
#include <font.h>
#ifdef DMD630
#include <5620.h>
#define texture16 texture
#endif

/*	Fill a disc of radius r centered at x1,y1
 *	The boundary is a sequence of vertically, horizontally,
 *	or diagonally adjacent points that minimize 
 *	abs(x^2+y^2-r^2).
 *
 *	The circle is guaranteed to be symmetric about
 *	the horizontal, vertical, and diagonal axes
 */

discture(b, p, r, t, f)
int r;
Bitmap *b;
Point p;
Texture16 *t;
{
	extern Rectangle saveScreenmap;
	    extern Word *saveBase;
	    int eps,exy,dxsq,dysq;
	    register x0,y0,x1,y1;
	    P->layer->base = addr(P->layer,Drect.origin);
	    P->layer->rect = Drect;
	    P->layer->rect.corner.y-=defont.height;
	    r--;
	    eps = 0;
	    dxsq = 1;
	    dysq = 1 - 2*r;
	    x0 = p.x-1;
	    x1 = p.x+1;
	    y0 = p.y-r-1;
	    y1 = p.y+r;
	    while(y1 > y0) {
		exy = eps + dxsq + dysq;
		    if(-exy <= eps+dxsq) {
			texture16(b, Rect(x0, y0, x1, y0+1), t, f);
			    texture16(b, Rect(x0, y1, x1, y1+1), t, f);
			    y1--;
			    y0++;
			    eps += dysq;
			    dysq += 2;
		}
		if(exy <= -eps) {
			x1++;
			    x0--;
			    eps += dxsq;
			    dxsq += 2;
		}
	}
	P->layer->base = saveBase;
	    P->layer->rect = saveScreenmap;
}
