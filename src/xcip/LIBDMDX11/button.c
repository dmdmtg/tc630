#include "jerq.h"

#define	UP	0
#define	DOWN	1

static short boxcurs_bits[] = {
	0x43FF, 0xE001, 0x7001, 0x3801, 0x1D01, 0x0F01, 0x8701, 0x8F01,
	0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0xFFFF,
};
static Cursor boxcurs;
static int firstime = 1;

Rectangle
getrect (n)
int n;
{
	return getrectb(8>>n, 1);
}

/*
	ngetrect:	ultimate getrect routine
			optional clipping rectangle
			optional blocking for mouse routines
			optional minimum width and height
			returns 1 if minimum rectangle is swept
*/
int
ngetrect (r, clip, but, block, minw, minh)
register Rectangle *r;		/* rectangle				*/
register Rectangle *clip;	/* clipping rectangle, can be 0		*/
register int but;		/* button 1, 2, or 3, defaults to 1	*/
register int block;		/* blocking flag 0 or 1			*/
register int minw;		/* minimum width, can be 0		*/
register int minh;		/* minimum height, can be 0		*/
{
	if(but == 0)
		but = 1;
	*r = getrectb(8>>but, block);
	if(r->origin.x == 0 &&
	   r->origin.y == 0 &&
	   r->corner.x == 0 &&
	   r->corner.y == 0)
		return 0;
	if(r->corner.x - r->origin.x < minw ||
	   r->corner.y - r->origin.y < minh)
		return 0;
	if(clip && !rectclip(r, *clip))
		return 0;
	return 1;
}

Rectangle
getrectb (n, block)
int n, block;
{
	Rectangle r, canon();
	Point p1, p2;
	Cursor *prev;
	
	if (firstime) {
		boxcurs = ToCursor(boxcurs_bits, boxcurs_bits, 8, 8);
		firstime = 0;
	}
	prev = cursswitch(&boxcurs);
	Jscreengrab();
	if(block){
		buttons(UP);
		buttons(DOWN);
	}
	if(!(mouse.buttons&n)){
		r.origin.x=r.origin.y=r.corner.x=r.corner.y=0;
		buttons(UP);
		goto Return;
	}
	p1 = add(mouse.xy, Joffset);
	p2 = p1;
	r = canon(p1, p2);
	outline(r);
	for(; mouse.buttons&n; nap(2)){
		outline(r);
		p2 = add(mouse.xy, Joffset);
		r = canon(p1, p2);
		outline(r);
	}
	outline(r);	/* undraw for the last time */
    Return:
	Jscreenrelease();
	cursswitch(prev);
	r = rsubp(r, Joffset);
	if(!rectclip(&r, Rpt(Drect.org,Drect.cor)))
		r.origin.x = r.origin.y = r.corner.x = r.corner.y = 0;
	return r;
}

buttons (updown)
{
	while((button123()!=0) != updown)
		nap(2);
}

Rectangle
canon (p1, p2)
Point p1, p2;
{
	Rectangle r;
	r.origin.x = min(p1.x, p2.x);
	r.origin.y = min(p1.y, p2.y);
	r.corner.x = max(p1.x, p2.x);
	r.corner.y = max(p1.y, p2.y);
	return(r);
}

static outline(r)
Rectangle  r;
{
	segment(&Jfscreen, r.org, Pt(r.cor.x, r.org.y), F_XOR);
	segment(&Jfscreen, Pt(r.cor.x, r.org.y), r.cor, F_XOR);
	segment(&Jfscreen, r.cor, Pt(r.org.x, r.cor.y), F_XOR);
	segment(&Jfscreen, Pt(r.org.x, r.cor.y), r.org, F_XOR);
}

Cursor *
cursswitch (cp)
Cursor *cp;
{
	Cursor *rc;

	if (cp == (Cursor *)0)
		cp = &normalcursor;
	XDefineCursor(dpy, display.dr, *cp);
	rc = P->cursor;
	P->cursor = cp;
	return rc;
}

cursset (p)
Point p;
{
	XWarpPointer(dpy, display.dr, display.dr, mouse.xy.x, mouse.xy.y,
		display.rect.corner.x, display.rect.corner.y, p.x, p.y);
	mouse.xy.x = p.x;
	mouse.xy.y = p.y;
}
