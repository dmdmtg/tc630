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
#define MAXWID 5000
#define NEWFMT	0
#define OLDFMT	1

struct bithead {
	int type;	/* 1 run encode raster, 0 run encode raster + "history" */
	int rastwid;	/* in shorts */
	int nrasters;	/* in bits */
	Rectangle r;	/* the rectange coordinanates. */
};

/* one day, this struct may also include the history buffer */
/* history contains parity of previous bits. */
