/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)newhp.c	1.1.1.1	(11/4/87)";

#include "hp2621.h"
/* newhp gets called by the new window routine.  The FALSE passed to */
/* realmain indicates that this is not the first layer. */

void
newhp ()
{
  realmain (FALSE);
}
