/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)handlekbd.c	1.1.1.2	(11/4/87)";

#include "hp2621.h"

void
handlekbd (up)
register struct User *up;
{
  register c, cc;
  int lastchar;
  register struct Proc *p = P;	/* Pointer to process table. */
  char echobuf[EBUFSIZE];
  char *eb, *ep;

  /* Store EBUFSIZE characters in the echo buffer. */
  ep = echobuf;
  do {
    while (ep < &echobuf[EBUFSIZE-(PFKEYSIZE+1)] &&	/* Note 1 */
	   (c = kbdchar ()) > 0 &&
	   c != ESC) {
      cc = c;
      c = 0;
      switch (cc) {
	case '\023': {	/* ctrl S */
	  /* Indicate blocked and ignore char */
	  if (!up->blocked) {
	    up->blocked = TRUE;
	    continue;
	  }
	}	/* Continue with next case statement */
	case '\021': {	/* ctrl Q */
	  if (up->blocked) {
	    up->blocked = FALSE;
	    if (up->pagemode) {
	      up->clear = TRUE;
	    }
	    continue;		/* Ignore ctrl-q if blocked */
	  }
	  *ep++ = cc;
	  break;
	}
	default: {	/* Process PF1 - PF8 */
#ifdef DMD630
	  if (cc >= 0x80 && cc <= 0x87) {
#else
	  if (cc >= 0x82 && cc <= 0x89) {
#endif
	    int i, j;
#ifdef DMD630
	    j = cc - 0x80;	/* Convert 0x80 - 0x87 to 0 - 7 */
#else
	    j = cc - 0x82;	/* Convert 0x80 - 0x87 to 0 - 7 */
#endif
	    i = 0;
#ifdef DMD630
	    ep += pfkey(j, ep, PFKEYSIZE);
#else
          while ((cc = BRAM->pfkeys[j][i].byte) && 
                 (++i <= PFKEYSIZE)) {
            *ep++ = cc;       /* Copy BRAM to buffer */
	  }
#endif
	  }
	  else {
	    *ep++ = cc;
	  }
	  break;
	}
      }
      /* Layer needs to be cleared if page is full and were blocked */
      if (up->pagemode && up->blocked) {
	up->clear = TRUE;
      }
      up->blocked = FALSE;
    }
    *ep = 0;			/* Don't scan beyond this point */
    lastchar = (c>0)?c:0;	/* Save unused character */

    /* Parse the characters just received for keyboard sequences */
    /* which perform cursor motion and clearing of screen. */

    eb = echobuf;
    if (echobuf[0] == ESC) {
      if (echobuf[1] == '[') {
	switch (echobuf[2]) {	/* Look for 2,7,A,B,C,D or H */
	  case '2': {
	    if (echobuf[3] == 'J') {	/* Clear */
	      clear (up);
	      eb += 4;
	    }
	    break;
	  }
	  case '7': {
	    if (strcmp (echobuf+3, HOMEDOWN) == 0) {	/* Home Down */
	      up->x = 0;
	      up->y = up->y_max;
	      eb += 7;
	    }
	    break;
	  }
	  case 'A': {			/* Cursor Up */
	    if (up->y > 0) {
	      --up->y;
	    }
	    eb += 3;
	    break;
	  }
	  case 'B': {			/* Cursor Down */
	    if (up->y < up->y_max) {
	      ++up->y;
	    }
	    eb += 3;
	    break;
	  }
	  case 'C': {			/* Cursor Right */
	    if (up->x < up->x_max) {
	      ++up->x;
	    }
	    eb += 3;
	    break;
	  }
	  case 'D': {			/* Cursor Left */
	    if (up->x > 0) {
	      --up->x;
	    }
	    eb += 3;
	    break;
	  }
	  case 'H': {			/* Home Cursor */
	    up->x = 0;
	    up->y = 0;
	    eb += 3;
	    break;
	  }
	}
      }
    }
    /* Send the characters accumulated so far and send them to the */
    /* host. */
    if (ep > eb) {
      sendnchars ((int) (ep-eb), eb);
    }
    if (lastchar) {
      ep = echobuf;
      *ep++ = lastchar;
    }
  } while (lastchar);
}

/* Note 1 */
/* PFKEYSIZE-1 is used to make room for a NULL sentinal */
/* and any function keys. */
