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
#include "pfd.h"

shipstr(s)
char *s;
{
	for(;*s;s++) shipchar(*s);
}

shipnchar(n,s)
int n;
char s[];
{
	int i;
	for(i=0;i<n;i++) 
		shipchar(s[i]);
}

extern flush;
extern ioctrlmenu();

shipchar(c)
char c;
{
	flush=0;
	if (psendchar(c) == 0) {
		formp((Texture16 *)NULL,(Texture16 *)NULL,(Texture16 *)NULL,"printer busy");
		while (psendchar(c) == 0) {
			iocntrlmenu();
			if (flush) 
				return;
		}
		formp((Texture16 *)NULL,(Texture16 *)NULL,(Texture16 *)NULL,"");
	}
}
