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
#ifdef PAR
#define shipchar(c)	thinkchar(c)
#define shipnchar(n,c)	thinknchar(n,c)
#define shipstr(s)	thinknchar(strlen(s),s)
#define Psendchar(c)	thinkchar(c)
#define Psends(s)	thinknchar(strlen(s),s)
#else
#define	thinkstart()
#define	thinkstop()
#define	thinkflush()
#define	thinkchar(c)	shipchar(c)
#define	thinknchar(n,s)	shipnchar(n,s)
#endif

