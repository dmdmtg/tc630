/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)rae.c	1.1.1.2	(11/10/87)";

/*
 * Simulate a bouncing ball
 */
#include	<dmd.h>

#undef	XMAX
#undef	YMAX
#define HEIGHT  32
#define WIDTH   32
#define XMIN    (Drect.origin.x+WIDTH)
#define YMIN    (Drect.origin.y+HEIGHT)
#define XMAX    (Drect.corner.x-WIDTH)
#define YMAX    (Drect.corner.y-HEIGHT)
#define TOP     YMIN
#define BOT     (YMAX-1)

int     GND[128];
int     xpos;
Bitmap	*ball;

main()
{      
	register i;
	register time;
	int x, xx, y, yy, xvel, yvel, xacc, yacc, ypos, offset;

#ifdef DMD630
	local();
#endif
again:;
	rectf(&display, display.rect, F_XOR);
	ball = balloc(Rect(0, 0, 16, 16));
	rectf(ball, ball->rect, F_CLR);
	disc(ball, Pt(8, 8), 7, F_XOR);
	segment(&display, Pt(XMIN, YMIN), Pt(XMIN, YMAX), F_XOR);
	segment(&display, Pt(XMIN, YMAX), Pt(XMAX, YMAX), F_XOR);
	segment(&display, Pt(XMAX, YMAX), Pt(XMAX, YMIN), F_XOR);

	for (i=0; i<128; i++) {
		x = i<<3;
		if (x <= XMIN+8 || x >= XMAX-8)
			GND[i] = 0;
		else {
			GND[i] = BOT - 9;
			if (i&01)
				if (GND[i-1]==0)
					GND[i] = 0;
				else
					GND[i] -= 7;
		}
	}
	request(KBD);
	while (kbdchar() != 'q') {
		/*
		* Check for reshaping
		*/
		if (P->state & RESHAPED)
		{
			P->state &= ~RESHAPED;
			rectf(&display, display.rect, F_CLR);
			goto again;
		}
		xpos = rand() & 0177;
		if (GND[xpos] <= TOP) {
			/*
			* Check for full box
			*/
			for (xpos = 0; xpos < 128; xpos++)
				if (GND[xpos] > TOP)
					break;
			if (xpos >= 128)	/* there is no more room */
			{
				drop_balls();
				goto again;
			}
			sleep(1);
			continue;
		}
		x = xpos << 3;
		xvel = 4 - ((rand()|01)&07);
		yacc = 1;
		yvel = 0;
		y = TOP;
		for (time = 0;; time++) {
			if (kbdchar()=='q')
				goto out;
			drawit(x,y);
			xx = x; 
			yy = y;         /* save x and y */
			yvel += yacc;
			y += yvel;
			x += xvel;
			if (y > GND[x>>3]) {	/* bounce? */
				if (yvel>5) {
					drawit(xx, yy);
					drawit(x, y);
					sleep(1);
					drawit(x, y);
					drawit(xx, yy);
				}
				if (y <= GND[xx>>3]) { /* side collision? */
					x = xx;
					xvel = -xvel;
				} else if (yy <= GND[x>>3]) { /*bottom? */
					y = yy;
					yvel = -yvel;
				} else {	/* corner */
					x = xx;
					y = yy;
					xvel = -xvel;
					yvel = -yvel;
				}
				if ((time & 017)==0)
					xvel = decay(xvel);
				if (xvel == 0) {
					xpos = x>>3;
					if (GND[xpos-1]<GND[xpos]
					 && GND[xpos]<GND[xpos+1])
						xvel = 1;
					/* roll left */
					else if (GND[xpos-1]>GND[xpos]
					      && GND[xpos]>GND[xpos+1])
						xvel = -1;
					/* on hilltop */
					else if (GND[xpos-1]>GND[xpos]
					      && GND[xpos]<GND[xpos+1]) {
						if (rand() & 01)
							xvel = 1;
						else
							xvel = -1;
					}
				}
				yvel = decay(yvel);
			}
			sleep(1);
			drawit(xx,yy);
			if (xvel==0 && yvel==0 && y > GND[x>>3]-4)
				break;
		}
		for (;;) {
			/* find stable position */
			if (GND[xpos-1]<GND[xpos] && GND[xpos]>GND[xpos+1]) {
				drawit(xpos<<3, GND[xpos]);
				GND[xpos] -= 21;
				if (GND[xpos-1]<=0)
					GND[xpos] -= 7;
				GND[xpos+1] -= 7;
				break;
			}
			/* roll right */
			if (GND[xpos-1]<GND[xpos] && GND[xpos]<GND[xpos+1]) {
				xpos++;
				continue;
			}
			/* roll left */
			if (GND[xpos-1]>GND[xpos] && GND[xpos]>GND[xpos+1]) {
				xpos--;
				continue;
			}
			/* on hilltop, choose at random */
			if (GND[xpos-1]>GND[xpos] && GND[xpos]<GND[xpos+1]) {
				if (rand() & 01)
					xpos++;
				else
					xpos--;
				continue;
			}
			/* else botch */
			drawit(xpos<<3, GND[xpos]);
			rectf(&display, display.rect, F_XOR);
			break;
		}
	}
out:
	bfree(ball);
}

drawit(x,y)
int x,y;
{       
	int i;

	if (x<XMIN)
		x = XMIN+8;
	if (y<TOP)
		y = TOP+8;
	if (x>XMAX)
		x = XMAX-8;
	xpos = x>>3;
	if (y>GND[xpos])
		y = GND[xpos];
	bitblt(ball, ball->rect, &display, Pt(x-12, y-8), F_XOR);
}

decay(v)
register v;
{
	if (v==0)
		return(v);
	if (v > 0)
/*
  compiler does not do arithmetic shifts so
	return(v-1-(v>>3));
  was changed to
    	return(v+1+(abs(v)>>3))
*/

		return(v-1-(v>>3));
	return(v+1+(abs(v)>>3));
}

drop_balls()	/* dump the balls so far out of the bottom */
{
	int dis, i;

	/*
	* Slide the bottom "door" to the left
	*/
	segment(&display, Pt(Drect.origin.x, YMAX), Pt(XMIN, YMAX), F_XOR);
	segment(&display, Pt(XMIN, YMAX), Pt(XMAX, YMAX), F_XOR);
	sleep(2);
	/*
	* The whole pile of balls fall out of the open door
	*/
	dis = 0;
	i = 1;
	while (i < YMAX)
	{
		bitblt(&display, Rect(XMIN + 1, Drect.origin.y + dis,
			XMAX - 1, YMAX + dis), &display,
			Pt(XMIN + 1, YMIN + i), F_STORE);
		rectf(&display, Rect(XMIN + 1, Drect.origin.y + dis,
			XMAX - 1, YMIN + dis + i), F_STORE);
		sleep(5);
		dis = i;
		if (i < 16)
			i <<= 1;
		else
			i += 16;
	}
	rectf(&display, display.rect, F_CLR);
	sleep(5);
}
