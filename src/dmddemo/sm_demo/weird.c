/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)weird.c	1.1.1.2	(11/4/87)";


/*
 * Simulate the famous weird program
 */
#include	<dmd.h>

#undef xmax
#undef ymax

Bitmap	*ball;
struct balls	{
	int	x, y;
	int	dx, dy;
} b[100];

struct	balls	*maxball;
#define	MAXV	9

main()
{      
	register xmax, ymax;
	register struct balls *bp;
	register xbase, ybase;
	register struct balls *vbp;
	long epoch = 500;
	long big = 1000;
	int nball;

#ifdef DMD630
	local();
#endif
	request(KBD|MOUSE);
	P->state |= RESHAPED;
	ball = balloc(Rect(0, 0, 16, 16));
	rectf(ball, ball->rect, F_CLR);
	circle(ball, Pt(8, 8), 6, F_XOR);
	while (kbdchar() != 'q') {
		if (P->state&RESHAPED) {
			P->state &= ~RESHAPED;
			rectf(&display, Drect, F_CLR);
			xbase = Drect.origin.x;
			ybase = Drect.origin.y;
			xmax = Drect.corner.x - Drect.origin.x - 16;
			ymax = Drect.corner.y - Drect.origin.y - 16;
			srand(mouse.xy);
			nball = ((long)xmax*ymax) / 2000;
			if (nball > 100)
				nball = 100;
			nball = 10;		/* temp */
			maxball = &b[nball];
			for (bp = b; bp < maxball; bp++) {
			   retry:
				bp->dx = (rand() % MAXV - MAXV/2);
				if (bp->dx == 0)
					bp->dx = (rand() % MAXV - MAXV/2);
				bp->dy = (rand() % MAXV - MAXV/2);
				if (bp->dy == 0)
					bp->dy = (rand() % MAXV - MAXV/2);
				for (vbp = b; vbp < bp; vbp++)
					if (bp->dx==vbp->dx && bp->dy==vbp->dy)
						goto retry;
				bp->x = (xmax/2 + big*xmax - epoch*bp->dx) % xmax;
				bp->y = (ymax/2 + big*ymax - epoch*bp->dy) % ymax;
				bitblt(ball, ball->rect, &display,
				   Pt(bp->x+xbase, bp->y+ybase), F_XOR);
			}
		}
		for (bp = b; bp < maxball; bp++) {
			bitblt(ball, ball->rect, &display,
			   Pt(bp->x+xbase, bp->y+ybase), F_XOR);
			if ((bp->x += bp->dx) < 0)
				bp->x += xmax;
			if (bp->x >= xmax)
				bp->x -= xmax;
			if ((bp->y += bp->dy) < 0)
				bp->y += ymax;
			if (bp->y >= ymax)
				bp->y -= ymax;
			bitblt(ball, ball->rect, &display,
			   Pt(bp->x+xbase, bp->y+ybase), F_XOR);
		}
		sleep(1);
	}
	free(ball);
}
