/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)clear.c	1.1.1.1	(11/4/87)";

#include "hp2621.h"

/* Calculate new limits to layer and then clear the layer. */
void
clear (up)
register struct User *up;
{
  register struct Proc *p = P;

  up->clear = FALSE;
  up->x = 0;
  up->y = 0;
  rectf (p->layer, p->rect, F_MODE);	/* Clear layer. */
}
