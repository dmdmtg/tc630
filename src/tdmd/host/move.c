/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)move.c	1.1.1.3	(11/13/87)";

#include <tdmd.h>
#include "jplot.h"

void
move(xi,yi)
{
	if (xi == lastx && yi == lasty)
		return;

	graphic(MOVE);
	xysc(xi, yi);
}
