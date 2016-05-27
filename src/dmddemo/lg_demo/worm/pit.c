/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)pit.c	1.1.1.1	(10/7/87)";


/* pit.c	*/

#include <dmd.h>
#include <font.h>
#include "but.h"
extern	Texture16 dkgraymap;
#ifndef DMD630
#undef texture
#define texture texture16
#endif

#define	MAXX	52
#define MAXY	52
#define	SCALE	90
#define UNIT	8

extern	int	best	;
extern	int	showoff	;
extern	int	COLMS	;	/* horizontal UNITS in field	*/
extern	int	LINES	;	/* vertical UNITS in field	*/
extern	int	XMAR	;	/* horizontal pels beside field	*/
extern	int	YMAR	;	/* vertical pels over field	*/
extern	int	TCREEP	;	/* delay between auto-moves	*/
extern	int	TFWD	;	/* delay between forward moves	*/
extern	int	TTURN	;	/* delay between L,R moves	*/
extern	char	stw	[];
extern	Point	rtv	;
extern	Button	*bu	[];
extern	Button	bbu	[];
extern	Rectangle rbox	;	/* snake pit			*/
extern	Rectangle rmarl	;	/* left margin			*/
extern	Rectangle rmarr	;	/* right margin			*/
extern	Rectangle r1234	;

Rectangle rdel	= {
	0,0,8,24};
Bitmap	bdel	;
Bitmap	bbut2	;
Bitmap	bbut13	;
Word	wdel	[24] = {
	0x1800, 0x3C00, 0x6600, 0xC300, 0x8100, 0x8100, 0x1800, 0x3C00,
	    0x6600, 0xC300, 0x8100, 0x8100, 0x1800, 0x3C00, 0x6600, 0xC300,
	    0xA500, 0x8100, 0xC300, 0x5A00, 0x5A00, 0x6600, 0x6600, 0x5A00,
};
Word	but2	[16] = {
	0x0180, 0x03C0, 0x07E0, 0x0FF0,
	    0x03C0, 0x03C0, 0x03C0, 0x03C0,
	    0x03C0, 0x03C0, 0x0000, 0x0000,
	    0x7BDE, 0x4BD2, 0x4BD2, 0x7BDE,
};
Word	but13	[16] = {
	0x1008, 0x300C, 0x7C3E, 0xFE7F,
	    0x7FFE, 0x37EC, 0x13C8, 0x03C0,
	    0x03C0, 0x03C0, 0x0000, 0x0000,
	    0x7BDE, 0x7A5E, 0x7A5E, 0x7BDE,
};

/* Board layout:
 *	0 WORM	+++++++message area++++++	
 *	1 30x40	+			+
 *	2 Score	+			+
 *	3 9999	+			+	  C
 *	4 Best	+			+	  R   T
 *	5 12500	+			+	  E F U
 *		+			+	  E W R
 *		+			+	  P D N
 *	6 Quit	+			+	  8 9 10
 *		+++++++++++++++++++++++++	7 Speed
 */
snakepit ()
{
	register short Dw, Dh, xsp;
	    Point	rc;

	    rectf (&display, Drect, F_CLR);
	    Dw = Drect.corner.x - Drect.origin.x;
	    Dh = Drect.corner.y - Drect.origin.y;
	    if (Dw > (MAXX*UNIT) + 98)	/* room for all		*/
	    COLMS = MAXX;
	    else if (Dw > (3*UNIT) + 98)	/* room for margins	*/
	    COLMS = (Dw - 98)/UNIT;
	    else	/* no room 		*/
		COLMS = (Dw - 4)/UNIT;
		    XMAR = (Dw - (COLMS*UNIT))>>1;
		    if (Dh > (MAXY*UNIT) + 4)
		    LINES = MAXY;
		    else
			LINES = (Dh - 4)/UNIT;
			    YMAR = (Dh - (LINES*UNIT))>>1;
			    box ();
			    setrtv (LINES, 0);
			    rbox.origin.x = rtv.x;
			    rbox.origin.y = Drect.origin.y;
			    rmarl.origin = Drect.origin;
			    rmarl.corner.x = rtv.x;
			    rmarl.corner.y = Drect.corner.y;
			    texture (&display, rmarl, &dkgraymap, F_XOR);
			    setrtv (0, COLMS);
			    rbox.corner.x = rtv.x;
			    rbox.corner.y = Drect.corner.y;
			    rmarr.origin.x = rtv.x;
			    rmarr.origin.y = Drect.origin.y;
			    rmarr.corner = Drect.corner;
			    texture (&display, rmarr, &dkgraymap, F_XOR);

			    xsp = min (Dh/32, 8);
			    bu [0] = newButton (" WORM ", Pt (centerx (rmarl),
			    Drect.origin.y+(4*xsp)), "c", bbu);
			    showbu (bu [0]);
			    if (showoff) {
				bu [0]->flags |= buselected;
				    shade (bu [0], bublack); 			}
	sprintf (stw, "%dx%d", LINES-2, COLMS-2);
	    bu [1] = newButton (stw, Pt (centerx (rmarl),
	    Drect.origin.y+(8*xsp)), "c", bbu+1);
	    bu [1]->flags |= bunugae;
	    showbu (bu [1]);
	    bu [2] = newButton ("Score", Pt (centerx (rmarl),
	    Drect.origin.y+(13*xsp)), "c", bbu+2);
	    bu [2]->flags |= bunugae;
	    showbu (bu [2]);
	    bu [3] = newButton ("    ", Pt (centerx (rmarl),
	    Drect.origin.y+(17*xsp)), "c", bbu+3);
	    bu [3]->flags |= bunugae;
	    showbu (bu [3]);
	    bu [4] = newButton ("Best", Pt (centerx (rmarl),
	    Drect.origin.y+(22*xsp)), "c", bbu+4);
	    bu [4]->flags |= bunugae;
	    showbu (bu [4]);
	    sprintf (stw, "%d", best);
	    bu [5] = newButton (stw, Pt (centerx (rmarl),
	    Drect.origin.y+(26*xsp)), "c", bbu+5);
	    showbu (bu [5]);
	    bu [6] = newButton ("Quit", Pt (centerx (rmarl),
	    Drect.corner.y-14), "c", bbu+6);
	    showbu (bu [6]);

	    xsp = min ((XMAR-8)/3, 20);
	    bu [7] = newButton ("Speed", Pt (centerx (rmarr),
	    Drect.corner.y-14), "c", bbu+7);
	    bu [7]->flags |= bunugae;
	    showbu (bu [7]);
	    bu [8] = newButWH ("", Pt (centerx (rmarr)-xsp-4,
	    centery (rmarr)), "c", xsp, Dh-60, bbu+8);
	    bu [8]->icon [0] = &bdel;
	    bdel.base = wdel;
	    bdel.width = (unsigned)1;
	    bdel.rect = rdel;
	    showbu (bu [8]);
	    rc.x = bu [8]->rect.origin.x;
	    rc.y = bu [8]->rect.corner.y - muldiv (Dh-60, TCREEP, SCALE);
	    rectf (&display, Rpt (rc, bu [8]->rect.corner), F_XOR);
	    bu [9] = newButWH ("", Pt (centerx (rmarr),
	    centery (rmarr)), "c", xsp, Dh-60, bbu+9);
	    bu [9]->icon [0] = &bbut2;
	    bbut2.base = but2;
	    bbut2.width = (unsigned)1;
	    bbut2.rect = r1234;
	    showbu (bu [9]);
	    rc.x = bu [9]->rect.origin.x;
	    rc.y = bu [9]->rect.corner.y - muldiv (Dh-60, TFWD, SCALE);
	    rectf (&display, Rpt (rc, bu [9]->rect.corner), F_XOR);
	    bu [10] = newButWH ("", Pt (centerx (rmarr)+xsp+4,
	    centery (rmarr)), "c", xsp, Dh-60, bbu+10);
	    bu [10]->icon [0] = &bbut13;
	    bbut13.base = but13;
	    bbut13.width = (unsigned)1;
	    bbut13.rect = r1234;
	    showbu (bu [10]);
	    rc.x = bu [10]->rect.origin.x;
	    rc.y = bu [10]->rect.corner.y - muldiv (Dh-60, TTURN, SCALE);
	    rectf (&display, Rpt (rc, bu [10]->rect.corner), F_XOR);
}

program (this)
short this;
{
	short	setting;
	    Rectangle rm;

	    if (this==8) setting = TCREEP;
	    else if (this==9) setting = TFWD;
	    else if (this==10) setting = TTURN;
	    rm = bu [this]->rect;
	    rm.origin.y = bu [this]->rect.corner.y
	    - muldiv (setting, height (bu [this]->rect), SCALE);
	    while (!bttn123 ()) {
		rectf (&display, rm, F_XOR);	/* erase	*/
		rm.origin.y = mouse.xy.y;
		    rectf (&display, rm, F_XOR);	/* shade	*/
	}
	bu [this]->flags &= ~buselected;
	    shade (bu [this], buwhite);
	    shade (bu [this], bultgray);	/* has (had) mouse	*/
	setting = muldiv (height (rm),
	    SCALE, height (bu [this]->rect));
	    if (setting>SCALE) setting = SCALE;
	    if (setting<0) setting = 0;
	    if (this==8) TCREEP = setting;
	    else if (this==9) TFWD = setting;
	    else if (this==10) TTURN = setting;
}
