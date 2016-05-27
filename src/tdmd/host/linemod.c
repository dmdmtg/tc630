/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)linemod.c	1.1.1.3	(11/13/87)";

#include <tdmd.h>
#include "jplot.h"

void
linemod(s)
char *s;
{
	char code = 0;

	graphic(LINEMOD);
	switch(s[0]){
	case 'l':	
		code = 1;			/* long-dashed */
		break;
	case 'd':	
		if(s[3] != 'd')code=3;		/* dots */
		else code=4;			/* dot-dashed */
		break;
	case 's':
		if(s[5] != '\0')code=2;		/* short-dashed */
		else code=0;			/* solid */
	}
	graphic(code);
}
