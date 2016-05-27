/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)readmenu.c	1.1.1.2	(11/4/87)";

#include "hp2621.h"

#ifdef DMD630
#define newwindow	(*Sys[498])
#else
#define reshape
#define newwindow
#include <pandora.h>	/* Get reshape and newwindow from pandora. */
#endif

/* Process the menu selections for button 2 */

void
readmenu (up)
register struct User *up;
{
  register struct Proc *p = P;
  Rectangle r;
  register struct Layer *l;

  switch (menuhit (&up->menu, 2)) {
    case 0: {		/* back up */
      if (!up->atend) {
	up->backc++;		/* Calculate # pages to go back by */
	backup (up, up->backc);
      }
      else {
	/* Flash layer if at end of history */
	rectf (p->layer, p->rect, F_XOR);
	rectf (p->layer, p->rect, F_XOR);
      }
      return;
    }
    case 1: {		/* move forward */
      if (up->backc > 0) {
	up->backc--;		/* Calculate # pages to go forward by */
	backup (up, up->backc);
      }
      else {
	/* Flash layer if at start of history */
	rectf (p->layer, p->rect, F_XOR);
	rectf (p->layer, p->rect, F_XOR);
      }
      return;
    }
    case 2: {		/* reset */
      up->backc = 0;
      backup (up, 0);	/* Display latest (last) page */
      return;
    }
    case 3: {		/* reverse video */
      up->rvid = ~up->rvid;
      rectf (p->layer, p->layer->rect, F_XOR);
      return;
    }
    case 4: {		/* Toggle blink */
      if (up->blink) {
	up->blink = 0;
      }
      else {
	up->blink = BLINK;
      }
      return;
    }
    case 5: {		/* clear screen */
      up->clear = TRUE;	/* Getchar will clear the screen */
      return;
    }
    case 6: {		/* 24x80 */
      /* Nail layer to screen at the upper left hand corner of */
      /* the layer. */
      r.origin = p->layer->rect.origin;
      /* Calculate bottom right hand corner of 24x80 character layer */
      r.corner = add (r.origin, Pt (80*CW+2*XMARGIN+2*INSET,
		                    24*NS+2*YMARGIN+2*INSET));
      /* If x corner is off screen then use XMAX-BORDER as right */
      /* edge of  layer and adjust the left edge. */
      if (r.corner.x > (XMAX-BORDER)) {
	r.origin.x -= r.corner.x - (XMAX-BORDER);
	r.corner.x = (XMAX-BORDER);
      }
      /* If y corner is off screen then use YMAX-BORDER as bottom */
      /* edge of  layer and adjust the top edge. */
      if (r.corner.y > (YMAX-BORDER)) {
	r.origin.y -= r.corner.y - (YMAX-BORDER);
	r.corner.y = (YMAX-BORDER);
      }
#ifdef DMD630
      reshape (r);
#else
      reshape(p->layer, inset (r, 2));
#endif
      return;
    }
    case 7: {		/* Toggle page/scroll flag */
      up->pagemode = ~up->pagemode;
      up->menu.item[7] = up->pagemode? "scroll": "page";
      return;
    }
    case 8: {		/* new hp window */
      /*
       * This dissociates the code space from all processes 
       * so as to avoid having it freed when the 1st layer is deleted
       */
      if (up->orig) {
	 ((char **)p->text)[-1] = 0;
      }
      /* Allow user to sweep out new window */
#ifdef DMD630
      newwindow (newhp, Rect(0,0,0,0), P->host, 2048L, 1);
#else
      newwindow (newhp);
#endif
      return;
    }
  }
}
