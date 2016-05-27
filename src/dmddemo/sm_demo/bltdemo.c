/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)bltdemo.c	1.1.1.2	(11/4/87)";

#include <dmd.h>
Rectangle myrect={400, 400, 501, 501};
main()
{
	register x, y;
	register long i;

#ifndef DMD630
	jinit();
	WonB();
	cursinhibit ();
#endif
	for(y=0; y<YMAX; y+=20){
		jmoveto(Pt(0, y));
		jlineto(Pt(XMAX, y), F_STORE);
	}
	for(x=0; x<XMAX; x+=20){
		jmoveto(Pt(x, 0));
		jlineto(Pt(x, YMAX), F_STORE);
	}
	for(x=0; x<300; x++){
		bitblt(&display, rtransform(myrect), &display, transform(
		   sub(myrect.origin, Pt(1, 1))), F_STORE);
		myrect=rsubp(myrect, Pt(1, 1));
	}
	sleep(120);
	eor();
	cursallow ();
	exit();
}
eor()
{
	rectf(&display, Drect, F_OR);
}
