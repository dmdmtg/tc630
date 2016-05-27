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
#define MAXRES  5

struct Printerdefs {
	int flowchars;
	int flowticks;
	int width[MAXRES];
	char *initstr[MAXRES];
	char *rowinit[MAXRES];
	char slicesize;
	char passes;
	char fudge;
	char upORdown;
	char swab;
	char *resetstr;
	char *graphicCR;
	char *outform;
};

