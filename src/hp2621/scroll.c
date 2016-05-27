/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)scroll.c	1.1.1.1	(11/4/87)";

#include "hp2621.h"

/* This routine scroll the screen.  It moves the lines from sy thru */
/* sy+ly to line dy and then blanks line cy. */
void
scroll (up, sy, ly, dy, cy)
short sy;		/* Source location of move */
short ly;		/* Limit of move */
short dy;		/* Destination of move */
short cy;		/* Which line to be cleared */
register struct User *up;
{
  bitblt (P->layer, Rpt (pt (0, sy), pt (up->x_max+1, ly)),
          P->layer, pt (0, dy), F_STORE);
  rectf (P->layer, Rpt (pt (0, cy), pt (up->x_max+1, cy+1)), F_MODE);
}
