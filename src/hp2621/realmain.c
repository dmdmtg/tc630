/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)realmain.c	1.1.1.2	(11/4/87)";

#include "hp2621.h"

/* This is the routine which acts as the real main of the program. */
/* It gets called from main and newhp.  Newhp is called whenever a */
/* new hp window is created. */
/* Input argument:					*/
/*	orig	TRUE  - This is the original layer.	*/
/*		FALSE - This is not the original layer.	*/

void
realmain (orig)
BOOLEAN orig;
{
  char     buf[BUFS+1];
  register struct Proc *p = P;	/* Pointer to process table. */
  register struct User *up;	/* Pointer to user table. */
  register short n;		/* Temp */
  register c;			/* Return value from getnxtchar */
  register BOOLEAN standout = FALSE;	/* Standout mode */
  register BOOLEAN insmode = FALSE;	/* Character insert mode */
  BOOLEAN relative;		/* Indicates relative cursor moves */

#ifndef DMD630
  if (!orig) {
    p->data = (char *)alloc (sizeof (struct udata));
  }
#endif
  p->state |= USER;	/* make layersys update mouse */
  /* Each layer gets a user table */
  if ((up = (struct User *)alloc (sizeof (struct User)))
      == (struct User *)NULL) {
    string (&defont, OUTOFSPACE, p->layer, pt(0,0), F_XOR);
    string (&defont, "",         p->layer, pt(1,0), F_XOR);
    sleep (ERRORWT);
    exit ();
  }
  up->histp = up->hist;			/* Start history list */
  up->m = m;				/* Menu */
  up->menu.item = up->m.menutext;	/* Point to 1st menu option */
  up->orig = orig;
  request (RCV|KBD|SEND|MOUSE);
  if (orig && f_size) {	/* if first time, send first time string */
    sendnchars(f_size, firsttime);
    sendnchars(1, "\n");
  }
  if (s_size) {		/* send startup string, if any */
    sendnchars(s_size, startup);
  }
  p->state |= RESHAPED;	/* Set x_max and y_max in struct up */
  for (;;) {
    if (up->x > up->x_max) {	/* Wrap at end of line */
      up->x = 0;			/* Start of line */
      newline (up);			/* Next line down */
    }
    /* Check first character for special properties */
    buf[0] = getnxtchar (up, 0);
    buf[1] = '\0';
    switch (buf[0]) {
      case '\007': {		/* bell */
	ringbell ();
	break;
      }
      case '\t': {		/* tab modulo 8 */
	up->x = (up->x|7)+1;
	break;
      }
      case '\033': {
	switch (getnxtchar (up, 0)) {
	  case '&': {
	    switch (getnxtchar (up, 0)) {
	      case 'a': {	/* position cursor &c */
		for (;;) {
		  /* Get column of row designation in n and trailing */
		  /* character in buf. */
		  n = number (up, buf, &relative);
		  switch (buf[0]) {
		    case 'R':
		    case 'Y':
		    case 'r':
		    case 'y': {
		      if (relative) {
			up->y += n;
		      }
		      else {
			up->y = n;
		      }
		      up->y = 
			  (up->y<0)?0:(up->y>up->y_max)?up->y_max:up->y;
		      if (buf[0] < 'a') {
			/* Terminate sequence on capital letters. */
			break;
		      }
		      continue;
		    }
		    case 'C':
		    case 'c': {
		      if (relative) {
			up->x += n;
		      }
		      else {
			up->x = n;
		      }
		      up->x = 
			  (up->x<0)?0:(up->x>up->x_max)?up->x_max:up->x;
		      if (buf[0] < 'a') {
			/* Terminate sequence on capital letters. */
			break;
		      }
		      continue;
		    }
		  }
		  break;
		}
		break;
	      }
	      /* Start/stop standout.  \033&d followed by A-O */
	      /* turns standout on and @ turns it off. */
	      case 'd': {
		if ((n = getnxtchar (up, 0))>='A' && n <= 'O') {
		  standout = TRUE;
		}
		else if (n == '@') {
		  standout = FALSE;
		}
		break;
	      }
	      case 'j': {
		getnxtchar (up, 0);	/* Ignore this character seq */
		break;			/* Should be '@', 'A', or 'B' */
	      }
	      case 'p': {	/* Ignore ESC & p <number> character */
		n = number (up, buf, &relative);
		break;
	      }
	      /* Ignore \033& followed by anything but the above. */
	      default: {
		getnxtchar (up, 0);
		break;
	      }
	    }
	    break;
	  }
	  case '0':		/* *Unimplemented* Print All */
	  case '1':		/* *Unimplemented* Set Tab */
	  case '2':		/* *Unimplemented* Clear Tab */
	  case '3':		/* *Unimplemented* Clear All Tabs */
	  case '4':		/* *Unimplemented* Set Left Margin */
	  case '5': {		/* *Unimplemented* Set Right Margin */
	    break;
	  }
	  case '@': {
	    sleep (60);		/* Delay 1 second */
	    break;
	  }
	  case 'A': {	/* Upline */
	    if (up->y>0) {
	      up->y--;
	    }
	    break;
	  }
	  case 'B': {	/* Downline */
	    if (up->y < up->y_max) {
	      standout = FALSE;
	      up->y++;
	    }
	    break;
	  }
	  case 'C': {	/* Right */
	    if (up->x < up->x_max) {
	      up->x++;
	    }
	    break;
	  }
	  case 'D': {	/* Left */
	    if (up->x > 0) {
	      up->x--;
	    }
	    break;
	  }
	  case 'E': {	/* Reset Terminal */
	    break;
	  }
	  case 'F': {	/* Home Down */
	    up->x = 0;
	    up->y = up->y_max;
	    break;
	  }
	  case 'G': {	/* Cursor Return */
	    up->x = 0;
	    break;
	  }
	  case 'H':	/* home cursor */
	  case 'h': {
	    up->x = 0;
	    up->y = 0;
	    break;
	  }
	  case 'I': {	/* Tab */
	    up->x = (up->x|7)+1;
	    break;
	  }
	  case 'J': {	/* clear to end of display */
	    rectf (p->layer, Rpt (pt (0, up->y+1),
		  pt (up->x_max+1, up->y_max+1)), F_MODE);
	    /* Continue onto next case statement */
	  }
	  case 'K': {	/* clear to EOL */
	    rectf (p->layer, Rpt (pt (up->x, up->y),
		  pt (up->x_max+1, up->y+1)), F_MODE);
	    break;
	  }
	  case 'L': {	/* insert blank line */
	    scroll (up, up->y, up->y_max, up->y+1, up->y);
	    break;
	  }
	  case 'M': {	/* delete line */
	    scroll (up, up->y+1, up->y_max+1, up->y, up->y_max);
	    break;
	  }
	  case 'P': {	/* delete char */
	    /* Note: Doesn't work for wrap around */
	    bitblt (p->layer, Rpt (pt (up->x+1, up->y),
		     pt (up->x_max+1, up->y+1)),
	       p->layer, pt (up->x, up->y), F_STORE);
	    rectf (p->layer, Rpt (pt (up->x_max, up->y),
		  pt (up->x_max+1, up->y+1)), F_MODE);
	    break;
	  }
	  case 'Q': {	/* enter insert mode */
	    insmode = TRUE;
	    break;
	  }
	  case 'R': {	/* leave insert mode */
	    insmode = FALSE;
	    break;
	  }
	  case 'S': {	/* Scroll up */
	    scroll (up, 1, up->y_max+1, 0, up->y_max);
	    break;
	  }
	  case 'T': {	/* Scroll down */
	    scroll (up, 0, up->y_max, 1, 0);
	    break;
	  }
	  case 'Y':	/* Display function on */
	  case 'Z': 	/* Display function off */
	  case '\\': 	/* Primary Status Response */
	  case '^': 	/* Term Primary Status */
	  case '`': 	/* Relative Cursor Sense */
	  case 'a':	/* Absolute CUrsor Sense */
	  case 'f': {	/* Modem Disconnect */
	    break;	/* Ignore these escape sequences */
	  }
	  case 'i': {	/* back tab */
	    if (up->x>0) {
	      up->x = (up->x-1) & ~07;
	    }
	    break;
	  }
	  case 'z': {	/* Terminal Self Test */
	    break;	/* Ignore */
	  }
	}
	break;
      }			/* End of case \033 */
      case '\b': {	/* Backspace */
	if (up->x > 0) {
	  --up->x;
	}
	break;
      }
      case '\n': {	/* Linefeed */
	newline (up);
	standout = FALSE;
	break;
      }
      case '\r': {	/* Carriage return */
	up->x = 0;
	standout = FALSE;
	break;
      }
      default: {		/* Ordinary char */

	/* Default characters are handled as follows: */
	/* Upto 32 characters are input and stored in buf. */
	/* Input continues until buf is full, the length of the line */
	/* is exceeded or a nondefault character is input.  When one */
	/* of these conditions is met the last input character is */
	/* stored in peekc and the buffer is output. */
	/* Characters are inserted or highlighted. */
	/* Getchar will return peekc next loop thru. */
	/* The outer for loop will handle a line longer than */
	/* x_max by doing a newline. */

	n = 1;		/* Number of input characters */
	c = 0;
	while (up->x+n<=up->x_max && n<BUFS &&
	       (c = getnxtchar (up, 1))>=' ' && c<'\177') {
	  buf[n++] = c;
	  c = 0;
	}
	buf[n] = 0;
	if (insmode) {	/* When in insert mode: */
	  bitblt (p->layer, Rpt (pt (up->x, up->y),
				 pt (up->x_max-n+1, up->y+1)),
		  p->layer, pt (up->x+n, up->y), F_STORE);
	}
	/* Clear area to receive n characters */
	rectf (p->layer, Rpt (pt (up->x, up->y), 
			  pt (up->x+n, up->y+1)), F_MODE);
	/* Place characters on screen */
	string (&defont, buf, p->layer, pt (up->x, up->y), F_XOR);
	if (standout) {	/*Rev vid characters that just went out */
	  rectf (p->layer, Rpt (pt (up->x,up->y),
				pt (up->x+n,up->y+1)), F_XOR);
	}
	up->x += n;
	up->peekc = c;		/* Save control character for next */
	break;			/* time thru loop. */
      }
    }
  }
}
