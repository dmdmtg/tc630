/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)lexit3.c	1.1.1.2 88/02/10 17:13:59";

/* includes */
#include <dmd.h>

/* defines */
#define	UP	0
#define	DOWN	1

/* procedures */


lexit3()	/* return true if button3 is clicked */
{
	extern Texture16 skull; Texture16 *prev; int lexit;
	prev=cursswitch(&skull);
	lexit=buttons(DOWN); buttons(UP);
	cursswitch(prev);
	return(lexit == 3);
}
