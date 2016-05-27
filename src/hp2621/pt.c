/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)pt.c	1.1.1.1	(11/4/87)";

#include "hp2621.h"

/* Return the pixel location of a character location on the screen. */
Point
pt (x, y)
register short x, y;
{
  return (add (P->rect.origin, Pt (x*CW+XMARGIN, y*NS+YMARGIN)));
}
