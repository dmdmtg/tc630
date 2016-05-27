/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)clock2.c	1.1.1.2	(11/4/87)";

/*
* clock2 - get time right, essentially independent of download times.
*	Add alarm setting capability.
*/

#include <dmd.h>
#include <font.h>
#ifdef DMD630
#include <5620.h>
#endif

long bits_alarmclock[] =	/* assumes 32 bit, big endian Words */
{
	 0x03c00000, 0x34200000, 0x37e00000, 0x13c00000,
	 0x17f00000, 0x18680000, 0x20740000, 0x20d40000,
	 0x418a0000, 0x430a0000, 0x430a0000, 0x418a0000,
	 0x20940000, 0x201c0000, 0x787e0000, 0x67F60000
};

Bitmap alarmclock =	/* denotes whether alarm is on */
{
	(Word *)bits_alarmclock, sizeof(long)/sizeof(Word), {{0, 0}, {16, 16}}	/* assumes 32 bit Words */
};

Texture16 deadmouse =	/* used when mouse will not be noticed */
{
	0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0008, 0x0004, 0x0082,
	0x0441, 0xffe1, 0x5ff1, 0x3ffe,
	0x17f0, 0x03e0, 0x0000, 0x0000
};

#define	atoi2(p)	((*(p)-'0')*10 + *((p)+1)-'0')
#define	itoa2(n, s)	{ (*(s) = (n)/10 + '0'); (*((s)+1) = n % 10 + '0'); }
Point	ctr, p, cur;
int	dx, dy;
int	h, m, s;	/* hour, min, sec */
int	rh, rm, rs;	/* radius of ... */
int	ah, am, as;	/* angle */
int	rad;
int	olds, old_h_m;
int	alarm_on;	/* flag for alarm */
int	want_time = 1;	/* set if should ask for time from host */
int	show_alarm;	/* display time or display alarm setting */
int	alarm_hour = 17;/* alarm setting, default is 5:00pm */
int	alarm_minute;	/*	ditto */
int	alarming;	/* set while alarm is going off */
char	last_top[9];	/* last displayed on top line */
char	last_bot[11];	/* last displayed on bottom line */

#define MAX_ALARMING	120	/* number of seconds until alarming stops */

main(argc, argv)
	char *argv[];
{
	char date[40], *p;
	register long oldtime;
	int stat, c, ds;
	int first = 1;

	request(KBD | MOUSE);

	rectf(&display, Drect, F_XOR);
	initface();

	oldtime=realtime();
	for ( ds = 0;; ) {
		if (want_time)
		{
			unix_date(&h, &m, &s);
			want_time = 0;
		}
		else
		{
			while (realtime() <= oldtime)
			{
				if (alarming)
				{
					rectf(&display, display.rect, F_XOR);
					sleep(10);
					rectf(&display, display.rect, F_XOR);
					sleep(10);
				}
				else
					sleep(20);
			}
			ds += realtime()-oldtime;
			oldtime=realtime();
			s += ds / 60;
			ds %= 60;
			if (olds == s)
				continue;
			if (alarming)
			{
				if (++alarming < MAX_ALARMING)
					ringbell();
				else
					alarming = 0;
			}
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
				want_time = 1;	/* check host's time */
			}
		}

		/*
		* h, m, and s have been set.  Check for the alarm going off.
		*/
		if (!alarming && alarm_on)
		{
			long al = 60 * alarm_hour + alarm_minute;
			long now = 60 * h + m;

			if (now >= al && (old_h_m < al || old_h_m > now))
			{
				alarming = 1;
				ringbell();
			}
		}
		old_h_m = 60 * h + m;

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
			if (alarm_on)
				disp_alarm();
			want_time = first = 1;
			last_top[0] = last_bot[0] = '\0';
			P->state &= ~RESHAPED;
		}

		/*
		* Check for mouse commands
		*/
		if (own() & MOUSE)
		{
			if (bttn1())
				do_mouse();
			else if (bttn23())	/* let others use also */
			{
				request(0);
				sleep(1);
				request(KBD | MOUSE);
			}
		}

		c = kbdchar();
		if (c == 'q')
			exit();
		else if (c != -1)	/* wants to re-sync */
			want_time = 1;

		strcpy(date, "00:00:00");
		if (show_alarm)
		{
			itoa2(alarm_hour, date);
			itoa2(alarm_minute, date + 3);
		}
		else
		{
			itoa2(h, date);
			itoa2(m, date + 3);
			itoa2(s, date + 6);
		}
		disp_top(date);
		ray(rs, as);	/* longest */
		first = 0;
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

#define NUMTRIES	4	/* number of times to attempt to get date */
#define TENTH_SEC	6	/* a tenth of a seconds' worth of timeout */

#define ISDIG(ch)	((ch) >= '0' && (ch) <= '9')
#define ISCOL(ch)	((ch) == ':')

unix_date(hour, minute, sec)		/* set from unix shell/date */
	int *hour, *minute, *sec;
{
	char buf[100];
	char *msg = "Query Fail";	/* default error message */
	int i;

	cursswitch(&deadmouse);
	/*
	* Give the process NUMTRIES to succeed
	*/
	for (i = 0; i < NUMTRIES; i++)
	{
		/*
		* Get all the characters waiting to be eaten (within
		* the timeout period).
		*/
		while (rcv_line(NULL, 50 * TENTH_SEC) == 0)
			;
		sendnchars(10, "/bin/date\n");		/* send the command */
		if (rcv_line(NULL, 100 * TENTH_SEC))	/* eat command echo */
		{
			msg = "UNIX Gone?";
			break;
		}
		if (rcv_line(buf, 100 * TENTH_SEC))	/* eat answer */
		{
			msg = "Date Gone?";
			break;
		}
		/*
		* We now have a possible answer in buf, do a quick
		* check to see if it is what we expect.  Only set
		* hour, min, sec if everything looks ok.
		*/
		if (ISDIG(buf[11]) && ISDIG(buf[12]) && ISCOL(buf[13])
			&& ISDIG(buf[14]) && ISDIG(buf[15]) && ISCOL(buf[16])
			&& ISDIG(buf[17]) && ISDIG(buf[18]))
		{
			*hour = atoi2(&buf[11]);
			*minute = atoi2(&buf[14]);
			*sec = atoi2(&buf[17]);
			buf[10] = '\0';
			msg = buf;
			break;
		}
	}
	disp_bot(msg);
	cursswitch((Texture16 *)0);
}

disp_top(msg)	/* place a new message in the top area */
	char *msg;
{
	disp(last_top, Drect.origin, msg);
}

disp_bot(msg)	/* place a new message in the bottom area */
	char *msg;
{
	disp(last_bot, Pt(Drect.origin.x, Drect.corner.y - 14), msg);
}

disp(old, pt, new)	/* display a message */
	char *old, *new;
	Point pt;
{
	Code f;

	if (old[0] != '\0')
		string(&defont, old, &display, pt, f = F_XOR);
	else
		f = F_STORE;
	string(&defont, new, &display, pt, f);
	strcpy(old, new);
}

disp_alarm()
{
	bitblt(&alarmclock, alarmclock.rect, &display,
		Pt(ctr.x - rad, ctr.y - 8), F_XOR);
}

rcv_line(p, timeout)
	register char *p;	/* place line here (ignore if NULL) */
	int timeout;		/* timeout limit (in TENTH_SEC) */
{
	register int w;
	long lastrcv;

	request(KBD | RCV | CPU | MOUSE);
	lastrcv = realtime();
	for (;;)
	{
		w = wait(KBD | RCV | CPU);
		if (w & KBD && kbdchar() == 'q')
			exit();
		if (w & RCV)
		{
			w = rcvchar();
			if (p != NULL)
				*p++ = w;
			if (w == '\n')
				break;
			lastrcv = realtime();
		}
		else if (w & CPU)
		{
			/*
			* No character from host, see if
			* timeout limit has been reached.
			*/
			if (realtime() - lastrcv >= timeout)
			{
				request(KBD | MOUSE);
				return (1);
			}
		}
	}
	request(KBD | MOUSE);
	return (0);
}

char drag_time[12];	/* where current time for drag_ray() is kept */

do_mouse()	/* deal with mouse commands (all on button 1) */
{
	static char *items[] =	/* unchanging menu items */
	{
		"show time",
		"set time",
		"show alarm",
		"set alarm",
		0, 0
	};
	static char *on_off[] = {" on /[off]", "[on]/ off "};
	static Menu menu = {items};
	int a_ah, a_am, ret, step;
	char date[11];

	items[4] = on_off[alarm_on];
	switch (menuhit(&menu, 1))
	{
	case 0:
		show_alarm = 0;
		return;
	case 1:
		want_time = 1;
		return;
	case 2:
		show_alarm = 1;
		return;
	case 3:
		break;
	case 4:
		alarm_on = !alarm_on;
		disp_alarm();
		alarming = 0;
		/*FALLTHROUGH*/
	default:
		return;
	}
	/*
	* Only here if selected "set alarm"; drag until time is right.
	* Display old alarm setting.
	*/
	strcpy(drag_time, "00:00:00");
	itoa2(alarm_hour, drag_time);
	itoa2(alarm_minute, drag_time + 3);
	disp_top(drag_time);
	/*
	* Get new alarm setting.
	*/
	strcpy(date, last_bot);
	step = 0;
	a_ah = 15 * alarm_hour;
	a_am = 6 * alarm_minute;
	ray(rm, a_am);
	ray(rh, a_ah);
	for (;;)
	{
		if (step == 0)
		{
			disp_bot("Set HOUR  ");
			ret = drag_ray(1, &a_ah);
		}
		else if (step == 1)
		{
			disp_bot("Set MINUTE");
			ret = drag_ray(0, &a_am);
		}
		else
		{
			disp_bot("Set More??");
			ret = drag_ray(2, (int *)0);
		}
		switch (ret)	/* determine what's next */
		{
		case 'q':
			exit();
		case -1:	/* no character typed */
			if (++step > 2)
				break;
			continue;
		case 'm':
			step = 1;
			continue;
		case 'x':
			break;
		case 'h':
			step = 0;
			continue;
		case ' ':
			step = !step;
			/*FALLTHROUGH*/
		default:	/* unknown character typed */
			continue;
		}
		break;	/* only here if want out */
	}
	if (!alarm_on)
	{
		disp_alarm();
		alarm_on = 1;
	}
	/*
	* Clean up.
	*/
	disp_bot(date);
	ray(rm, a_am);
	ray(rh, a_ah);
}

drag_ray(is_hour, new_ang)	/* use button 1 to drag to new time */
	int is_hour;	/* set if setting hour time */
	int *new_ang;
{
	static int conv[] =	/* table to convert slope to index */
	{
		5,	11,	16,	21,	27,	/*  3,  6,  9, 12, 15 */
		32,	38,	45,	51,	58,	/* 18, 21, 24, 27, 30 */
		65,	73,	81,	90,	100,	/* 33, 36, 39, 42, 45 */
		111,	123,	138,	154,	173,	/* 48, 51, 54, 57, 60 */
		196,	225,	261,	308,	373,	/* 63, 66, 69, 72, 75 */
		470,	631,	951,	1908	/*inf*/	/* 78, 81, 84, 87 (90)*/
	};
	Point mcur;
	int angle;
	register int i;
	int ans;
	int slope;

	/*
	* Wait for drag to begin.
	*/
	for (;;)
	{
		i = wait(KBD | MOUSE);
		if (i & KBD)
			return (kbdchar());
		else if (bttn1())
		{
			if (is_hour > 1)
			{
				while (bttn1())
					;
				return (-1);
			}
			break;
		}
	}
	angle = *new_ang;
	/*
	* We now have button pressed, follow until let go of.
	*/
	while (bttn1())
	{
		ray(is_hour ? rh : rm, angle);
		/*
		* Find the angle between the mouse cursor and the
		* center of the clock.  This determines the time.
		* Assume the hours version, then scale if for minutes.
		*/
		mcur = mouse.xy;
		if (ctr.x == mcur.x)			/* 0 or 60 */
		{
			if (ctr.y > mcur.y)
				ans = 0;
			else if (ctr.y < mcur.y)
				ans = 60;
			else
			{
				ans = is_hour ? 5 * alarm_hour
					: 2 * alarm_minute;
			}
		}
		else if (ctr.y == mcur.y)		/* 30 or 90 */
		{
			if (ctr.x > mcur.x)
				ans = 90;
			else
				ans = 30;
		}
		else	/* must calculate somehow */
		{
			slope = abs(((ctr.y - mcur.y) * 100)
				/ (ctr.x - mcur.x));
			/*
			* Calculate index based on scaled slope.
			*/
			for (i = 0; i < sizeof(conv) / sizeof(conv[0]); i++)
				if (slope < conv[i])
					break;
			if (ctr.x > mcur.x)		/* 60 <= ans <= 119 */
			{
				if (ctr.y > mcur.y)	/* 90 <= ans <= 119 */
					ans = i + 90;
				else	/* ctr.y < mcur.y; 60 <= ans <= 89 */
					ans = 89 - i;
			}
			else	/* ctr.x < mcur.x */	/* 0 <= ans <= 59 */
			{
				if (ctr.y > mcur.y)	/* 0 <= ans <= 29 */
					ans = 29 - i;
				else	/* ctr.y < mcur.y; 30 <= ans <= 59 */
					ans = i + 30;
			}
		}
		/*
		* We now have ans set for the correct reading for a
		* 24 hour clock.  Display the new ray and value.
		*/
		if (is_hour)
		{
			alarm_hour = ans / 5;
			angle = alarm_hour * 15;
			itoa2(alarm_hour, drag_time);
			ray(rh, angle);
		}
		else
		{
			alarm_minute = ans / 2;
			angle = alarm_minute * 6;
			itoa2(alarm_minute, drag_time + 3);
			ray(rm, angle);
		}
		disp_top(drag_time);
		nap(2);		/* to avoid beating with the refresh */
	}
	*new_ang = angle;
	return (-1);
}
