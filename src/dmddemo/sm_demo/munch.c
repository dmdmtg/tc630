/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)munch.c	1.1.1.2	(11/4/87)";

/*
 * munching squares
 */
#include <dmd.h>
#define	DELTA 13
#define	START	0x700000
long bit[]={
	0x80000000,	0x40000000,	0x20000000,	0x10000000, 
	0x8000000,	0x4000000,	0x2000000,	0x1000000,
	0x800000,	0x400000,	0x200000,	0x100000,
	0x80000,	0x40000,	0x20000,	0x10000,
	0x8000,		0x4000,		0x2000,		0x1000,
	0x800,		0x400,		0x200,		0x100,
	0x80,		0x40,		0x20,		0x10,
	0x8,		0x4,		0x2,		0x1
};

main()
{
    register unsigned x, y;
    register long i;
    register long d;
    Bitmap b;

#ifdef DMD630
    local();
#else
    jinit();
    cursinhibit();
#endif
    d=0;
#ifdef DMD630
    b.rect = fRect(0,0,32,1);
#else
    b.rect.origin.x = 0;
    b.rect.origin.y = 0;
    b.rect.corner.x = 32;
    b.rect.corner.y = 1;
#endif
    b.width = sizeof(long)/sizeof(Word);
    b._null = (char *)0;
    request(KBD);
    while (kbdchar() != 'q') {
    	for(x=Drect.origin.x; x<Drect.corner.x; x++){
    	    y = (x^d) & 1023;
	    if(y>=Drect.origin.y && y<Drect.corner.y) {
		b.base = (Word *)&bit[x&0x1F];
		bitblt(&b, b.rect, &display, Pt(x&~0x1F,y), F_XOR);
	    }
    	}
    	d+=DELTA;
	wait(CPU);
#ifdef DMD630
	if(P->state&RESHAPED) {
	    rectf(&display, Drect, F_CLR);
	    d = 0;
	    P->state &= ~RESHAPED;
	}
#endif
    }
}
