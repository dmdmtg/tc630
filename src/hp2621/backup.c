/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)backup.c	1.1.1.1	(11/4/87)";

#include "hp2621.h"

/* This routine is used to show, on the display, the 'count' most */
/* page stored in memory.  A page is defined as 3/4th the maximum */
/* number of lines  in a layer. */ 
/* Backup is performed in the following manner:  This routine        */
/* determines the location in memory of one page backward.  It sets  */
/* a pointer (up->backp) to this location and sets a variable        */
/* (up->nbacklines) to the number of lines to be redrawn (always     */
/* up->y_max-2).  The getnxtchar */
/* routine does the actual redrawing of   */
/* screen.  When it sees that backp is non-null, it starts returning */
/* characters pointed to by backp to realmain.                       */

/* External influences: */
/*	up->atend:	Set to TRUE if routine can't backup anymore. */
/*	up->backp:	Set to location of next character to be */
/*			output by getnxtchar. */
/*	up->nbacklines:	Number of lines to be displayed when */
/*			getnxtchar redraws the screen.  */
/*			Decremented in newline 	routine. */

void
backup (up, count)
register struct User *up;	/* Pointer to User structure. */
short count;			/* Back up to this page. */
{
  register n;
  register char *cp;

  up->clear = TRUE;		/* Clear layer when in waitchar */

  /* Get the number of lines back in memory where displaying is to */
  /* begin. */
  n = 3* (count+1)*up->y_max/4;	/* count+1 because first page */
				/* actually starts 1 page back. */
  cp = up->histp;		/* Start from last line stored. */
  up->atend = FALSE;		/* Indicate not at end of hist buffer.*/

  /* Now count back n lines. */
  while (n >= 0) {		
    cp--;
    if (cp < up->hist) {	/* Hist buffer is circular, so wrap */
      cp = &up->hist[HISTSIZ-1];/* to end of buffer if start reached. */
    }
    if (*cp == '\0') {		/* NULL indicates end of stored lines */
      up->atend = TRUE;
      break;
    }
    if (*cp == '\n') {
      n--;
    }
  }
  cp++;
  if (cp >= &up->hist[HISTSIZ]) {	/* Wrap to start if at end. */
    cp = up->hist;
  }
  up->backp = cp;		/* Getchar will start output here. */
  up->nbacklines = up->y_max-2;	/* Display only y_max-2 lines when */
				/* the layer is redrawn by */
				/* getnxtchar. */
}
