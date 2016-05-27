/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)maxwell.c	1.1.1.2	(11/4/87)";

/*
 * Maxwell, eat your heart out
 */
#include	<dmd.h>
#include	<font.h>

#define	RAD	6
#define	MAXV	8
#define	SC	5		/* velocity scale factor */
#define	xmin	0
#define	ymin	0

Bitmap	*ball;
Bitmap	*circ;
struct balls	{
	int	x, y;
	int	ox, oy;
	int	dx, dy;
	int	lr;
	int	hot;
	Bitmap	*which;
} b[100];

struct	balls	*maxball;
long	totenergy;

main()
{      
	register xxmax, yymax;
	register struct balls *bp;
	register xbase, ybase;
	register xmid;
	register topdoor, botdoor, realtopdoor;
	int nball, c;
	int oddeven = 0;

#ifdef DMD630
	local();
#endif
	request(MOUSE|KBD);
	P->state |= RESHAPED;
	srand(mouse.xy);
	ball = balloc(Rect(0, 0, 16, 16));
	rectf(ball, ball->rect, F_CLR);
	disc(ball, Pt(RAD, RAD), RAD, F_XOR);
	circ = balloc(Rect(0, 0, 16, 16));
	rectf(circ, circ->rect, F_CLR);
	circle(circ, Pt(RAD, RAD), RAD, F_XOR);
	while ((c = kbdchar()) != 'q') {
		if (P->state&RESHAPED || c == 'r') {
			P->state &= ~RESHAPED;
			xbase = Drect.origin.x;
			ybase = Drect.origin.y;
			xxmax = (Drect.corner.x - Drect.origin.x)<<SC;
			yymax = (Drect.corner.y - Drect.origin.y)<<SC;
			xmid = xxmax/2;
			realtopdoor = muldiv(7, yymax, 16);
			botdoor = muldiv(9, yymax, 16);
			topdoor = botdoor;
			rectf(&display, Drect, F_CLR);
			rectf(&display,
			  Rpt(Pt(xbase+(xmid>>SC)-1,ybase),
			      Pt(xbase+(xmid>>SC)+1, ybase+(yymax>>SC))),
			  F_XOR);
			rectf(&display,
			  Rpt(Pt(xbase+(xmid>>SC)-2, ybase+(realtopdoor>>SC)-1),
			      Pt(xbase+(xmid>>SC)+2, ybase+(realtopdoor>>SC))),
			  F_STORE);
			rectf(&display,
			  Rpt(Pt(xbase+(xmid>>SC)-2, ybase+(botdoor>>SC)),
			      Pt(xbase+(xmid>>SC)+2, ybase+(botdoor>>SC)+1)),
			  F_STORE);
			nball = ((long)xxmax*yymax) / (8000L<<(2*SC));
			if (nball > 100)
				nball = 100;
			maxball = &b[nball];
			for (bp = b; bp < maxball; bp++) {
				bp->dx = 0;
				bp->x = xmid;
			}
			for (bp = b; bp < maxball; bp++) {
				while (bp->x >= xmid-((2*RAD)<<SC) && bp->x <= xmid)
					bp->x = rno(xxmax-xmin-((2*RAD)<<SC)) + xmin;
				bp->y = rno(yymax-ymin-((2*RAD)<<SC)) + ymin;
				while (bp->dx == 0 || bp->dy == 0) {
					bp->dx = rno((2*MAXV)<<SC) - (MAXV<<SC);
					bp->dy = rno((2*MAXV)<<SC) - (MAXV<<SC);
				}
				bp->lr = bp->x > xmid;
				bp->which = rno(2)? ball: circ;
				bitblt(bp->which, bp->which->rect, &display,
				   Pt((bp->x>>SC)+xbase, (bp->y>>SC)+ybase), F_XOR);
				bp->ox = bp->x;
				bp->oy = bp->y;
			}
		}
		oddeven = ~oddeven;
		for (bp = b; bp < maxball; bp++) {
			bp->x += bp->dx;
			if (bp->lr == 0) {
				if (bp->x < xmin) {
					bp->x -= bp->dx;
					bp->dx = -bp->dx;
				} else if (bp->x >= xmid-((2*RAD)<<SC) && bp->dx>0) {
					if (bp->y < topdoor
					 || bp->y+((2*RAD)<<SC) > botdoor) {
						bp->x -= bp->dx;
						bp->dx = -bp->dx;
					} else
						bp->lr = 1;
				}
			} else {
				if (bp->x >= xxmax-((2*RAD)<<SC)) {
					bp->x -= bp->dx;
					bp->dx = -bp->dx;
				} else if (bp->x <= xmid && bp->dx<0) {
					if (bp->y < topdoor
					 || bp->y+((2*RAD)<<SC) > botdoor) {
						bp->x -= bp->dx;
						bp->dx = -bp->dx;
					} else
						bp->lr = 0;
				}
			}
			if ((bp->y += bp->dy) < ymin) {
				bp->y -= bp->dy;
				bp->dy = -bp->dy;
			}
			else if (bp->y >= yymax-((2*RAD)<<SC)) {
				bp->y -= bp->dy;
				bp->dy = -bp->dy;
			}
			if (oddeven || bp->hot) {
				bitblt(bp->which, bp->which->rect, &display,
				   Pt((bp->ox>>SC)+xbase, (bp->oy>>SC)+ybase),
				   F_XOR);
				bp->which = bp->hot? circ: ball;
				bitblt(bp->which, bp->which->rect, &display,
				   Pt((bp->x>>SC)+xbase, (bp->y>>SC)+ybase),
				   F_XOR);
				bp->ox = bp->x;
				bp->which = bp->hot? circ: ball;
				bp->oy = bp->y;
			}
		}
		collisions();
		if (bttn2()) {
			if (topdoor == botdoor) {
				topdoor = realtopdoor;
				rectf(&display,
				 Rpt(Pt(xbase+(xmid>>SC)-1, ybase+(realtopdoor>>SC)),
				     Pt(xbase+(xmid>>SC)+1, ybase+(botdoor>>SC))),
				     F_XOR);
			}
		} else {
			if (topdoor != botdoor) {
				topdoor = botdoor;
				rectf(&display,
				 Rpt(Pt(xbase+(xmid>>SC)-1, ybase+(realtopdoor>>SC)),
				     Pt(xbase+(xmid>>SC)+1, ybase+(botdoor>>SC))),
				     F_XOR);
			}
		}
		sleep (1);
	}
}

collisions()
{
	register struct balls *bp1, *bp2;
	register dx, dy;
	register rx, ry, cmx, cmy, vx, vy;
	long w, z;
	char ebuf[32];

	for (bp1=b; bp1<maxball-1; bp1++) {
		for (bp2 = bp1+1; bp2<maxball; bp2++) {
			dx = bp1->x - bp2->x;
			if (dx < 0)
				dx = -dx;
			if (dx > ((2*RAD)<<SC))
				continue;
			dy = bp1->y - bp2->y;
			if (dy < 0)
				dy = -dy;
			if (dy > ((2*RAD)<<SC))
				continue;
			if ((long)dx*dx + (long)dy*dy > ((4L*RAD*RAD)<<(2*SC)))
				continue;
			/* collision */
			cmx = (bp1->dx + bp2->dx) / 2;
			cmy = (bp1->dy + bp2->dy) / 2;
			vx = bp1->dx - cmx;
			vy = bp1->dy - cmy;
			rx = bp2->x - bp1->x - bp2->dx + bp1->dx;
			ry = bp2->y - bp1->y - bp2->dy + bp1->dy;
			z = (long)vx*rx + (long)vy*ry;
			w = (long)rx*rx + (long)ry*ry;
			if (w < 4)
				continue;
			rx = ((2*rx*z)+(w/2)) / w;
			ry = ((2*ry*z)+(w/2)) / w;
			vx -= rx;
			vy -= ry;
			/* hack */
			if (totenergy < 0) {
				if (cmx < 0)
					cmx--;
				else
					cmx++;
				if (cmy < 0)
					cmy--;
				else
					cmy++;
			}
			totenergy -= ((long)bp1->dx*bp1->dx
				     +(long)bp1->dy*bp1->dy
				     +(long)bp2->dx*bp2->dx
				     +(long)bp2->dy*bp2->dy) >> SC;
			bp1->dx = cmx + vx;
			bp1->dy = cmy + vy;
			bp2->dx = cmx - vx;
			bp2->dy = cmy - vy;
			totenergy += ((long)bp1->dx*bp1->dx
				     +(long)bp1->dy*bp1->dy
				     +(long)bp2->dx*bp2->dx
				     +(long)bp2->dy*bp2->dy) >> SC;
#define H	((MAXV/2)<<SC)
			bp1->hot = bp1->dx>H||bp1->dx<-H||bp1->dy>H||bp1->dy<-H;
			bp2->hot = bp2->dx>H||bp2->dx<-H||bp2->dy>H||bp2->dy<-H;
		}
	}
}

rno(n)
{
	return(rand() % n);
}
