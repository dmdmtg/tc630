/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)reshaped.c	1.1.1.2 88/02/10 17:14:03";

/* includes */
#include <dmd.h>

/* procedures */


reshaped()
{
	if (P->state & (RESHAPED|MOVED)) {
		P->state &= ~(RESHAPED|MOVED);
		return 1;
	} else
		return 0;
}
