/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)getnxtchar.c	1.1.1.1	(11/4/87)";

#include "hp2621.h"

/* This routine gets one character to be displayed on the screen.   */
/* Simple enough, but where it gets these characters from is a      */
/* different story.  getnxtchar first looks at up->peekc.  If this  */
/* value is non-null then it is returned (see declaration of peekc).*/
/* Next, getnxtchar looks to see if backup is active.  It determines*/
/* this by looking for a non-null up->backp.  If backup is active   */
/* then the character pointed to by backp is examined.  It is       */
/* returned if it is non-zero.                                      */
/* Finally, getnxtchar will resort to input from the host.  This    */
/* character is stored in memory (up->hist) and returned to the     */
/* caller.                                                          */
/*                                                                  */
/* Passed parameters:                                               */
/*         up:      pointer to user structure.                      */
/*         w:       special flag used to speed through put of       */
/*                  character when dealing with text.               */

int
getnxtchar (up, w)
register struct User *up;
BOOLEAN w;
{
  register c = up->peekc;		/* Get any saved character. */

  up->peekc = 0;
  if (c > 0) {
    return (c);			/* Return last saved character */
  }
  while (c <= 0) {
    if (up->backp) {		/* If backup actively being processed */
      c = *up->backp;		/* then use this character. */
      /* Return this character only if more lines needed and not at */
      /* end of history list (signaled by c = NULL). */
      if (c && up->nbacklines >= 0) {
        up->backp++;
        if (up->backp >= &up->hist[HISTSIZ]) {
          up->backp = up->hist;	/* Wrap pointer to start hist list. */
	}
        return (c);
      }
      up->backp = 0;		/* Indicate backup finished */
    }
    /* W is used to speed through put of 32 blocks of text. */
    /* If w is true return input characters without checking mouse */
    /* buttons and keyboard. */
    /* If we are blocked, then we will need to look at input from */
    /* the keyboard. */
    if (w && !up->blocked) {	/* faster if don't want to block */
      c = rcvchar ();
      if (c>defont.n) {
	c = 0;
      }
      else if (c <= 0) {
        return (c);
      }
    }
    else {
      c = waitchar (up);	/* Get character from host */
    }
  }
  /* Update history list, remembering circularity of list. */
  *up->histp++ = c;
  if (up->histp >= &up->hist[HISTSIZ]) {
    up->histp = up->hist;
  }
  *up->histp = '\0';		/* Indicate end of history list */
  return (c);
}
