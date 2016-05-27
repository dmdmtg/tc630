/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)defs.c	1.1.1.1	(11/4/87)";

#include "hp2621.h"

M m = {
  "backup",
  "forward",
  "reset",
  "rev vid",
  "blink",
  "clear",
  "24x80",
  "page",
#ifdef DMD630
  0,
#else
  "new",
#endif
  0
};

char startup [STARTUP];		/* String to send on each startup */
char *firsttime = FALSE;	/* String to be sent first time */
int s_size = 0;			/* Size of startup string */
int f_size = 0;			/* Size of firsttime string */
