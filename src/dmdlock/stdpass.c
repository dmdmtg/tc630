/*     Copyright (c) 1991 AT&T */
/*     All Rights Reserved     */

/*     THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T     */
/*     The copyright notice above does NOT evidence any        */
/*     actual or intended publication of such source code.     */
/*     Department 45262 reserves the rights for distribution.  */
/*     Filename: stdpass.c.                                        */

#ident "@(#)stdpass.c   version  25.1.2.3  RDS UNIX source.  Last delta: 7/20/91 20:16:56"

/*
 * stdpass.c - routines for password standards checking
 *
 *	This file is used both for host (ANSI cc) and terminal ($DMD cc)
 *	and thus uses the __STDC__ macro to differentiate between compilers
 *
 * Includes SVR4 password standards.
 *  These are:
 *	o It must contain at least 6 characters.
 *	o It cannot be a circular shift of the login ID.
 *	o It must contain at least two alphabetid characters
 *	  and one numeric or special character.
 *
 *	This code borrows heavily from taken from tlock.c
 *
 *	S.D. Ericson, 7/91
 */

#include "stdpass.h"

/* check password against standards, using login id */
int
#ifdef __STDC__
stdpass(char * login, char * password)	/* ANSI C */
#else
stdpass(login,password)			/* $DMDcc */
char *login, *password;
#endif
{
        int c, alpha, numeric, special, pwok;
	char *p;
#ifdef __STDC__
	int circ(char *, char *);
#else
	int circ();
#endif
	
	/* check password length */
	if ((int) strlen(password) < PWDMIN) 
	        return(PWDSHORT);

	/* check the circular shift of the logonid */
	if (circ(login, password)) 
		return(PWDCIRC);

	/* passwords must contain at least two alpha characters */
	/* and one numeric or special character                 */

	p = password;
	alpha=0, numeric=0, special=0, pwok=0;
	while (c = *p++) {
		if( ( (c >= 'a') && (c <= 'z') ) ||
				( (c >= 'A') && (c <= 'Z') ) )
			alpha++;
		else if( (c >= '0') && (c <= '9') ) numeric++;
		else special++;
	}

	if ( (alpha >= 2) && (numeric || special) )
	{
		return(PWDOK); 
	}
	else 
	{
		return(PWDNOMIX); 
	}
	

}

int
circ( s, t )
char *s, *t;
{
	char c, *p, *o, *r, buff[25], ubuff[25], pubuff[25];
	int i, j, k, l, m;
	 
	m = 2;
	i = strlen(s);
	o = &ubuff[0];
	for( p = s; c = *p++; *o++ = c )
		if( (c >= 'a') && (c <= 'z') ) c += 'A' - 'a';
	*o = '\0';
	o = &pubuff[0];
	for( p = t; c = *p++; *o++ = c )
		if( (c >= 'a') && (c <= 'z') ) c += 'A' - 'a';
	*o = '\0';

	p = &ubuff[0];
	while( m-- ) {
		for( k = 0; k  <=  i; k++) {
			c = *p++;
			o = p;
			l = i;
			r = &buff[0];
			while(--l) *r++ = *o++;
			*r++ = c;
			*r = '\0';
			p = &buff[0];
			if( strcmp( p, pubuff ) == 0 ) return(1);
			}
		p = p + i;
		r = &ubuff[0];;
		j = i;
		while( j-- ) *--p = *r++;
	}
	return(0);
}
