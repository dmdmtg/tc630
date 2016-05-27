/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)cell.c	1.1.1.1	(11/4/87)";

#include <dmd.h>
#ifdef DMD630
#include <5620.h>
#endif

#define NSTACK	6

struct	cell {
	long	value;
	Point	loc;
};

struct cell LedDisplay;

struct cell Base = {16, };


struct cell Stack[NSTACK];
int Nstack;
int Error = 0;	/* Indicates ERROR being displayed if 1 */

initcell()
{
	register int i;
	int cellwidth, cellhight, xdelta, ydelta;
	int xorigin, yorigin;
	Rectangle t;

	cellwidth = XMAX/3;
	cellhight = YMAX/12;
	xdelta = cellwidth/10;
	ydelta = cellhight/3;

	xorigin = XMAX/10;
	yorigin = YMAX/10;
	LedDisplay.loc.x = xorigin + xdelta;
	LedDisplay.loc.y = yorigin + ydelta;
	drawcell(LedDisplay);
	if (Error) {
		displayerror ();
	}
	t = Rect(xorigin, yorigin, xorigin + cellwidth, yorigin + cellhight);
	jrectf(t, F_XOR);
	t.origin.x += xdelta/2;
	t.origin.y += ydelta/2;
	t.corner.x -= xdelta/2;
	t.corner.y -= ydelta/2;
	jrectf(t, F_XOR);

	xorigin = XMAX/2;

	for (i = 0; i < NSTACK; i++) {
		yorigin = YMAX/5 + (cellhight + cellhight/3) * i;
		Stack[i].loc.x = xorigin + xdelta;
		Stack[i].loc.y = yorigin + ydelta;
		t = Rect(xorigin, yorigin, xorigin + cellwidth, yorigin + cellhight);
		jrectf(t, F_XOR);
 		t.origin.x += xdelta/2;
		t.origin.y += ydelta/2;
		t.corner.x -= xdelta/2;
		t.corner.y -= ydelta/2;
		jrectf(t, F_XOR);
		if (i < Nstack)
			drawcell(Stack[i]);
	}
	Base.loc.x = xorigin;
	Base.loc.y = YMAX/10 + ydelta;
	drawbase();
}

changebase(new)
{
	register int i, j;

	if (new != 8 && new != 10 && new != 16)
		return;
	for (i = 0; i < 2; i++) {
		drawcell(LedDisplay);
		for (j = 0; j < Nstack; j++)
			drawcell(Stack[j]);
		drawbase();
		Base.value = new;
	}
}

drawbase()
{
	jmoveto(Base.loc);
	switch (Base.value) {
	case 10:
		jstring("BASE = 10");
		break;
	case 8:
		jstring("BASE = 8");
		break;
	case 16:
		jstring("BASE = 16");
		break;
	}
}

drawcell(cell)
struct cell cell;
{
	jmoveto(cell.loc);
	jputnumber(cell.value, Base.value);
}

long
getdisplay()
{
	return LedDisplay.value;
}

putdisplay(n)
long n;
{
	drawcell(LedDisplay);
	LedDisplay.value = n;
	drawcell(LedDisplay);
}

displayerror ()
{
	drawcell (LedDisplay);
	jmoveto (LedDisplay.loc);
	jstring ("ERROR");
	Error = 1;
}

enterdisplay(c)
char c;
{
	drawcell(LedDisplay);
	LedDisplay.value = numvalue(c);
	drawcell(LedDisplay);

	for (;;) {
		c = getkey();
		if (!isdigit(c)) {
			if (isop(c))
				break;
			else
				continue;
		}
		drawcell(LedDisplay);
		LedDisplay.value = LedDisplay.value * Base.value + numvalue(c);
		drawcell(LedDisplay);
	}

	ungetkey(c);
}

numvalue(c)
char c;
{
	if (c >= '0' && c <= '9')
		return c - '0';
	c |= 'a' - 'A';	  /* Convert to upper case */
	return c - 'a' + 10;
}

jputnumber(val, base)
long val, base;
{
	char buf[12];
	int i, hradix, plmax, lowbit;

	switch (base) {
	default:
	case 10:
		if (val < 0) {
			jputchar('-');
			val = -val;
		}
		hradix = 5;	/* half radix: 5 for base 10 */
		plmax = 10;	/* max places: 2^32 takes 10 places */
		break;
	case 16:
		hradix = 8;
		plmax = 8;
		break;
	case 8:
		hradix = 4;
		plmax = 11;
		break;
	}

	for (i = 0; i < plmax; i++) {
		lowbit = val & 1;
		val = ((unsigned long)val) >> 1;
		buf[i] = "0123456789ABCDEF"[val % hradix * 2 + lowbit];
		val /= hradix;
		if(val == 0)
			break;
	}
	if(i == plmax)
		i--;
	for(; i >= 0; i--) {
		jputchar(buf[i]);
	}
}

push(a)
long a;
{
	register int i;

	if (Nstack >= NSTACK) {
		ringbell();
		return;
	}
	for (i = Nstack; i > 0; i--) {
		drawcell(Stack[i-1]);
		Stack[i].value = Stack[i-1].value;
	}
	Stack[0].value = a;
	Nstack++;
	for (i = 0; i < Nstack; i++)
		drawcell(Stack[i]);
}

long
pop()
{
	register long i, ret;

	if (Nstack <= 0) {
		ringbell();
		return 0;
	}
	ret = Stack[0].value;
	for (i = 0; i < Nstack; i++) {
		drawcell(Stack[i]);
		Stack[i].value = Stack[i+1].value;
	}
	--Nstack;
	for (i = 0; i < Nstack; i++)
		drawcell(Stack[i]);
	return ret;
}

isempty()
{
	if (Nstack == 0)
		return 1;
	else
		return 0;
}

isdigit(c)
{
	switch (Base.value) {
	case 8:
		if (c >= '0' && c <= '7')
			return 1;
		break;
	case 16:
		c |= 'a' - 'A';   /* Convert to upper case */
		if (c >= 'a' && c <= 'f')
			return 1;
		/* fall through */
	case 10:
		if (c >= '0' && c <= '9')
			return 1;
	}
	return 0;
}

