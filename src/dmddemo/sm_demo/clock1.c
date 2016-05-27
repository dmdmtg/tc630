/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)clock1.c	1.1.1.2	(11/4/87)";

#include <dmd.h>
#include <font.h>
#ifdef DMD630
#include <5620.h>
#endif

#define	atoi2(p)	((*(p)-'0')*10 + *((p)+1)-'0')
#define	itoa2(n, s)	{ (*(s) = (n)/10 + '0'); (*((s)+1) = n % 10 + '0'); }
Point	ctr, p, cur;
int	dx, dy;
int	h, m, s;	/* hour, min, sec */
int	rh, rm, rs;	/* radius of ... */
int	ah, am, as;	/* angle */
int	rad;
int	olds;

main(argc, argv)
	char *argv[];
{
	char date[40], *p;
	register long oldtime;
	int stat, c, ds;
	int first = 1;

#ifdef DMD630
	local();
#endif
	request(KBD);

	rectf(&display, Drect, F_XOR);
	if(argc!=2){
		jmoveto(Pt(0, 0));
		jstring("Usage: 32ld clock \"`date`\"");
		sleep(600);
		exit();
	}
	initface();
	strcpy(date, argv[1]);
	h = atoi2(date+11);
	m = atoi2(date+14);
	s = atoi2(date+17);

	oldtime=realtime();
	for ( ds = 0;; ) {
		while (realtime() <= oldtime)
			sleep(20);
		ds += realtime()-oldtime;
		oldtime=realtime();
		s += ds / 60;
		ds %= 60;
		if (olds == s)
			continue;
		while (s >= 60) {
			s -= 60;
			m++;
		}
		olds = s;
		while (m >= 60) {
			m -= 60;
			h++;
			if (h >= 24)
				h = 0;
		}

		if (!first) {	/* zap previous rays */
			ray(rs, as);
			if (am != as)
				ray(rm, am);
			if (ah != am && ah != as)
				ray(rh, ah);
		}
		ah = (30 * (h%12) + 30 * m / 60);
		am = 6 * m;
		as = 6 * s;

		if (P->state & RESHAPED) {
			initface();
			first=1;
			P->state &= ~RESHAPED;
		}

	        if ( kbdchar() == 'q' )
			exit();


		if (!first)
			string(&defont,date,&display,
			       add(Drect.origin,Pt(1,1)),F_XOR);
		strcpy(date, "00:00:00");
		itoa2(h, date);
		itoa2(m, date+3);
		itoa2(s, date+6);
		string(&defont,date,&display,
		       add(Drect.origin,Pt(1,1)),first?F_STORE:F_XOR);
		first = 0;
		ray(rs, as);	/* longest */
		if (am != as)
			ray(rm, am);
		if (ah != as && ah != am)
			ray(rh, ah);
	}
}

initface()	/* set up clock circle in window */
{
	rectf(&display, Drect, F_CLR);
	texture(&display, Drect, &T_background, F_STORE);
	ctr.x = (Drect.corner.x + Drect.origin.x) / 2;
	ctr.y = (Drect.corner.y + Drect.origin.y) / 2;
	rad = Drect.corner.x - Drect.origin.x;
	if (rad > Drect.corner.y - Drect.origin.y)
		rad = Drect.corner.y - Drect.origin.y;
	rad = rad/2 - 2;
	rh = 6 * rad / 10;
	rm = 9 * rad / 10;
	rs = rad - 1;
	circle(&display, ctr, rad, F_XOR);
	disc(&display, ctr, rad, F_STORE);
}

ray(r, ang)	/* draw ray r at angle ang */
	int r, ang;
{
	int dx, dy;

	dx = muldiv(r, sin(ang), 1024);
	dy = muldiv(-r, cos(ang), 1024);
	segment(&display, ctr, add(ctr, Pt(dx,dy)), F_XOR);
}
