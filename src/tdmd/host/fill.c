/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)fill.c	1.1.1.3	(11/13/87)";

/* includes */
#include <tdmd.h>
#include "jplot.h"

/* This is used for polygon filling (e.g. for bar charts, pie charts, etc.)
/* tplot and the plot(3,4) routines do not support this currently, so
/* you will have to create your data yourself
/* The following routine can be used to help generate your data:
/*
	#include <stdio.h>

	typedef struct Point {
		short	x;
		short	y;
	} Point;

	polyf(t,n,p)
	short t, n;
	Point *p;
	{
		short i;

		putc('F',stdout);
		putsi(t);		/* texture number (0-6)
		putsi(n);		/* number of points 
		for ( i = 0 ; i < n ; i++) {
			putsi(p[i].x);
			putsi(p[i].y);
		}
	}
/*
/* To use the above (call it p.c) to generate data from a given C program,
/* You should say:  cc given.c p.c -lplot; a.out>data
*/

#define MAXPOINTS 64

void
fill(texture1, num, points)
short texture1;
short num;
Point points[];
{
	short i;

	graphic (FILL);

	if (num > MAXPOINTS)
		num = MAXPOINTS;
	graphic ((char)texture1);
	graphic ((char)num);
	for (i = 0; i < num ; i++) {
		xysc (points[i].x, points[i].y);
	}
}
