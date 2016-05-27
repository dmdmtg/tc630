/*	Copyright (c) 1987 AT&T	*/
/*	All Rights Reserved	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


static char _2Vsccsid[]="@(#)slave.c	1.1.1.12 90/06/21 13:31:47";

/* includes */
#include <dmd.h>
#ifdef DMD630
#	include <5620.h>
#	include <object.h>
#endif /* DMD630 */
#include "font.h"
#include "../include/comm.h"
#ifdef lint
#	include <menu.h>
#	include <lintdefs.h>
#endif /* lint */

/* defines */
#define True 1
#define False 0
#undef CURSOR
#define CURSOR '\01'
#ifdef	MPX
#	undef	cursinhibit
#	undef	cursallow
#	define	cursinhibit()	{}
#	define	cursallow()	{}
#else	MPX
#	undef	transform
#	define transform(p)	p
#endif	/* MPX */
#undef infont
#define	NEWLINESIZE	16
#define	LINEBUFSIZE 100
#define Options "cp"
#define Yes 1
#define No 0
#ifdef DMD630
#	define SpacePtr ((space *)P->appl)
#else
#	define SpacePtr ((space *)spacePtr)
#endif
#define MAXPAGE 0x7fff		/* no sign-bit, just to be sure */

/* types */
struct line {
	char buf[LINEBUFSIZE];
	char *bufp;
};
typedef struct space {
	Font *curfont;
#	define Curfont (SpacePtr->curfont)
	Rectangle frame;
#	define Frame (SpacePtr->frame)
	Point fudge;			/* {5, 3} */
#	define Fudge (SpacePtr->fudge)
	int dotroff;			/* 1 */
#	define Dotroff (SpacePtr->dotroff)
	int cursvis;			/* 0 */
#	define Cursvis (SpacePtr->cursvis)
	Point org;		/* current origin (screen coordinates) */
#	define Org (SpacePtr->org)
	int scaled;				/* 1 */
#	define Scaled (SpacePtr->scaled)
	struct ftab {
		char name[10];
		Font *font;
	} ftab[NFONTS];
#	define Ftab (SpacePtr->ftab)
	Menu windowMenu;		/* window5620items, window630items */
#	define WindowMenu (SpacePtr->windowMenu)
	Menu scaledMenu;		/* scaled5620items, scaled630items */
#	define ScaledMenu (SpacePtr->scaledMenu)
#	ifdef DMD630
		short proofterm;		/* False */
#		define Proofterm (SpacePtr->proofterm)
		short cacheFont;		/* True */
#		define CacheFont (SpacePtr->cacheFont)
#	endif /* DMD630 */
	Point pts[60];
#	define Pts (SpacePtr->pts)
	Point curpt;
#	define Curpt (SpacePtr->curpt)
} space;

/* globals */

/* locals */
#ifndef DMD630
	/*
	 * Any global variables installed in other files (e.g.
	 * infont.c) must be installed in space.  This will
	 * necessitate putting space in an include file.  It
	 * will also necessitate putting the definition of
	 * spacePtr for the 5620 in a header file.
	 */
	long spacePtr;
#endif

/* read-only locals */
static Texture16 cup = {
	0x0100, 0x00E0, 0x0010, 0x03E0, 0x0400, 0x0FE0, 0x123C, 0x1FE2,
	0x101A, 0x101A, 0x1002, 0x103C, 0x1810, 0x6FEC, 0x4004, 0x3FF8
};
static Texture16 prompt = {
	0x0000, 0x0000, 0x0000, 0x322E, 0x4B69, 0x4369, 0x42A9, 0x42A9,
	0x42A9, 0x4229, 0x4229, 0x4229, 0x322E, 0x0000, 0x0000, 0x0000
};
static Texture16 readyIcon = {
	0x0780, 0x0780, 0x0780, 0x0780, 0x0780, 0x0780, 0x0780, 0xC78E,
	0xE01C, 0x7038, 0x3870, 0x1FE0, 0x0FC0, 0x0780, 0x0300, 0xFFFE,
};
static Texture16 ok = {
	0x1C44, 0x2248, 0x2250, 0x2270, 0x2248, 0x1C44, 0x0000, 0x0380,
	0x0440, 0x0440, 0x0080, 0x0100, 0x0100, 0x0100, 0x0000, 0x0100,
};

/*
 * proofterm menus (needed for the 630 as well as the 5620)
 */
static char *scaled5620items[] = {
	"next",
	"page",
	"scale",
	"quit",
	"exit",
	NULL
};
static char *window5620items[] = {
	"next",
	"page",
	"window",
	"quit",
	"exit",
	NULL
};

/*
 * cache menus (only needed for the 630)
 */
#ifdef DMD630
	static char *window630items[] = {
		"next",
		"page",
		"window",
		"exit",
		NULL
	};
	static char *scaled630items[] = {
		"next",
		"page",
		"scale",
		"exit",
		NULL
	};
#endif
static int eightspaces=72;

/* procedures */
extern Font *fontrequest ();
extern void ffree ();
extern char *strcpy ();
extern char *strcat ();
extern int strcmp ();
extern void arc ();
extern void ellipse ();
extern void circle ();
#ifdef DMD630
	extern void exit ();
	extern void sleep ();
	extern Font *infont();
	extern Texture16 *cursswitch ();
	extern int kbdchar ();
	extern int own ();
	extern int wait ();
	extern int strlen ();
	extern int rcvchar ();
	extern Point string ();
	extern int menuhit ();
	extern int rectXrect ();
	extern Point add ();
	extern Point sub ();
	extern void segment ();
	extern void rectf ();
	extern void sendnchars ();
	extern void bitblt ();
#endif /* DMD630 */


main(argc, argv)

int argc;
char **argv;
{
	struct line line;
	Point curpos;
	short c;
	short optind;


#	ifdef DMD630
		P->appl = (long)alloc (sizeof (struct space));
#	else
		spacePtr = (long)alloc (sizeof (struct space));
#	endif
	initspace ();

	/*
	 * getopt() couldn't be practically used because it depends
	 * on static variables getting initialized: that not only
	 * causes a problem for shared text, but is difficult with
	 * caching.  The getopt internal arg cursor might not always
	 * be reset, for example.  This approach is cheaper an better,
	 * but it does not allow clumped arguments.
	 */
	for (optind = 1; optind < argc && *argv[optind] == '-'; optind++)
	    switch (*(argv[optind]+1)) {
	    case 'c':
#		ifdef DMD630
		    cache ((char *)0, A_NO_SHOW | A_SHARED);
#		endif /* DMD630 */
		break;
	    case 'p':
#		ifdef DMD630
		    Proofterm = True;
		    WindowMenu.item = window5620items;
		    ScaledMenu.item = scaled5620items;
#		endif /* DMD630 */
		break;
	    }

	request(RCV|SEND|KBD|MOUSE);

	/* use some distinctive icon */
	(void)cursswitch (&readyIcon);

	/* initialize the default font */
	(void)strcpy(Ftab[0].name, "defont");
	Ftab[0].font = &defont;

	/*
	 * wait for char - send NAK
	 *	wait for REQ-NAK protocol before sending window
	 *	host always sends three but we catch the last two in troff
	 *
	 * To put it another way: the host-resident troff sends
	 * either 3 or 2 ACK/NAK cycles, depending on whether it
	 * just down loaded proof or not:  Thus everytime troff
	 * is started it begins by sending two cycles - except
	 * when proof was just downloaded, in which case proof sends
	 * an additional one.
	 */
	(void)inchar();
	send(NAK);
	send('\n');

  restart:

#ifdef	MPX
	P->state&=~RESHAPED|MOVED;
#else
	Drect = inset(Drect, 2);
#endif	/* MPX */

	/*
	 * this is always the coordinates of the window.
	 * Set here, and after a move.
	 */
	Org=Drect.origin;

	/* use Drect until the FRAME command is received. */
	Frame=Drect;

	/*
	 * set screen origin
	 * Apparently, it is the upperleft-most scan line of the cmd
	 * enter area.  It must be presumed that proof starts up in
	 * proofterm.
	 */
	curpos.x = Org.x + Fudge.x;
	curpos.y = Drect.corner.y - Fudge.y - defont.height;

	/* initialize line buffer */
	line.bufp=line.buf;

	if(Cursvis)
		term(CURSOR, 0, &line, &curpos);	/* flip state */

	for(;;){
		(void)wait(KBD|RCV);
#ifdef	MPX
		if((P->state&(RESHAPED|MOVED))==RESHAPED)
		{
			if(Dotroff)
				/* send new window dimensions */
				sendwindow();

			goto restart;
		}

		if(P->state&MOVED){
			P->state&=~(MOVED|RESHAPED);

			/*
			 * take the difference between the curpos and
			 * the last origin, and add it to the current
			 * window origin.  Reset the local origin var.
			 */
			curpos=add(sub(curpos, Org), Drect.origin);
			Org=Drect.origin;
		}
#endif	/* MPX */
		c = inchar();
		if(Dotroff)
			troff(c);
		else {
			if(Cursvis)
				/* undraw cursor */
				term(CURSOR, 0, &line, &curpos);

			term(c, 1, &line, &curpos);

			while(own()&RCV)
				term(inchar(), 1, &line, &curpos);

			/* draw at new spot */
			term(CURSOR, 0, &line, &curpos);

			Cursvis = True;
		}
	}
}


initspace ()
{
	Fudge.x = 5;
	Fudge.y = 3;
	Dotroff = True;
	Cursvis = False;
	Scaled = True;
#	ifdef DMD630
		WindowMenu.item = window630items;
		ScaledMenu.item = scaled630items;

		Proofterm = False;
		CacheFont = True;
#	else
		WindowMenu.item = window5620items;
		ScaledMenu.item = scaled5620items;
#	endif
}


/* send window size dimensions */
sendwindow()
{
	register c;

	send(WIND);

	/* send 16 bits of window width in pixels */
	c = Drect.corner.x - Drect.origin.x;
	send(c >> 8);
	send(c);

	/* send 16 bits of window height in pixels */
	c = Drect.corner.y - Drect.origin.y;
	send(c >> 8);
	send(c);
}


term(c, advance, linep, pp)
	int c;
	int advance;
	register struct line *linep;
	register Point *pp;
{
	register Fontchar *fp;
	Rectangle r;
	Point p;

	if(c == 0x80)
		c = 0x7f;

	switch(c&=0x7F){
	default:
		/* get font info for this char */
		fp=defont.info+c;

		/* scroll, if necessary */
		if(fp->width+pp->x >= Drect.corner.x)
			newline(linep, pp);

		p = *pp;

		/* define the rectangle in defont to copy from */
		r.origin.x=fp->x;
		r.corner.x=(fp+1)->x;
		r.origin.y=fp->top;
		r.corner.y=fp->bottom;

		/* put the top of the char at the top of the line */
		p.y+=fp->top;

		bitblt(defont.bits, r, &display, p, F_XOR);

		/* enter c into line, if requested to */
		if(advance){
			pp->x+=fp->width;
			/*
			 * insert the char into the line buffer only
			 * if there's enough room.  What happens if
			 * there's not?  pp has already been incremented.
			 */
			if(linep->bufp < linep->buf+LINEBUFSIZE)
				*linep->bufp++=c;
		}
		break;
	case REQ:
		/* send NAK */
		send(NAK);
		send('\n');
		Dotroff = True;
		request(SEND|RCV|KBD|MOUSE);
		break;
	case '\n':
		newline(linep, pp);
		break;
	case '\7':
/*
 * fix this later!
		*((char *)(384*1024L+062)) = 2;	/* beep */
		break;
	case '\r':
		pp->x=Drect.origin.x+5;
		break;
	case '\013':	/* ^K: reverse linefeed */
		if(pp->y>Drect.origin.y+5)
			pp->y-=NEWLINESIZE;
		break;
	case '\b':
		backspace(linep, pp);
		break;
	case '\014':
		formfeed(linep, pp);
		break;
	case '\t':
		pp->x=nexttab(pp->x);
		if(pp->x>=Drect.corner.x)
			newline(linep, pp);
		if(linep->bufp<linep->buf+LINEBUFSIZE)
			*linep->bufp++=c;
		break;
	}
}


nexttab(x)
{
	/*
	 * eightspaces is not changed anywhere, so it will not
	 * not effect this program's eligibility to be re-entrant.
	 * Its definition has been moved up to the other data
	 * definitions.
	 * The following definition was a comment that preceded
	 * the real definition of eightspaces:
	 * int eightspaces=8*dispatch[' '].c_wid;
	 */

	register xx=x-Drect.origin.x-5;
	return(xx-(xx%eightspaces)+eightspaces+Drect.origin.x+5);
}


backspace(linep, pp)
	register struct line *linep;
	register Point *pp;
{
	register char *p;
	register x=Drect.origin.x+5;
	if(linep->bufp>linep->buf){
		for(p=linep->buf; p<linep->bufp-1; p++)
			if(*p=='\t')
				x=nexttab(x);
			else
				x+=defont.info[*p].width;
		pp->x=x;
		--linep->bufp;
		if(*p!='\t')
			term(*p, 0, linep, pp);
	}
}


newline(linep, pp)
	struct line *linep;
	register Point *pp;
{
	register cursoff=0;
	if(pp->y+2*NEWLINESIZE > Drect.corner.y-2){
		/* weirdness is because the tail of the arrow may be anywhere */
		if(rectXrect(Rect(mouse.xy.x-16, mouse.xy.y-16, mouse.xy.x+16,
				mouse.xy.y+16), Drect)){
			cursinhibit();
			cursoff++;
		}
		lscroll();
		if(cursoff)
			cursallow();
	}else
		pp->y+=NEWLINESIZE;
	pp->x=Drect.origin.x+Fudge.x;
	linep->bufp=linep->buf;
}


lscroll()
{
	Rectangle r;

	r = Drect;
	r.origin.y += NEWLINESIZE;
	bitblt(&display, r, &display, Pt(r.origin.x, r.origin.y-NEWLINESIZE), F_STORE);
	stipple(Rpt(Pt(Drect.origin.x, Drect.corner.y-NEWLINESIZE), Drect.corner), No);
}


formfeed(linep, pp)
	struct line *linep;
	Point *pp;
{
	cursinhibit();
	stipple(Drect, No);
	cursallow();
	*pp=add(Drect.origin, Fudge);
	linep->bufp=linep->buf;
}


send(c)
{
	char cc = c;

	sendnchars(1, &cc);
}


stipple(r, flash)
	Rectangle r;
	int flash;
{
	cursinhibit();
	if (flash)
	    texture (&display, Drect, &T_checks, F_STORE);
	rectf(&display, r, F_CLR);
	cursallow();
}


/* read a char from the host process */
inchar()
{
	register c;

	while ((c = rcvchar()) == -1) {
		(void)wait(RCV);
			/*	send(kbdchar()&0177); */
	}
#ifndef MPX
	if(!Dotroff)
		c &= 0177;
#endif
	return(c&0377);
}


/* read in a short from the host process */
inshort()
{
	short i;
	register char *p = (char *) &i;

	*p++ = inchar();
	*p++ = inchar();
	return(i);
}


/*
 * read in from the host process the xy distance from the old
 * coordinates, and set the point p to the new screen coordinates.
 * The following discussion is only for us folks who don't
 * intimately know device independent troff output:
 * I guess the point here is that the host process doesn't know
 * where the window is, so it just sends distances, which are
 * kept track of here.  Wouldn't it be feasible for troff to send
 * window coordinates?  Probably not, because troff (the real thing)
 * wants to keep track of real distances, etc... no, that's not the
 * reason - the real reason is that troff doesn't know the size of the
 * devices real window: it can't work on the presumption of knowing
 * what XMAX and YMAX are, so instead, it works on the smallest unit -
 * move a pixel here, a pixel there.
 */
Point
inpoint()
{
	Point p;

	p.x = inshort()+Org.x;
	p.y = inshort()+Org.y;
	return(p);
}


Point
drawchar(p,c)
Point p;
{
	char s[2];

	s[0] = c; s[1] = 0;
	string(Curfont, s, &display, Pt(p.x, p.y-Curfont->ascent), F_OR);
	p.x += Curfont->info[c].width;
	return(p);
}


jspline(n)
int n;
{
	register i;
	Point *pp;

	for (i = 0, pp = &Pts[1]; i < n; i++)
		*pp++ = inpoint();
	spline(n+1, Pts, F_OR);
}


spline(n,pp,f)
register Point *pp;
int n, f;
{
	register long w, t1, t2, t3, scale=1000; 
	register int i, j, steps=10; 
	Point p,q;
    	if (pp != (Point *) NULL) {
		pp[0] = pp[1];
		pp[n] = pp[n-1];
		p = pp[0];
		for (i = 0; i < n-1; i++) {
			for (j = 0; j < steps; j++) {
				w = scale * j / steps;
				t1 = w * w / (2 * scale);
				w = w - scale/2;
				t2 = 3*scale/4 - w * w / scale;
				w = w - scale/2;
				t3 = w * w / (2*scale);
				q.x = (t1*pp[i+2].x + t2*pp[i+1].x + 
					t3*pp[i].x + scale/2) / scale;
				q.y = (t1*pp[i+2].y + t2*pp[i+1].y + 
					t3*pp[i].y + scale/2) / scale;
				segment(&display, p, q, f);
				p = q;
			}
		}
	}
}


Font *
ldfont(name)
	char *name;
{
	register struct ftab *f;
	register x;

	/*
	 * find the first unused entry in the mount table, or the
	 * font that's requested.
	 */
	for(f = Ftab; f->name[0]; f++)
		if(strcmp(f->name, name) == 0)
			break;

	/*
	 * at this point, we have either the desired font entry, or
	 * at least an available one (or the very last one).
	 */
	x = f - Ftab;

	/*
	 * if the whole (NFONTS) table is used up, then we've
	 * got to recycle some space.  Free the last entry.
	 * However, don't free cached entries.
	 */
	if(f == &Ftab[NFONTS]) {
		/* load over the top ... */
		f--;
#		ifdef DMD630
		    if (!CacheFont)
			ffree(f->font);
#		else
		    ffree(f->font);
#		endif /* DMD630 */
		f->name[0] = 0;
	}

	/*
	 * if the desired entry is present, reply "thanks anyway"
	 * and send its ftab entry index.  This is the same index
	 * that the host uses in *its* mount table.
	 */
	if(f->name[0] != 0) {
		send(ACK);
		send(x);
	}
	/*
	 * if that entry is empty (or recycled), give the high sign,
	 * and be prepared to suffer the consequences.
	 */
	else {
		/* put the new font name in the ftab */
		(void)strcpy(f->name, name);

#		ifdef DMD630
		    /*
		     * this font is not known to proof.  See if it's cached.
		     * if present, inform the term that we intend to use it.
		     */
		    if (CacheFont) {
			if ((f->font = fontrequest (name)) != 0) {
			    send(ACK);
			    send(x);

			    /* it seems we always return a ptr to the font */
			    return(f->font);
			}
		    }
#		endif /* DMD630 */

		/*
		 * if not cached, begin the download
		 */
		fontmsg(name);

		/* switch cursor to coffee cup (presumably) */
		(void)cursswitch(&cup);

		/* presumably begins the actual font download */
		f->font = infont(inchar, x);

		/*
		 * Just to see if run out of memeory
		 */
		if(f->font == 0){
			/*
			 * free up slot so that if more memory becomes
			 * available later, we won't be misled into
			 * believing that the font is already here.
			 * Notice the distinction from the case of
			 * missing fonts: in that case, we assume that
			 * they won't show up later, so we must dummy
			 * them out.
			 */
			f->name[0] = 0;
			/* pass the default font back to troff */
			f = Ftab;
			debmsg(name);
			sleep(60);
			debmsg(name);
		}
		if(f->font == (Font *)-1){
			/*
			 * what happens if Ebat is stored in the text area?
			 * There's presumably no access checks.  Is this
			 * truly self-modifying code?  From Bell Labs?
			(void)strcpy("Ebat",name);
			 * I'm going to assume that this may serve
			 * some useful function (although I can't
			 * force this code to happen), but that the
			 * arguments are reversed.
			 */
			(void)strcpy(name, "Ebat");
			debmsg(name);
			sleep(1000);
		}
		/*
		 * host *still* couldn't find font
		 */
		if(f->font == (Font *)-2)
			/*
			 * pass the default font back to troff().
			 * Because this builds an ftab entry, it
			 * should eliminate repeated calls to
			 * ldfont().
			 */
			f->font = &defont;

		/* switch cursor to default cursor (arrow?) */
		(void)cursswitch(&readyIcon);

		fontmsg(name);

#		ifdef DMD630
		    /* now that the font is loaded, share it */
		    if (CacheFont && (f->font != &defont))
			fontcache (f->name, f->font);
#		endif /* DMD630 */
	}
	return(f->font);
}


fontmsg(name)
	char *name;
{
	char buf[64];

	(void)strcpy(buf, "loading font ");
	(void)strcat(buf, name);
	string(&defont, buf, &display, add(Pt(3,3), Drect.origin), F_XOR);
}


debmsg(name)
	char *name;
{
	char buf[64];

	(void)strcpy(buf, " not returning font ");
	(void)strcat(buf, name);
	string(&defont, buf, &display, add(Pt(3,14), Drect.origin), F_XOR);
}


/* process one char from the host */
troff(c)
{
	int a;
	Point q;
	char fname[64], *f;

	/* apparently - just to make sure */
	if (Curfont == (Font *) NULL)
		Curfont = &defont;

	switch (c) {
	case EXIT:
		bye();
		break;
	case FRAME:
		getframe (True);
		break;
	case ASCII:
#		ifdef DMD630
		    if (!Proofterm)
			exit (0);
		    else {
			Cursvis = Dotroff = False;
			request(RCV|SEND);
		    }
#		else
		    Cursvis = Dotroff = False;
		    request(RCV|SEND);
#		endif /* DMD630 */
		break;
	case WINDOW:
	case SCALE:
		Scaled = c == SCALE;
		break;
	case CLEAR:
		stipple(Frame, Yes);
		break;
	case SLICE:
		pagecmd();
		break;
	case REQ:
		send(NAK);
		send('\n');
		sendwindow();
		break;
	case PAGE:
		pagecmd();
		break;
	case POS:
		Curpt = inpoint();
		break;
	case RIGHT:
		c = inchar();
		if(c&0200)	/* if the high order bit is on */
			c = 0xFF00 | c;	/* make it a neg. short */
		Curpt.x += c;
		break;
	case FONT:
		Curfont = Ftab[inchar()].font;
		break;
	case NEWFONT:
		for(f = fname; (*f = inchar()) != EOT; f++);
		*f = 0;		/* turn EOT to EOS */
		Curfont = ldfont(fname);
		break;
	case LINE:
		Curpt = inpoint();
		segment(&display, Curpt, inpoint(), F_OR);
		break;
	case CIRCLE:
		Curpt = inpoint();
		circle(&display, Curpt, inshort(), F_OR);
		break;
	case ELLIPSE:
		Curpt = inpoint();
		a = inshort();
		ellipse(&display, Curpt, a, inshort(), F_OR);
		break;
	case ARC:
		Curpt = inpoint();
		q = inpoint();
		arc(&display, Curpt, q, inpoint(), F_OR);
		break;
	case SPLINE:
		jspline(inchar());
		break;
	case '\n':
		Curpt.y += Curfont->height+3;
		if (Curpt.y > YMAX)
			Curpt.y = 0;
		break;
	case '\r':
		Curpt.x = 0;
		break;
	default:		/* print ascii chars */
		if (c >127)
			break;
		Curpt = drawchar(Curpt,c);
	}
}


minkbd()
{
	register w;
	Texture16 *t;

	/* wait until one of these resources is "ready for service" */
	while(w = wait(KBD|MOUSE|RCV)) {
		if(w&KBD)
			return(kbdchar());
		else if(w&RCV)
			return(rcvchar());
		else {
			/* if no button is depressed, false alarm */
			if(!button123())
				continue;

			/* if button 3 is depressed, handle it */
			if(button3()) {
				switch(w = menuhit(Scaled?
					&WindowMenu : &ScaledMenu, 3)) {
				case -1:	/* no selection made */
					break;
				case 4:
				    /*
				     * only happens on the 5620
				     *
				     * First display distinctive icon.
				     */
				    t = (Texture16 *)cursswitch(&ok);

				    /* 
				     * what until the user presses
				     * a button again.
				     */
				    while(!button123())
					sleep(1);

				    /* get the status of button3 */
				    w = button3();

				    /*
				     * before doing anything, wait
				     * till he lets go of the button
				     */
				    while(button123())
					sleep(1);

				    /* return to default cursor */
				    (void) cursswitch(t);

				    /*
				     * if button3 was depressed, exit.
				     * otherwise, return to some
				     * more benign mode.
				     */
				    if(w)
					return('x');
				    else
					continue;
				default:
				    return(Scaled? "\rpwq"[w] : "\rpsq"[w]);
				}	/* end of switch */
			}		/* end of button 3 depressed */
			/*
			 * Otherwise, wait till the user lets go.
			 */
			else
				while(button123())
					sleep(1);
		}
	}		/* while resource ready for service */

	/*NOTREACHED*/
}


/*
 * getframe can be called either as the result of a received
 * command - as processed by troff - or in a protocol dialog
 * with the host side - e.g. when conversing about the desired
 * end of page action (e.g. exit, quit, scale, etc.)
 *
 * command is boolean, indicating the former case.
 */
getframe (command)
	int command;
{
	if (!command)
		inchar ();
	Frame.origin = Org;
	Frame.corner.x = min (inshort () + Org.x, Drect.corner.x);
	Frame.corner.y = min (inshort () + Org.y, Drect.corner.y);
}


pagecmd()
{
	register Texture16 *oldcursor;
	char buf[40];
	register char *strcursor;
	int pagenum;

	/* set cursor to CMD.  Save ptr to old cursor in "oldcursor" */
	oldcursor = (Texture16 *)cursswitch(&prompt);
pgstart:
	switch(minkbd())
	{
	case 0177:
	case 04:
	case 'q':
		/* tell the host side to get lost */
		send(EXIT);

#		ifdef DMD630
		    if (!Proofterm)
			bye();
		    else {
			Cursvis = Dotroff = False;
			request(SEND|RCV);

			/* forget the old cursor: return to default */
			oldcursor = 0;
		    }
#		else
		    /* resort to proofterm */
		    Cursvis = Dotroff = False;
		    request(SEND|RCV);

		    /* forget the old cursor: return to default */
		    oldcursor = 0;
#		endif /* DMD630 */
		break;
	case 'x':
		send(EXIT);
		bye();
		break;
	case 's':
		Scaled = True;
		send(SCALE);
		getframe (False);
		stipple(Frame, Yes);
		break;
	case 'w':
		Scaled = False;
		send(WINDOW);
		getframe (False);
		stipple(Frame, Yes);
		break;
	/*
	 * plug up hole where we hang
	 *
	 */
	case 'p':
		(void)strcpy(buf, "Page: ");
		edit(buf);

		/* skip over "Page:" */
		for(strcursor = buf; *strcursor != ' '; strcursor++)
			/* empty */;

		/* skip over blanks */
		while(*strcursor == ' ')
			strcursor++;

		/* collect the page number */
		pagenum = 0;
		while((*strcursor >= '0') && (*strcursor <= '9'))
			pagenum = 10*pagenum + *strcursor++ - '0';

		/*
		 * special case page number processing.
		 * If the page count is 0 or non-numeric, use
		 * page num 1, unless the page count is '$',
		 * in which case we'll leave it at 0 (corny).
		 */
		if(pagenum == 0)
			if(*strcursor != '$')
				pagenum = 1;
		/*
		 * only one byte of count can be sent.
		 * I still don't understand why this is 255, though.
		 * Give it a shot with 2 bytes.
		 */
		if(pagenum > MAXPAGE)
			pagenum = MAXPAGE;

		send(PAGE);
		sendnchars (2, &pagenum);

		getframe (False);
		stipple(Frame, Yes);
		break;
	case '\r':
	case '\n':
		send(ACK);
		getframe (False);
		stipple(Frame, Yes);
		break;
	default:
	/*	ringbell(); */
		goto pgstart;
		/*NOTREACHED*/
		break;
	}

	/* restore saved (or default) cursor */
	(void)cursswitch (oldcursor);
}


bye()
{
	exit(0);
}


inkbd()
{
	(void)wait(RCV|KBD);
	return((own()&KBD)? kbdchar():rcvchar());
}


edit(s)
	char *s;
{
	char buf[40];
	Point p;
	int c;
	register i;
	short width;
	short x, y;

	(void)strcpy(buf, s);
	width = jstrwidth (buf);

	/* don't put prompt on cursor */
	p = add (mouse.xy, Pt (0, 10));

	/* make sure prompt is visible */
	x = Drect.corner.x - (width + p.x);
	y = Drect.corner.y - (defont.height + p.y);
	p = add (p, Pt (min (x, 0), min (y, 0)));

	/* display prompt */
	disp(buf, p);

	for(i = strlen(buf); (c = inkbd()) != '\r'; /* empty */) {
		/* erase old prompt, so new one will xor */
		disp(buf, p);

		switch(c) {
		case '\b':
			if((buf[i-1] != ' ') && (i > 0))
				buf[--i] = 0;
			break;
		/* kill line */
		case '@':
			while(buf[i] != ' ') i--;
			buf[++i] = 0;
			break;
		default:
			buf[i++] = c;
			buf[i] = 0;
			break;
		}

		/* display updated prompt */
		disp(buf, p);
	}

	/* erase final prompt */
	disp(buf, p);

	/* return operator request */
	(void)strcpy(s, buf);
}


disp(s, p)
	char *s;
	Point p;
{
	string(&defont, s, &display, p, F_XOR);
}
