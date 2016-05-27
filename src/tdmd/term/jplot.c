/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)jplot.c	1.1.1.4	(3/31/88)";

/* includes */
#include <dmd.h>
#ifdef DMD630
#	include <5620.h>
#	include <object.h>
#endif
#include <font.h>
#include <tdmd.h>
#ifdef lint
#	ifdef DMD630
#		include <menu.h>
#	endif /* DMD630 */
#	include <lintdefs.h>
#endif /* lint */

/* defines */
#define Options "c"	/* for caching */
#define	CW	9	/* width of a character */
#ifdef  NS
#undef  NS
#endif
#define	NS	16	/* height of a character */
#ifdef DMD630
#	define Texture Texture16
#endif

#define	XMARGIN	3	/* inset from border of layer */
#define	YMARGIN	3

/* externals */
extern int optind;
extern char *optarg;
extern int optptr;

/* globals */
Point
	PtCurr;	/* current position */

Rectangle
	arect;		/* the aesthetic rectangle in a layer */

int
	xdelta,		/* distance along x axis */
	ydelta,		/* distance along y axis */
	delta;		/* min (xdelta, ydelta) */

#ifndef MPX
int
	ready = 0,	/* send out READY every PACKET chararacters */
	incount = 0;	/* number of characters until we output a READY */
#endif

/* locals */
static unsigned int pattern;	/* linemod */
static unsigned int linemodes[] = {
	0xffff,			/* solid line  */
	0x03ff,			/* long dashed */
	0x0f0f,			/* short dashed */
	0x3333,			/* dotted line */
	0x0c3f 			/* dot- dashed */
};
static short quitNow = 0;
static short cacheText = 0;

/* procedures */
Texture gettext();
Point getpt ();
extern void dlarc ();
extern Point pt ();
#ifdef DMD630
	extern void sleep ();
	extern void ringbell ();
	extern void exit ();
	extern int kbdchar ();
	extern int rcvchar ();
	extern Point string ();
	extern void bitblt ();
	extern void rectf ();
#endif /* DMD630 */


main(argc, argv)

int argc;
char *argv [];
{
	char
		buf[2]	/* make a string out of one character */
	;
	Point
		p0,	/* temporary points */
		p1,
		p2
	;
	Point points[64];	/* array for polygon points */
	short i;		/* index for polygon points */
	int num;		/* number of polygon points */
	Texture textp;		/* texture for filling      */
	int option;

        /*
         * the standard getopt() relies on static initialized storage.
         * Since getopt() is called before cache() (to handle the -c
         * flag), the cache static initializer snapshot will be wrong.
         * Therefore, always initialize these globals, instead.
         */
        optind = 1;
        optptr = 1;

	while ((option = getopt (argc, argv, Options)) != -1)
	    switch (option) {
	    case 'c':
		cacheText = 1;
		break;
	    default:
		break;
	    }

#	ifdef DMD630
	    if (cacheText)
		    cache (NULL, A_NO_SHOW);
#	endif

	buf[1] = '\0';
#ifndef MPX
	request(SEND | RCV | KBD);
#else
	request (RCV);
#endif

	arect = Drect;
	arect.origin.x += XMARGIN;
	arect.corner.x -= XMARGIN;
	arect.origin.y += YMARGIN;
	arect.corner.y -= YMARGIN;

	xdelta = arect.corner.x - arect.origin.x;
	ydelta = arect.corner.y - arect.origin.y;
	/* scale the square window to the smaller of the two dimensions */
	delta = xdelta > ydelta ? ydelta : xdelta;

	PtCurr = arect.origin;
	pattern = 0xffff;

	while (!quitNow) {
		switch(buf[0] = getch() & 0177) {

		case '\007':		/* bell */
			ringbell();
			break;

		case '\t':		/* tab modulo 8 */
			PtCurr.x = (PtCurr.x | (7 * CW)) + CW;
			break;

		case OPEN:
			while ((buf[0] = getch()) != CLOSE)
				switch(buf[0]) {

				case DELAY:		/* delay */
					sleep(60);
					break;

				case FILL:			/* fill */
					i = (int) getch();
					textp = gettext(i);
					num = (int) getch();
					for (i = 0 ; i < num ; i++)
						points[i] = getpt();
					polyf(&display,points,num,&textp,F_STORE);
					break;

				case LINEMOD:	/* change line mode */
					pattern = linemodes[getch()];
					break;
	
				case ARC:	/* arc's Pcenter, Pstart, Pfinish */
					p0 = getpt();
					p1 = getpt();
					p2 = getpt();
					dlarc(&display, p0, p1, p2, F_OR, pattern);
					PtCurr = p2;
					break;
	
				case ERASE:	/* erase screen */
					stipple(arect);
					PtCurr = arect.origin;
					break;
	
				case MOVE:	/* move to point */
					PtCurr = getpt();
					break;
	
#ifndef MPX
				case OFF:	/* stop sending READYs */
					ready = 0;
					break;

				case ON:	/* start sending READYs */
					incount = PACKET;
					++ready;
					sendchar(READY);
					break;

#endif
				case CONT:	/* continue to P */
					dlsegment(&display, PtCurr, p0 = getpt(), F_OR, pattern);
					PtCurr = p0;
					break;

				case EXIT:
					request (KBD);
					do {
					    wait (KBD);
					} while (kbdchar () == -1);
					quitNow = 1;
					break;
				}
			break;

		case '\b':		/* backspace */
			PtCurr.x -= CW;
			if(PtCurr.x < arect.origin.x)
				PtCurr.x = arect.origin.x;
			break;

		case '\n':		/* linefeed */
			newline();

		case '\r':		/* carriage return */
			PtCurr.x = arect.origin.x;
			break;

		default:		/* ordinary char */
			(void)string (&defont, buf, &display, PtCurr, F_XOR);
			PtCurr.x += CW;
			break;
		}

		if(PtCurr.x > arect.corner.x - CW) {
			PtCurr.x = arect.origin.x;
			newline();
		}
	}

	exit (0);	/*NOTREACHED*/
}

newline()
{
	if(PtCurr.y >= arect.corner.y - 2 * NS) {
		bitblt(&display, Rpt(Pt(arect.origin.x, arect.origin.y + NS), 
			arect.corner), &display, arect.origin, F_STORE);
		stipple(Rpt(Pt(arect.origin.x, arect.corner.y - NS),
			Drect.corner));
	} else
		PtCurr.y += NS;
}

getch()
{
	register c;

	while ((c = rcvchar()) == -1) {
#ifdef MPX
		wait(RCV);
#endif
	}
#ifndef MPX
	if (ready && --incount == 0) {
		sendchar(READY);
		incount = PACKET;
	}
#endif
	return(c);
}

Point
getpt()
{
	Point	p;
	short coordinate;

	coordinate = getch();
	coordinate |= getch () << 8;

	/*
	 * to the origin of the window, add the coordinate
	 * times the ratio of the smaller dimension to the maximum 
	 * possible dimension.  Thus, this code scales the image to
	 * fit the window.
	 */
	p.x = arect.origin.x + muldiv(coordinate, delta, XMAX);

	coordinate = getch();
	coordinate |= getch () << 8;

	p.y = arect.origin.y + muldiv(coordinate, delta, XMAX);

	return (p);
}

stipple(r)
Rectangle r;
{
	rectf(&display, r, F_CLR);
}

Texture
gettext (i)
{
	switch (i % 7) {
		case 0:
			return (T_grey);
		case 1:
			return (T_lightgrey);
		case 2:
			return (T_darkgrey);
		case 3:
			return (T_checks);
		case 4:
			return (T_black);
		case 5:
			return (T_white);
		default:
			return (T_background);
	}
}
