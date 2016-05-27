/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)key.c	1.1.1.1	(11/4/87)";

#include <dmd.h>

#define NKEYCOL	5
#define NKEYROW	7
#define NKEY	(NKEYCOL * NKEYROW)

Point	KeyOrigin;
short	KeyWidth;
short	KeyHight;

struct key {
	char	*name;
	int	value;
	Rectangle  rect;
} Key[NKEY] = {
	0,	0,	{{0, 0}, {0, 0}},
	0,	0,	{{0, 0}, {0, 0}},
	"C",	'@',	{{0, 0}, {0, 0}},
	"SV",	's',	{{0, 0}, {0, 0}},
	"EX",	'q',	{{0, 0}, {0, 0}},
	"&",	'&',	{{0, 0}, {0, 0}},
	"D",	'D',	{{0, 0}, {0, 0}},
	"E",	'E',	{{0, 0}, {0, 0}},
	"F",	'F',	{{0, 0}, {0, 0}},
	"+",	'+',	{{0, 0}, {0, 0}},
	"|",	'|',	{{0, 0}, {0, 0}},
	"A",	'A',	{{0, 0}, {0, 0}},
	"B",	'B',	{{0, 0}, {0, 0}},
	"C",	'C',	{{0, 0}, {0, 0}},
	"-",	'-',	{{0, 0}, {0, 0}},
	"<<",	'<',	{{0, 0}, {0, 0}},
	"7",	'7',	{{0, 0}, {0, 0}},
	"8",	'8',	{{0, 0}, {0, 0}},
	"9",	'9',	{{0, 0}, {0, 0}},
	"/",	'/',	{{0, 0}, {0, 0}},
	">>",	'>',	{{0, 0}, {0, 0}},
	"4",	'4',	{{0, 0}, {0, 0}},
	"5",	'5',	{{0, 0}, {0, 0}},
	"6",	'6',	{{0, 0}, {0, 0}},
	"*",	'*',	{{0, 0}, {0, 0}},
	"^",	'^',	{{0, 0}, {0, 0}},
	"1",	'1',	{{0, 0}, {0, 0}},
	"2",	'2',	{{0, 0}, {0, 0}},
	"3",	'3',	{{0, 0}, {0, 0}},
	"%",	'%',	{{0, 0}, {0, 0}},
	"~",	'~',	{{0, 0}, {0, 0}},
	"CE",	'#',	{{0, 0}, {0, 0}},
	"0",	'0',	{{0, 0}, {0, 0}},
	0,	0,	{{0, 0}, {0, 0}},
	"CR",	'\n',	{{0, 0}, {0, 0}},
};

initkey()
{
	int col, row;
	register struct key *p;

	KeyOrigin.x = XMAX/10;
	KeyOrigin.y = YMAX/5;
	KeyWidth = XMAX/3 / NKEYCOL;
	KeyHight = 7*YMAX/10 / NKEYROW;

	for (row = 0; row < NKEYROW; row++)
	 for (col = 0; col < NKEYCOL; col++) {
		p = &Key[row * NKEYCOL + col];
		p->rect.origin.x = KeyOrigin.x + col * KeyWidth;
		p->rect.origin.y = KeyOrigin.y + row * KeyHight;
		p->rect.corner.x = p->rect.origin.x + KeyWidth ;
		p->rect.corner.y = p->rect.origin.y + KeyHight;
		p->rect.origin.x += KeyWidth/10;
		p->rect.origin.y += KeyHight/5;
		p->rect.corner.x -= KeyWidth/10;
		p->rect.corner.y -= KeyHight/5;
		jrectf(p->rect, F_XOR);
		if (p->name) {
			Point pt;

			pt = p->rect.origin;
			if (p->name[1])
				pt.x += KeyWidth/7;
			else
				pt.x += KeyWidth/5;
			pt.y += KeyHight/6;
			jmoveto(pt);
			jstring(p->name);
		}
	}

}

getmouse()
{
	register struct key *p;

	if (bttn2()) {
		getoption();
		return -1;
	}
	for (p = Key; p < &Key[NKEY]; p++)
		if (ptinrect(mouse.jxy, p->rect)) {
			jrectf(p->rect, F_XOR);
			while (bttn123())
				sleep(1);
			jrectf(p->rect, F_XOR);
			return p->value;
		}
	return -1;
}

char *OptionText[] = {
	"hex",
	"dec",
	"oct",
	0
};

Menu OptionMenu = { OptionText };

getoption()
{
	switch (menuhit(&OptionMenu, 2)) {
	default:
		return;
	case 0:
		changebase(16);
		break;
	case 1:
		changebase(10);
		break;
	case 2:
		changebase(8);
		break;
	}
}
