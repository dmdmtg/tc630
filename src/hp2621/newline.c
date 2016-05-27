/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)newline.c	1.1.1.1	(11/4/87)";

#include "hp2621.h"

/* The newline routine is called whenever a newline is encountered in */
/* the input stream or when the line becomes too long. */
/* The struct up is used to determine the cursor position and to */
/* check the flags for page mode and no scroll. */
/* up->y and up->backlines are updated. */

void
newline (up)
register struct User *up;
{
  up->nbacklines--;		/* One less line for getnxtchar */
  if (up->y >= up->y_max) {
    if (up->pagemode) {		/* If at bottom of page and in page */
      up->blocked = TRUE;	/* mode indicate output blocked. */
      return;
    }
    /* Scroll entire page if at bottom and in scroll mode. */
    scroll (up, 1, up->y_max+1, 0, up->y_max);
    up->atend = FALSE;
  }
  else {
    up->y++;		/* Indicate cursor one line further down. */
  }
}
