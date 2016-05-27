/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)disc.c	1.1.1.2	(11/4/87)";

#include <dmd.h>
main(){
	register i;
#ifndef DMD630
	jinit();
	cursinhibit();
#endif
	request (KBD);
	for(i=XMAX/2; i>0; i-=2) {
		if (kbdchar() == 'q')
			break;
		jdisc(Pt(XMAX/2, YMAX/2), i, F_XOR);
		wait(CPU);
	}
	while (kbdchar () != 'q') wait(KBD);
	cursallow ();
}
