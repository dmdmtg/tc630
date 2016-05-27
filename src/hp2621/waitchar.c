/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)waitchar.c	1.1.1.1	(11/4/87)";

#include "hp2621.h"

/* This routine basically waits for input from the user or host.    */
/* The routine first blanks the screen if needed.                   */
/* Next, the routine wait for and processes mouse buttons.          */
/* Any character coming from the host is returned at this point.    */
/* If no mouses and no characters then look to the keyboard.        */
/* Characters are received, translated and stuffed into a buffer.   */
/* When this buffer is full or no more characters are received, the */
/* buffer is sent to the host.  The host will echo these characters */
/* and we will see them later.                                      */

int
waitchar (up)
register struct User *up;
{
  register c, wcond;
  register tick;
  register struct Proc *p = P;	/* Pointer to process table. */

  /* Loop until we get a character from the host */
  for (;;) {
    if (p->state & MOVED) {	/* Has layer been moved? */
      p->state &= ~MOVED;
      p->state &= ~RESHAPED;
    }
    else {			/* Has layer been reshaped? */
      if (p->state & RESHAPED) {
	/* Reset max and min values for x and y in struct up */
        up->x_max = (p->rect.corner.x-p->rect.origin.x-2*XMARGIN)/CW-1;
        up->y_max = (p->rect.corner.y-p->rect.origin.y-2*YMARGIN)/NS-1;
        up->x = 0;
        up->y = 0;
	/* Reshaping a layer will clear the layer.  The layer must */
	/* be returned to reverse video in this case. */
	if (up->rvid) {
	  rectf (p->layer, p->layer->rect, F_XOR);
	}
	backup (up, 0);
	p->state &= ~RESHAPED;
      }
    }
    if (up->clear) {
      clear (up);
    }
    if (up->backp) {		/* We really shouldn't get here */
      return (0);		/* if backp is non- null. */
    }
    if (bttn123 () && (own ()&MOUSE)) {
      if (bttn13 ()) {
        request (RCV|KBD|SEND);		/* Unrequest mouse so */
        sleep (1);			/* layersys handles buttons */
        request (RCV|KBD|SEND|MOUSE);	/* 1 and 3. */
        continue;
      }
      readmenu (up);		/* Process menu for button 2. */
      continue;
    }
    /* If were not blocked then get a character from the host. */
    if (!up->blocked && (c = rcvchar ()) > 0) {
      if (c > defont.n) {
	continue;
      }
      return (c);
    }
    handlekbd (up);	/* Send keyboard characters to the host */
    curse (up);		/* Turn cursor on. */
    up->cursor = TRUE;
    tick = 0;
    wcond = KBD | (up->blocked? 0: RCV);
    do {
      wait (wcond | MOUSE);
      sleep (1);
      if ((++tick >= up->blink) && (up->blink > 0)) {
	curse (up);	/* Blink cursor if set. */
	tick = 0;
	up->cursor = ~up->cursor;
      }
    } while (! ((own () & wcond) || bttn123 ()));
    if (up->cursor) {
      curse (up);		/* Turn cursor off. */
      up->cursor = FALSE;
    }
  }
}
