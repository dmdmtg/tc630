/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)line.c	1.1.1.3	(11/13/87)";

#include <tdmd.h>
#include "jplot.h"

void
line(x0,y0,x1,y1)
{
	if (x1 == lastx && y1 == lasty) {
		move(x1, y1);
		cont(x0, y0);
		return;
	}

	move(x0, y0);
	cont(x1, y1);
}

void
cont(x, y)
{
	graphic(CONT);
	xysc(x, y);
}
