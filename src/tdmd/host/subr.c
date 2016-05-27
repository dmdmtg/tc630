/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)subr.c	1.1.1.3	(11/13/87)";

/* includes */
#include <stdio.h>
#include <tdmd.h>
#include "jplot.h"

/* externals */
extern short dumpMode;
extern short debugMode;
extern short termSimulation;
extern FILE *dbfp;

/* locals */
static int
	opened = 0		/* 0 if in alphanumeric mode, 1 if graphics */
;

static char
	buf[PACKET],		/* storage for a packet of data */
	*p = &buf[0]		/* free position in buf */
;

static void
	flush(),		/* output what we have */
	mywrite()		/* write for hex mode */
;

/* procedures */


void
xysc(xi, yi)
{
	int termx;
	int termy;

#	ifdef DEBUG
	    if (debugMode) {
		(void)fprintf (dbfp,
		    "\nxi=%6d, porigx=%10f, scalex=%10f, torigx=%10f",
		    xi, porigx, scalex, torigx);
		(void)fprintf (dbfp,
		    "\nyi=%6d, porigy=%10f, scaley=%10f, torigy=%10f",
		    yi, porigy, scaley, torigy);
	    }
#	endif /* DEBUG */

	/*
	 * The following equation, which is basically:
	 *
	 *	term = (i - porig) * scale + torig;
	 *
	 * is a combination of the two formulas:
	 *
	 *  Pnorm = (Pdata - Odata)/((Cdata-Odata)/(Cnorm-Onorm))
	 *  Pdisp = Pnorm * (Cdisp-Odisp)/(Cnorm-Onorm) + Odisp
	 *
	 * to normalize the data, and then to convert it for use by the 630.
	 */
	termx = (xi - porigx) * scalex + torigx;
	termy = (yi - porigy) * scaley + torigy;

#	ifdef DEBUG
	    if (debugMode)
		(void)fprintf (dbfp,
			"\n	termx=%d, termy=%d", termx, termy);
#	endif /* DEBUG */

	/*
	 * pass low order 6 bits of termx, with 0xC0, to the terminal.
	 *
	graphic((termx&077) | 0300);
	 *
	 * pass high order 4 bits (of 10 bits total) of termx and
	 * high order 3 bits (of 10 bits total) of termy, with 0x80,
	 * to the terminal.
	 *
	graphic(((termx >> 6) & 017) | ((termy >> 3) & 0160) | 0200);
	 *
	 * pass low order 7 bits of termy, with 0x80, to the terminal.
	 *
	graphic(termy&0177 | 0200);
	 */

	/*
	 * send x and y coordinates to the terminal, low order byte
	 * (of shorts) first.
	 */

	graphic(termx&0xff);
	graphic((termx>>8)&0xff);

	graphic(termy&0xff);
	graphic((termy>>8)&0xff);

	/*
	 * Make new position the current position.
	 * "last" really means "current".
	 */
	lastx = xi;
	lasty = yi;
}

void
start()
{
	if (mpx == 0) {
		graphic(ON);
		flush();
		++wantready;
	}
}

void
graphic(c)
char c;
{

	if (!opened) {
		++opened;
		graphic(OPEN);
	}

	*p++ = c;
	if (p == &buf[PACKET])
		flush();
}

void
alpha(c)
char c;
{

	if (opened) {
		graphic(CLOSE);
		opened = 0;
	}

	*p++ = c;
	if (p == &buf[PACKET])
		flush();
}

void
Flush()
{
	if (wantready)
		graphic(ON);
	flush();
}

static void
flush()
{
	char c;

	if (wantready)
		do {
			(void)read (fromjerq, &c, 1);
		} while (c != READY);

	if (!hex_mode) {
		(void)write (tojerq, &buf[0], (unsigned) (p - &buf[0]));

#		ifdef DEBUG
		    if (dumpMode) {
			short column;
			char *hexPtr;

			(void)fprintf (dbfp, "\n%d bytes\n", p - buf);
			for (hexPtr = buf; hexPtr < p; ) {
			    for (column = 0; column < 4; column++) {
				if (hexPtr >= p)
					break;
				(void)fprintf (dbfp, "%.2x %.2x %.2x %.2x  ",
				    *hexPtr++, *hexPtr++,
				    *hexPtr++, *hexPtr++);
				putc (' ', dbfp);
			    }

				putc ('\n', dbfp);
			}
		    }
#		endif /* DEBUG */
	}
	else
		mywrite(buf, p - &buf[0]);

	p = &buf[0];
}

void
finish()
{
	if (mpx == 0) {
		graphic(OFF);
		flush();
		wantready = 0;
	}

	/* set quitNow flag */
	if (!termSimulation)
		graphic(EXIT);

	/* return to main loop */
	graphic(CLOSE);
	flush();
}

static void
mywrite (bufp, n)
char *bufp;
{
	char buf1[HPACKET];
	char *sp, *s1;
	char c1, c2, c3;
	short i;

	/* initalize the outward bound packet in case n is 0 */
	buf1[0] = 0x20;

	for (sp = bufp, s1 = buf1, i = 0 ; i < n ; i+=3) {

		/*
		 * load 3 bytes from the input buffer into c1, c2, and c3
		 */
		c1 = *sp++;
		if (i+1 < n)
			c2 = *sp++;
		if (i+2 < n)
			c3 = *sp++;

		/*
		 * convert every 3 bytes in input buffer into 4 bytes
		 * of the output buffer.
		 */
		*s1++ = 0x40 | (c1 & 0xc0)>>2 | (c2 & 0xc0)>>4 | (c3 & 0xc0)>>6;
		*s1++ = 0x40 | (c1 & 0x3f);
		if (i+1 < n)
			*s1++ = 0x40 | (c2 & 0x3f);
		if (i+2 < n)
			*s1++ = 0x40 | (c3 & 0x3f);
	}

	(void)write (tojerq, buf1, (unsigned) (s1 - buf1));

#	ifdef DEBUG
	    if (dumpMode) {
		    short column;
		    char *hexPtr;

		    (void)fprintf (dbfp, "\n%d bytes\n", s1 - buf1);
		    for (hexPtr = buf1; hexPtr < s1; ) {
			    for (column = 0; column < 8; column++) {
					if (hexPtr >= s1)
						break;
					(void)fprintf (dbfp, "%x%x%x%x ",
					    *hexPtr++, *hexPtr++,
					    *hexPtr++, *hexPtr++);
					putc (' ', dbfp);
			    }

			    putc ('\n', dbfp);
		    }
	    }
#	endif /* DEBUG */
}
