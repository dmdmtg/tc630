/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)dlsegment.c	1.1.1.3	(11/13/87)";

/*	Draw a line from point p0 to point p1
*	This code was stolen from arc.c
*/

/* includes */
#include "dmd.h"
#ifdef lint
#	include <lintdefs.h>
#endif /* lint */

/* defines */
#define	sgn(x)	((x)<0? -1 : 1)
#define	lclabs(x)	((x)<0? (-x) : (x))

/* procedures */
#ifdef DMD630
	extern void cursallow ();
	extern void cursinhibit ();
	extern void point ();
#endif /* DMD630 */


dlsegment(bp, p0, p1, f, pattern)
	register Bitmap *bp;
	Point p0, p1;
	unsigned short pattern;
{
	short xpen = 0;
	short ypen = 0;
	short xtgt = p1.x - p0.x;
	short ytgt = p1.y - p0.y;
	register xstep = sgn (xtgt);
	register ystep = sgn (ytgt);
	short inhibited = 0;
	long bitmask = 1;
	long slope1;
	long slope2;

	/* I haven't a clue why this is necessary */
	if (f!=F_XOR) {
		cursinhibit();
		inhibited=1;
	}

	point(bp, Pt(p0.x, p0.y), f);

	/*
	 * The idea here, if I understand it right, is to follow the
	 * slope of the line defined by the beginning and ending pts.
	 * Thus, the algorithm steps along in one axis until it reaches
	 * a point that lies on the desired line.  Then, if necessary,
	 * it makes a step along the other axis.  Subsequently, it must
	 * work to reach the desired slope again.
	 *
	 * Would you get a better line by overstepping in both directions
	 * instead of just one?
	 */
	do {
		slope1 = (long)lclabs (((long)xpen + xstep) * ytgt);
		slope2 = (long)lclabs (((long)ypen + ystep) * xtgt);

		if (slope1 < slope2)
			xpen+=xstep;
		else if (slope1 > slope2)
			ypen+=ystep;
		else {
			xpen+=xstep;
			ypen+=ystep;
		}

		/* modulo-64k increment */
		bitmask <<= 1;
		if (bitmask >= 0x10000)
			bitmask = 1;

		if (bitmask & pattern) {
#ifdef MPX
			if (P->layer->obs == (Obscured *)0)
#endif
				point(bp, Pt(p0.x+xpen, p0.y+ypen), f);
		}
	} while (lclabs(xtgt) > lclabs(xpen) || lclabs(ytgt) > lclabs(ypen));

	if(inhibited)
		cursallow();
}
