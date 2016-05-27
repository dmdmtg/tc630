/*     Copyright (c) 1991 AT&T */
/*     All Rights Reserved     */

/*     THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T     */
/*     The copyright notice above does NOT evidence any        */
/*     actual or intended publication of such source code.     */
/*     Department 45262 reserves the rights for distribution.  */
/*     Filename: stdpass.h.                                        */

#ident "@(#)stdpass.h   version  25.1.2.3  RDS UNIX source.  Last delta: 7/20/91 20:16:59"

/*
 * stdpass.h - header file for password standards checking
 *
 *	This file is used both for host (ANSI cc) and terminal ($DMD cc)
 *	and thus uses the __STDC__ macro to differentiate between compilers
 *
 *	S.D. Ericson, 7/91
 */

/* returns from stdpass(): */

#define	PWDOK		0	/* Password meets standards */
#define	PWDSHORT	1	/* Password too short */
#define PWDCIRC		2	/* Password is circular shift of login */
#define PWDNOMIX	3	/* Password does not have at least
				   2 alphanumerics and 1 special char 	*/

/* standard definition */

#define PWDMIN		6	/* minimum number of chars in a password */

/* checks the 2nd arg for the standards:
 *  first arg is login id, second is password
 */
#ifdef __STDC__
int stdpass(char *, char *);
#else
int stdpass();
#endif
