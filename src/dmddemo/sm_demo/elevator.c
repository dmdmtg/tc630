/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)elevator.c	1.1.1.2	(11/4/87)";

#include <dmd.h>
main()
{
	register x, y, dx, dy;
	register long i;
#ifndef DMD630
	jinit();
	WonB();
#endif
	dx=20;
	dy=20;
	for(y=0; y<YMAX; y+=dy){
		jmoveto(Pt(0, y));
		jlineto(Pt(XMAX, y), F_STORE);
	}
	for(x=0; x<XMAX; x+=dx){
		jmoveto(Pt(x, 0));
		jlineto(Pt(x, YMAX), F_STORE);
	}
	for(x=0; x<99; x++){
		bitblt(&display, rtransform(Rect(99, 99, 300, 300)),
			 &display, transform(Pt(100,100)), F_STORE);
		bitblt(&display, rtransform(Rect(99, 399, 300, 600)),
			 &display, transform(Pt(100,400)), F_STORE);
		bitblt(&display, rtransform(Rect(99, 699, 300, 900)),
			 &display, transform(Pt(100,700)), F_STORE);
		bitblt(&display, rtransform(Rect(499, 99, 700, 300)),
			 &display, transform(Pt(500,100)), F_STORE);
		bitblt(&display, rtransform(Rect(499, 399, 700, 600)),
			 &display, transform(Pt(500,400)), F_STORE);
		bitblt(&display, rtransform(Rect(499, 699, 700, 900)),
			 &display, transform(Pt(500,700)), F_STORE);
	}
	sleep (120);
	eor();
	cursallow ();
	exit();
}
eor(){
	rectf(&display, Drect, F_OR);
}
