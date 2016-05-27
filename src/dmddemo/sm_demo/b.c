/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)b.c	1.1.1.2	(11/4/87)";

#include <dmd.h>

#define SIZE	33
main()
{
	register Point Min;
	register Point Max;
	register Rectangle r;
	register Point p, q, s;
	register Point inc;

#ifdef DMD630
	local();
#endif
	request(KBD);
	s.x = s.y = SIZE;
	P->state |= RESHAPED;
	while (kbdchar() != 'q') {
		if (P->state & MOVED) {
			q = sub (Drect.origin, q);
			p = add (q, p);
			Max = add (q, Max);
			Min = add (q, Min);
			P->state &= ~(MOVED|RESHAPED);
			q = Drect.origin;
		}
		else if (P->state & RESHAPED) {
			rectf(&display, Drect, F_XOR);
			Min = p = Drect.origin;
			Max = sub (Drect.corner, s);
			inc.x = 1;
			inc.y = 1;
			P->state &= ~RESHAPED;
			q = Drect.origin;
		}
		rectf(&display, Rpt(p, add (p, s)), F_XOR);
		p.x += inc.x;
		if ((p.x >= Max.x) || (p.x <= Min.x))
			inc.x = -inc.x;
		p.y += inc.y;
		if ((p.y >= Max.y) || (p.y <= Min.y))
			inc.y = -inc.y;
		wait(CPU);
	}
}
