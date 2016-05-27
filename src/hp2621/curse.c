/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)curse.c	1.1.1.1	(11/4/87)";

#include "hp2621.h"

/* Toggle the little black rectangle on and off */
void
curse (up)
register struct User *up;
{
  rectf(P->layer, Rpt(pt(up->x, up->y), add(pt(up->x, up->y), Pt(CW, NS))), F_XOR);
}
