/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)setjwin.c	1.1.1.1     (6/10/87)";

#include <dmd.h>
#include <msgs.h>

void
setjwin(cols, rows)
int cols, rows;
{
        long l[2];

        l[0] = cols;
        l[1] = rows;
        (void) sysctl(P, W_JWIN, l);
}
