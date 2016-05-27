/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)delay.c	1.1.1.3	(11/13/87)";

/* includes */
#include <tdmd.h>
#include "jplot.h"

/* procedures */
extern void Flush();


/* This is used to cause a 1-second pause on output.
/* tplot and the plot(3,4) routines do not support this currently, so
/* you will have to create your data yourself (which is just a 'd')
/* The following routine can be used to help generate your data:
/*
	#include <stdio.h>
	delay(){
		putc('d',stdout);
	}
/*
/* To use the above (call it d.c) to generate data from a given C program,
/* You should say:  cc given.c d.c -lplot; a.out>data
*/
void
delay()
{
	graphic(DELAY);
	Flush();
}
