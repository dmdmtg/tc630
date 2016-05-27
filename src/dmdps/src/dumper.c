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
#include "printer.h"

#ifndef DMD630
#define Sprintf sprintc
#endif

extern int dump;
char buffer[6];

title(str)
char *str;
{
 	if (dump) 
		shipstr(str);
}

dumpchar(c)
char c;
{
	if (dump) {
		Sprintf(buffer,"%x ",c);
		shipstr(buffer);
	} else
		shipchar(c);
}

dumpnchar(n,s)
int n;
char *s;
{
	int i;

	if (dump)
		for ( i=0 ; i<n ; i++ ) {
			Sprintf(buffer,"%x ",s[i]);
			shipstr(buffer);
	} else
		shipnchar(n,s);
}

dumpstr(s)
char *s;
{
	char c;

	if (dump)
		while (c = *s++)  {
			Sprintf(buffer,"%x ",c);
			shipstr(buffer);
		} 
	else
		shipstr(s);
}
