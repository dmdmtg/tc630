/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)buttons.c	1.1.1.2 88/02/10 17:13:51";

/* includes */
#include <dmd.h>

/* defines */
#define UP	0		/* false */
#define DOWN	1		/* true */

/* procedures */


buttons(updown)
int updown;
{
	/*
	 * wait for the mouse to be in window, and one or more
	 * buttons to be in the desired state (up or down).
	 */
	do {
		wait(MOUSE);
	} while((button123()!=0) != updown || !ptinrect(mouse.xy, Drect));

	/* now report which button is in that state */
	switch (button123()) {
		case 4:
			return 1;
		case 2:
			return 2;
		case 1:
			return 3;
	}
	return 0;
}
