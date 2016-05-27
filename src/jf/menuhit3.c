/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)menuhit3.c	1.1.1.2 88/02/10 17:14:01";

/* includes */
#include <dmd.h>

/* defines */
#define UP	0
#define DOWN	1
#define TNULL	(Texture16 *)0

/* procedures */


menuhit3(menup)
Menu *menup;
{
	extern Texture16 menu3; Texture16 *prev; int m;
	prev=cursswitch(&menu3);
	if (buttons(DOWN) == 3) { cursswitch(TNULL); m=menuhit(menup,3); }
	else { buttons(UP); m=-1; }
	cursswitch(prev);
	return m;
}
