/* */
/*									*/
/*	Copyright (c) 1987,1988,1989,1990,1991,1992   AT&T		*/
/*			All Rights Reserved				*/
/*									*/
/*	  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.		*/
/*	    The copyright notice above does not evidence any		*/
/*	   actual or intended publication of such source code.		*/
/*									*/
/* */
#include "dmdps.h"
#include "defs.h"
#include "printer.h"


#ifndef PSEND
#define PSEND CPU
#endif

extern Texture16 menu2;
extern laststate;

extern char 	maplf, hold, flush;

doUNIXwork()
{
	int	c;

	if ( !hold && own()&RCV ) {
		formp(INULL,&menu2,INULL,"dmdpr: forwarding...");
		request(MOUSE|KBD|RCV|PSEND);
		if (own()&PSEND) {
			while ((c = rcvchar()) != -1)  {
				if ( bttn2() && own()&MOUSE )
					iocntrlmenu();
				if ( !flush ) {
					if ( maplf && c == '\n' )
						shipchar('\r');
					shipchar((char) c); 
				}
			}
			laststate=RES;
			request(MOUSE|KBD|RCV);
			return 1;
		} else {
			formp(INULL,&menu2,INULL,"dmdpr: printer busy");
			request(MOUSE|KBD|RCV);
			return 0;
		}
	}
	if ( hold && own()&RCV ) {
		formp(INULL,&menu2,INULL,"dmdpr: on hold");
		laststate=RES; sleep(14);
		if ( bttn2() && own()&MOUSE )
				iocntrlmenu();
		return 1;
	}
	return 0;
}
