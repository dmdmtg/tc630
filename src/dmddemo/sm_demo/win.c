/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)win.c	1.1.1.2	(11/4/87)";

#include <dmd.h>
#ifdef DMD630
#include <5620.h>
#else
#define moveto jmoveto
#define lprintf jstring
#undef texture
#define texture texture16

Rectangle
canon(p1, p2)
Point p1, p2;
{
      Rectangle r;

      r.origin.x = min(p1.x, p2.x);
      r.origin.y = min(p1.y, p2.y);
      r.corner.x = max(p1.x, p2.x);
      r.corner.y = max(p1.y, p2.y);
      return(r);
}

outline(r)
Rectangle  r;
{
      register dx=r.corner.x-r.origin.x-1, dy=r.corner.y-r.origin.y-1;
      jmoveto(r.origin);
      jline(Pt(dx, 0), F_XOR);
      jline(Pt(0, dy), F_XOR);
      jline(Pt(-dx,0), F_XOR);
      jline(Pt(0,-dy), F_XOR);
}

#endif


Point dy={0, 17};

main()
{
	register i;
	Point p;

	request(KBD|MOUSE);
	cursinhibit();
Loop:
	jgrey(display.rect);
	mynewlayer(Rect(100, 100, 300, 300));
	p=add(Pt(100,100), Pt(3, 3));
	for(i=0; i<=10; i++){
		moveto(add(p, mul(dy, i)));
		if(ptinrect(p, Drect))
			lprintf("11111111111111111");
	}
	mynewlayer(Rect(225, 225, 425, 425));
	p=add(Pt(225,225), Pt(3, 3));
	for(i=0; i<=10; i++){
		moveto(add(p, mul(dy, i)));
		if(ptinrect(p, Drect))
			lprintf("22222222222222222");
	}
	mynewlayer(Rect(125, 250, 275, 350));
	p=add(Pt(125,250), Pt(3, 3));
	for(i=0; i<=4; i++){
		moveto(add(p, mul(dy, i)));
		if(ptinrect(p, Drect))
			lprintf("3333333333333");
	}
	mynewlayer(Rect(250, 275, 500, 500));
	cursallow();
	for(;kbdchar() == -1;wait(KBD|MOUSE))
#ifdef DMD630
		if((P->state&RESHAPED) && !(P->state&MOVED))
		{
			P->state &= ~RESHAPED;
			goto Loop;
		} else
#endif
		if(button3())
			newwindow();
}
newwindow()
{
	Rectangle r;
	Point p1, p2;

	p1=mouse.xy;
	p2=p1;
	r=canon(p1, p2);
	outline(r);
	for(; bttn3(); nap(2)){
		cursinhibit();
		outline(r);
		p2=mouse.xy;
		r=canon(p1, p2);
		outline(r);
		cursallow();
	}
	cursinhibit();
	outline(r);	/* undraw for the last time */
	mynewlayer(r);
	cursallow();
}

jgrey(r)
Rectangle r;
{
#ifdef DMD630
	static Word greymap[16]={
#else
	static short greymap[16]={
#endif
		0xAAAA,	0x5555,	0xAAAA,	0x5555,	0xAAAA,	0x5555,	0xAAAA,	0x5555,
		0xAAAA,	0x5555,	0xAAAA,	0x5555,	0xAAAA,	0x5555,	0xAAAA,	0x5555,
	};

	texture(&display, r, greymap, F_STORE);
}

mynewlayer(r)
Rectangle r;
{
	cursinhibit();
	rectf(&display, r, F_OR);
	rectf(&display, inset(r, 2), F_CLR);
	cursallow();
}

