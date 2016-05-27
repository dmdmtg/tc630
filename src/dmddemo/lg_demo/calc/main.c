/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)main.c	1.1.1.1	(11/4/87)";

#include <dmd.h>
#ifndef DMD630
#define reshape
#include <pandora.h>
#endif


extern int Error;	/* Indicates ERROR displayed in display window. */

int	opadd(), opsub(), opmult(), opdiv(), oprem();
int	opclr(), opsave(), openter(), opclrent();
int	opor(),	opand(), opxor(), opcomp();
int	oplshft(), oprshft(), opexit();
long	getdisplay(), pop();

#define NOP	(sizeof(Op)/sizeof(struct op))

struct op {
	char	name;
	int	(*func)();
} Op[] = {
	'+',	opadd,
	'&',	opand,
	'|',	opor,
	'^',	opxor,
	'~',	opcomp,
	'-',	opsub,
	'*',	opmult,
	'/',	opdiv,
	'%',	oprem,
	'<',	oplshft,
	'>',	oprshft,
	'@',	opclr,
	's',	opsave,
	'\n',	openter,
	'#',	opclrent,
	'\b',	opclrent,
	'q',	opexit,
};

int	DidOp;	/* Flag, set if last key did an operation */
		/* Used to generate automatic enter */


long CalcIcon[] = {
	0x00000000,
	0x00000000,
	0x03FFFF00,
	0x06000180,
	0x04F71880,
	0x0414A480,
	0x04770880,
	0x04149080,
	0x04F73C80,
	0x06000180,
	0x07FFFF80,
	0x07FFFF80,
	0x06000180,
	0x04000080,
	0x04DB6C80,
	0x04DB6C80,
	0x04000080,
	0x04000080,
	0x04DB6C80,
	0x04DB6C80,
	0x04000080,
	0x04000080,
	0x04DB6C80,
	0x04DB6C80,
	0x04000080,
	0x04000080,
	0x04DB6C80,
	0x04DB6C80,
	0x06000180,
	0x03FFFF00,
	0x00000000,
	0x00000000
};

Bitmap Calc = {
  (Word *)CalcIcon,
  sizeof(long)/sizeof(Word),
  0,0,32,32,
  0
};

main()
{
	register int c;

#ifdef DMD630
	local();
#endif
	init();

	for (;;) {
		c = getkey();
		if (isdigit(c))	{
			if (!Error) {
				if (DidOp) {
					openter();
					DidOp = 0;
				}
				enterdisplay(c);
			}
		} else {
			register int i;

			if (Error && c!='@' && c!='#' &&
			    c!='\b' && c!='s' && c!='q') 
				continue;
			for (i = 0; i < NOP; i++) {
				if (c == Op[i].name) {
					(*Op[i].func)();
					break;
				}
			}
		}
	}
}

init()
{
	request(KBD|MOUSE|RCV);
	initcell();
	initkey();
}

isop(c)
char c;
{
	register int i;

	for (i = 0; i < NOP; i++)
		if (c == Op[i].name)
			return 1;
	return 0;
}

opadd()
{
	putdisplay(pop() + getdisplay());
	DidOp = 1;
}

opsub()
{
	putdisplay(pop() - getdisplay());
	DidOp = 1;
}

opmult()
{
	putdisplay(pop() * getdisplay());
	DidOp = 1;
}

opdiv()
{
	long a, b;

	a = pop();
	b = getdisplay();
	if (b == 0)
		displayerror ();
	else
		putdisplay(a / b);
	DidOp = 1;
}

oprem()
{
	long a, b;

	a = pop ();
	b = getdisplay();
	if (b == 0) 
		displayerror ();
	else
		putdisplay(a % b);
	DidOp = 1;
}

opor()
{
	putdisplay(pop() | getdisplay());
	DidOp = 1;
}

opand()
{
	putdisplay(pop() & getdisplay());
	DidOp = 1;
}

opxor()
{
	putdisplay(pop() ^ getdisplay());
	DidOp = 1;
}

opcomp()
{
	putdisplay(~getdisplay());
	DidOp = 1;
}

oplshft()
{
	putdisplay(pop() << getdisplay());
	DidOp = 1;
}

oprshft()
{
	putdisplay(((unsigned long)pop()) >> getdisplay());
	DidOp = 1;
}

opclr()
{
	while(!isempty())
		(void) pop();
	opclrent();
}

opclrent()
{
	if (Error) 
		displayerror ();
	else
		putdisplay(0L);
	DidOp = 0;
	Error = 0;
}

opsave()
{
	Rectangle r;

	r = inset (P->rect, -2);
#ifdef DMD630
	reshape(Rect(8, 8, 48, 48));
#else
	reshape(P->layer, Rect(8, 8, 48, 48));
#endif
	bitblt(&Calc, Calc.rect, P->layer, 
		add(P->layer->rect.origin, Pt(7, 6)), F_XOR);
	
	while (own() & MOUSE) {
		if (bttn1()) {
			while (bttn1());
			break;
		}
		sleep(3);
	}
	sleep(3);
	while (!(own() & MOUSE))
		sleep (3);
#ifdef DMD630
	reshape(r);
#else
	reshape(P->layer, r);
#endif
}

opexit()
{
	exit(0);
}

openter()
{
	push(getdisplay());
	putdisplay(0L);
	DidOp = 0;
}

short	PushedBack;
char	PushedKey;

getkey()
{
	if (PushedBack) {
		PushedBack = 0;
		return PushedKey;
	}

	for (;;) {
		int c, r;

		do {
			if (P->state & RESHAPED) {
				init();
				P->state &= ~(RESHAPED|MOVED);
			}
		} while ((r = wait(KBD|MOUSE)) == MOUSE && !bttn123());

		if (r & MOUSE && bttn123()) {
			c = getmouse();
			if (c < 0)
				continue;
			return c;
		} else if (r & KBD) {
			c = kbdchar();

			switch (c) {
			case '\t':	
			case '\r':
			case ' ':
				c = '\n';
			}
			return c;
		}
	}
}

ungetkey(c)
char c;
{
	PushedKey = c;
	PushedBack = 1;
}

jputchar(c)
char c;
{
	char buf[2];

	buf[0] = c;
	buf[1] = '\0';
	jstring(buf);
}

