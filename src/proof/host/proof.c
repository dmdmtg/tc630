/*	Copyright (c) 1987 AT&T	*/
/*	All Rights Reserved	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


static char _2Vsccsid[]="@(#)proof.c	1.1.2.31 (4/11/90)";

/*
 * proof.c - jerq interpreter for troff output language
 */

/* includes */
#include <stdio.h>
#include <termio.h>
#include <quiet.h>
#include <signal.h>
#include "host.h"
#include "../include/comm.h"
#include <ctype.h>
#ifndef TIOCEXCL
#include <sgtty.h>
#endif

/* defines */
#define True 1
#define False 0
/* maximum pixels inside a 630 window with proper proportions for 8.5x11 */
#define	XMAX	773
#define	YMAX	1000
/*
 * The 630 has a resolution of 102.4 bits per inch in both directions
 * (the screen is 10 inches X 10 inches), and the 5620 had a resolution
 * of 100 bits per inch (it is believed).
 *
 * Thus, the resolution of the 5620 should be multiplied by 10.24/11
 * and the resolution of the 630 by 10/11.
 *
 * Empirically, though, 91 works.  The only problem is that looks
 * squished.  Use an option.
 */
#define DefXres	91
#define DefYres	91
#define DefInRes 12000	/* default input (troff) resolution ?? */
#define ApsRes	723
#define USAGE "Usage: %s [-VLbcdpr:st:vwz] [file]\n"
#define Options "FLPVbcdlp:stvwx:y:z"
#define LegalNumOfArgs(argc) ((argc-optind) <= 2)
#define LigFactor .75
#define iabs(x)	((x > 0) ? x : (-x))
#define PACKET	100

/* externals */
#ifdef lint
	short ver[2];
#else
	extern short ver[2];
#endif /* lint */
extern char fname[];
extern int newfont;
extern char *optarg;
extern int optind;

/* globals */
short proofterm = 0;
char specchar [4];
short cacheText = 0;
Font curfont;
Font defont;
int jerq;
int scaled = 1;		/* selects scaled mode */
double scalefac = 1.0;	/* used for scaled mode */
int ptsz;		/* used for ligature simulation */
short debugOpt = 0;

#ifdef DEBUG
	FILE *debug = NULL;
	int verbose = False;
#endif /* DEBUG */

/* locals */
static char obuf[PACKET], *op = obuf;
static short packet = PACKET;
static int newword;
static short ligsim = False;
static int firstpage = 0;
static Point curpoint;
static long termXres = DefXres;
static long termYres = DefYres;
static double xFactor = DefXres/DefInRes;
static double yFactor = DefYres/DefInRes;
static int curpage;
static int oldpage = 0;
static int startpage = 0;
static int pageSpecified = False;
static int yoffset = 0;	/* origin of this slice */
/* used for slice detection and scaled mode scale factor */
static int windx = XMAX, windy = YMAX;
static int MaxX = XMAX;
static int MaxY = YMAX;
static short noRevSeeks = False;
#ifdef DEBUG
	static char debugFilename [BUFSIZ];
	static short ignoreGarbage = 0;
#endif /* DEBUG */
static short benchmark = False;
static Point frame;	/* troff usable part of the window */
static int deviceResolution;
static short faststart = 0;
static double PageWidth = 8.5;
static double PageLength = 11;


/* procedures */
extern char *malloc ();
extern char *strcpy ();
extern char *strcat ();


sighup(unused)
int unused;
{
	outchar(EXIT);
	oflush();
#ifdef TIOCNXCL
	(void)ioctl(1,TIOCNXCL,0);
#endif
	exit(0);
}

#ifdef NO_TERMIO
struct sgttyb modes, savetty;
#else
struct termio sttybuf, sttysave;
#endif

char *Progname;

main(argc,argv)
char *argv[];
{


	int option;
	short state;
	short loadError;

	(void)signal(SIGHUP, sighup);

	Progname = &argv[0][strlen(argv[0])];
	while(--Progname > &argv[0][0]) {
		if (*Progname == '/') {
			Progname++;
			break;
		}
	}

	/*
	 *	analyse command line
	 */
	while ((option = getopt (argc, argv, Options)) != EOF)
		switch(option) {
		case 'F':
			faststart = 1;
			break;
		case 'L':
			ligsim++;
			break;
		/*
		 * This option is used primarily on the 630 when the host
		 * side is to be debugged with sdb.  It is used in con-
		 * junction with the -t option to run the host side in
		 * one window, and the term side in the other.  To use:
		 *	- download the term side in one window, using -Pc
		 *	  (you can use dmdpi on this window to db both sides)
		 *	- enter "tty" in that window to find the device
		 *	- enter "kill -9 $$"
		 *	- in another window, create a subshell & export
		 *	  DMDTERM, DMDLD, and DMDLIB.
		 *	- In that window, enter "sdb proof"
		 *	- when ready to run, use the -t option with
		 *	  device name of the other window.
		 * "-P" becomes a term side argument, so if it was
		 * previously cached, it'll need to be re-downloaded.
		 */
		case 'P':
			proofterm = 1;
			break;
		case 'V':
			(void)printf("xproof: version %d.%d\n", ver[0], ver[1]);
			exit(0);
		case 'b':
			benchmark = 1;
			break;
		case 'c':
			cacheText = 1;
			break;
		case 'l':	/* flip maximum sizes for landscape mode */
			windx = MaxX = YMAX;
			windy = MaxY = XMAX;
			PageWidth = 11;
			PageLength = 8.5;
			break;
		case 'p':
			startpage = atoi (optarg);
			pageSpecified = True;
			break;
		case 's':
			scaled = 1;
			break;
#ifdef DEBUG
		case 't':
			/*
			 * the -t option was installed to support debugging
			 * with sdb, although it may have other, unforeseen
			 * possibilities.
			 */
			/*
			 * When using sdb, if you hit the delete key, and
			 * then continue, while waiting for user input,
			 * illegal data will be read, causing an error.
			 * Don't abort in that case.
			 */
			ignoreGarbage = 1;

			/*
			 * let every character be sent immediately to
			 * the terminal.
			 */
			packet = 1;
#endif /* DEBUG */
			break;
		case 'w':
			scaled = 0;
			break;
		case 'x':
			termXres = atoi (optarg);
			break;
		case 'y':
			termYres = atoi (optarg);
			break;
		case 'z':
			debugOpt++;
			break;
#ifdef DEBUG
		case 'd':
			(void)sprintf (debugFilename,
				"/tmp/proof.db%d", getuid());
			debug = fopen(debugFilename, "w");
			break;
		case 'v':
			verbose++;
			break;
#endif /* DEBUG */
		default:
			(void)fprintf(stderr,
			    "unknown option '-%c' ignored!\n", option);
			break;
		}

	if (!LegalNumOfArgs (argc)) {
		(void)fprintf (stderr, USAGE, Progname);
		exit (-1);
	}

	/*
	 * if a troff output file was specified on the command line,
	 * open the file as the stdin.  Thus, the only time that the
	 * stdin is attached to a tty after this point is when the
	 * user is merely downloading proof, with no work to do.
	 */
	if(argv[optind] != NULL) {
		if(freopen(argv[optind], "r", stdin) == NULL) {
			(void)fprintf (stderr,
			    "%s: open of %s failed: ", Progname, argv[optind]);
			(void)perror(argv[optind]);
			exit(1);
		}
	}
	else if (isatty (0) && !cacheText) {
		(void)fprintf (stderr,
		    "%s: preload without caching is useless (use -c option)\n",
		    Progname);
		exit (-1);
	}
	else if(!isatty(0) && lseek (0, 0, 1) < 0) {
		/*
		 * Now, the last consideration is whether stdin
		 * is associated with a pipe or file.  Apparently
		 * some users want to be able to redirect file
		 * input to proof, and have all the benefits of
		 * normal file input.  The best way to tell if
		 * this is true is to see if a seek on stdin fails.
		 */
                if ( faststart || !fixpipe())
                        noRevSeeks = True;
        }

	for (jerq = 0; jerq <= 2; jerq++)
		if (isatty(jerq))
			break;
	if ((jerq > 2) && ((jerq = open("/dev/tty", 2)) == -1)) {
		fprintf (stderr, "proof: Not on a tty\n");
		exit(1);
	}

#ifdef NO_TERMIO
	(void)ioctl(jerq, TIOCGETP, &modes);
	savetty = modes;
	modes.sg_flags|=RAW;
	modes.sg_flags&=~ECHO;
	(void)ioctl(jerq, TIOCSETP, &modes);
#else
	(void)ioctl(jerq, TCGETA, &sttysave);
	sttybuf.c_iflag = IGNBRK;
	sttybuf.c_cflag = (sttysave.c_cflag & (CBAUD | CLOCAL)) | CS8 | CREAD;
	sttybuf.c_lflag = 0;
	sttybuf.c_cc[VMIN] = 1;
	(void)ioctl(jerq, TCSETAW, &sttybuf);
#endif

	state = getstate();
	loadError = (state != AlreadyLoaded) && (state != LoadComplete);



#ifdef TIOCEXCL
	(void)ioctl(1,TIOCEXCL,0);
#endif

	if (!loadError)
	    /*
	     * if stdin is a terminal, then nothing is being piped into
	     * it: thus, the user has merely loaded it, and will control
	     * subsequent processing from the dmd.
	     */
	    if(isatty(0))
		    no_troff(state == AlreadyLoaded);
	    /*
	     * Otherwise, call troff to handle troff data on the
	     * stdin pipe.
	     */
	    else
		    troff(state == AlreadyLoaded);

	oflush();

#	ifdef NO_TERMIO
		(void)ioctl(jerq, TIOCSETP, &savetty);
#	else
		(void)ioctl(jerq, TCSETAW, &sttysave);
#	endif

#ifdef TIOCNXCL
	(void)ioctl(1,TIOCNXCL,0);
#endif

	exit (loadError? state : 0);

	/*NOTREACHED*/
}


fixpipe ()
{
    char tmpname [L_tmpnam];
    FILE *tmpfp;
    char tmpbuf[BUFSIZ];
    int bufcnt;

    tmpnam (tmpname);
    if ((tmpfp = fopen (tmpname, "w")) == NULL) {
	perror ("open of tmp file failed");
	return False;
    }
    else {
	while ((bufcnt = fread(tmpbuf, sizeof(char), BUFSIZ, stdin)) > 0)
	    if (fwrite(tmpbuf, sizeof(char), bufcnt, tmpfp) != bufcnt) {
		perror ("write to tmp file failed");
		fclose(tmpfp);
		return False;
	    }

	if (bufcnt == 0) {
	    rewind (tmpfp);
	    if (freopen (tmpname, "r", stdin) == NULL) {
		perror ("reopen of tmp file failed");
		return False;
	    }
	}

	if (unlink (tmpname) < 0)
	    perror ("fixpipe file unlink failed");
    }

    return True;
}


outchar(c)
register char c;
{
#ifdef	DEBUG
	if (verbose&&debug)
		(void)fprintf (debug, "outchar(0%o)\n", c);
#endif	/* DEBUG */
	*op++ = c;
	if (op-obuf == packet) {
		oflush();
	}
}

oflush()
{
#ifdef	DEBUG
	if(verbose&&debug)
		(void)fprintf(debug, "oflush()\n");
#endif	/* DEBUG */
	if(op != obuf)
	{
		(void)write(jerq, obuf, (unsigned)(op-obuf));
		op = obuf;
	}
}

putshort(i)
register int i;
{
	outchar(i>>8);
	outchar(i&0377);
}

putpoint(p)
Point p;
{
	putshort(p.x);
	putshort(p.y - yoffset);
}

int saved;

inchar()
{
	register int c;

	if(c = saved)
		saved = 0;
	else
		c = getchar();
	return(c);
}

p_unchar(c)
int c;
{
	saved = c;
}

eatline()
{
	register int c;

	while (((c=inchar()) != '\n') && (c != EOF))
		/* empty */;
}

inkbd()
{
	char c;
	register int i;

	oflush();
	if(read(jerq,&c,1)!=1)
		c=4;		/* ^D, looks like EOF */
	i = c & 0377;
#ifdef	DEBUG
	if(verbose&&debug)
		(void)fprintf(debug, "inkbd(0%o)\n", i);
	if(debug)fflush(debug);
#endif	/* DEBUG */
	return(i);
}

/*
 * look for WIND command.  if not WIND, return the character received.
 *   if WIND, read the window size parameters and return WIND.
 */
inwind()
{
	register int i;

	if((i = inkbd()) == WIND)
	{
		/* construct a 16 bit x size (?) from jerq input */
		i = (inkbd()&0377) << 8;
		windx = i | (inkbd()&0377);

		/* construct a 16 bit y size (?) from jerq input */
		i = (inkbd()&0377) << 8;
		windy = i | (inkbd()&0377);

		if(windx*MaxY > windy*MaxX)	/* ??? */
			scalefac = ((double)windy)/MaxY;
		else
			scalefac = ((double)windx)/MaxX;
#ifdef	DEBUG
		if(debug)
			(void)fprintf(debug,
				"wind = %d,%d scalefac = %f\n",
				windx, windy, scalefac);
#endif	/* DEBUG */
		return(WIND);
	}
#ifdef	DEBUG
	if(debug)
		(void)fprintf(debug, "inwind(0%o)\n", i);
#endif	/* DEBUG */
	return(i);
}

/*
 * like inkbd but process WIND messages
 */
interm()
{
	register int i;

	while((i = inwind()) == WIND)
		/* empty */;
	
	return(i);
}


skip()
{
	register int c;
	while ((c = inchar()) == ' ')
		;
	p_unchar(c);
}


/*
 * get an n from the troff input.  Does an initial skip
 */
inint()
{
	register int i = 0,c,mflag = 0;

	while (!isdigit((c = inchar())))
		if (c == '-')
			mflag = 1;
	do {
		i = i*10 + c-'0';
	} while (isdigit((c = inchar())));	/* eats up an extra char */
	p_unchar(c);				/* spits it back out */
	return(mflag ? -i : i);
}


/*
 * get a point (x,y coordinate pair) from the troff input
 */
Point
inpoint()
{
	Point p;

	p.x = inint();
	p.y = inint();
	return(p);
}


scalex(x)
{
	register int scaledX;

	/*
	 * make the "real" troff image fit on the (slightly smaller)
	 * screen.  Although the new troff has the same resolution in
	 * both the x and y directions, as does the 630, there is
	 * nevertheless a scalex(), and a scaley() so that these
	 * factors can be independently tweaked.
	 */
	scaledX = x * xFactor;
	if(scaled)
		scaledX = scaledX * scalefac;
	return(scaledX);
}

scaley(y)
{
	register int scaledY;

	/*
	 * make the "real" troff image fit on the (slightly smaller)
	 * screen.  Since the new troff has the same resolution in
	 * both the x and y directions, as does the 630, there is
	 * nevertheless a scalex(), and a scaley() so that these
	 * factors can be independently tweaked.
	 */
	scaledY = y * yFactor;
	if(scaled)
		scaledY = scaledY * scalefac;
	return(scaledY);
}

Point
scalept(p)
Point p;
{
	p.x = scalex(p.x);
	p.y = scaley(p.y);
	return(p);
}

Point
add(p,q)
Point p,q;
{
	p.x += q.x;
	p.y += q.y;
	return(p);
}

Point
wiggly(p)		/* starting at p */
Point p;
{
	register int n = 1;
	Point pts[60],*pp = pts;

	*pp++ = scalept(p);
	while (inchar() != '\n') {	/* go 'till end of line */
		p = add(p,inpoint());	/* additional points are increments */
		*pp++ = scalept(p);
		n++;
	}
/*	jspline(n+1,pts,F_OR);		/* but we only have n points */
	outchar(SPLINE);
	outchar(n);
	pp = pts;
	do {
		putpoint(*pp++);
	} while (--n > 0);
	return(p);
}

Point
Pt(x,y)
{
	Point p;

	p.x = x;
	p.y = y;
	return(p);
}


/* called by troff() */
Point
draw(p)
Point p;
{
	register int a,b;
	Point pp,q,p1,p2;

	pp = scalept(p);

	switch (inchar()) {
	case 'l':	/* draw a line */
		q = add(p,inpoint());
	/*	jsegment(pp,scalept(q),F_OR);	*/
		outchar(LINE);
		putpoint(pp);		/* send scaled starting point */
		/*
		 * assert ((pn*s + d)*s == (pn+d)*s)
		 * is this true?
		 */
		putpoint(p1 = scalept(q));	/* p1 used for debug */

#ifdef	DEBUG
		if(debug)
			(void)fprintf(debug,
				"Line (%d,%d) to (%d,%d)\n",
				pp.x, pp.y, p1.x, p1.y);
#endif	/* DEBUG */

		p = q;
		break;

	case 'c':	/* circle */
		a = inint()/2;
		p = add(p,Pt(a,0));
	/*	jcircle(scalept(p),scale(a),F_OR);	*/
		outchar(CIRCLE);
		putpoint(scalept(p));
		putshort(scalex(a));	/* choice of scalex() is arbitrary */
		break;
	case 'e':	/* ellipse */
		a = inint()/2;
		b = inint()/2;
		p = add(p,Pt(a,0));
	/*	jellipse(scalept(p),scale(a),scale(b),F_OR);	*/
		outchar(ELLIPSE);
		putpoint(scalept(p));
		putshort(scalex(a));	/* choice of scalex() is arbitrary */
		putshort(scalex(b));	/* choice of scalex() is arbitrary */
		break;
	case 'a':	/* arc */
		p1 = add(p,inpoint());
		p2 = add(p1,inpoint());
	/*	jarc(pp,scalept(p2),scalept(p1),F_OR);	*/
		outchar(ARC);
		putpoint(scalept(p1));
		putpoint(pp);
		putpoint(scalept(p2));
		p = p2;
		break;
	case '~':	/* wiggly line */
		p = wiggly(p);
		break;
	default:
		break;
	}
	eatline();
	return(p);
}

Point cursor;

no_troff(loaded)
{
	/* clear the terminal's screen */
	outchar(CLEAR);

	/* tell blit to go away */
	outchar(ASCII);

	oflush();

	/*
	 * if the terminal side was downloaded, something's got
	 * to be eaten.
	 */
	if(!loaded)
		eatline();
}

troff(loaded)
{
	register int c;

	/*
	 * Send 2 or 3 REQ messages to the terminal.  After at least the
	 *  last one it will add on a WIND message which we should explicitly
	 *  eat in case we never do another interm() (which also eats WIND
	 *  messages).
	 */
	for(c = (loaded? 2 : 3); c; c--)
	{
		outchar(REQ);
		(void)interm(); /* grab NAK */
		(void)interm(); /* grab newline as well */
	}
	(void)inwind();

	/*
	 * I now presume that troff(1) will send a x-r command before
	 * anything is displayed.  devcntrl() sends a CLEAR after it
	 * knows the frame dimensions.  This should eliminate a lot of
	 * redundant flashing.
	outchar(CLEAR);
	 */
	outchar(scaled? SCALE : WINDOW);

	cursor.x = cursor.y = 0;
	yoffset = 0;
	while ((c = inchar()) != EOF) {
		if(scaled == 0) {
		    /*
		     * This is where slices (i.e. the end of a window
		     * of displayed text) are detected.
		     */
		    if (scaley (cursor.y)-yoffset > windy) {
#			ifdef DEBUG
			    if(debug)
				(void)fprintf (debug,
			    "cursor.y=%d, yoffset=%d, windy=%d: at SLICE\n",
					cursor.y, yoffset, windy);
#			endif /* DEBUG */
			yoffset += windy;
			if(page(SLICE))
			    return;
		    }
		}

		switch (c) {
		case '\n':	/* when input is text */
		case ' ':
		case 0:		/* occasional noise creeps in */
			break;
		case '0': 
		case '1': 
		case '2': 
		case '3': 
		case '4':
		case '5': 
		case '6': 
		case '7': 
		case '8': 
		case '9':
			/* two motion digits plus a character */
			cursor.x += (c-'0') * 10 + inchar()-'0';
			drawchar(cursor, inchar());
			break;
		case 'c':	/* single ascii character */
			drawchar(cursor,inchar());
			break;
		case 'C':
			special(cursor);
			break;
		case 'D':	/* draw function */
			cursor = draw(cursor);
			curpoint = scalept(cursor);
			break;
		case 'H':	/* absolute horizontal motion */
			cursor.x = inint();
			break;
		case 'h':	/* relative horizontal motion */
			cursor.x += inint();
			break;
		case 'V':
			cursor.y = inint();
			break;
		case 'v':
			cursor.y += inint();
			break;
		case 'p':	/* new page */
			/*
			 * Save the old curpage before changing it in case
			 * user decides to flip between scaled and window
			 * mode at the end of a page.
			 */
			oldpage = curpage;
			curpage = inint();
#			ifdef DEBUG
			    if (debug) {
				(void)fprintf (debug, "Page %d, ", curpage);
				(void)fprintf (debug, "yoffset %d\n", yoffset);
			    }
#			endif
			/*
			 * The next two instructions used to follow
			 * the call to page().  It shouldn't hurt to put
			 * them here, because it is already known that a
			 * new page is necessary.  Moving them here ensures
			 * that the new frame dimensions will be sent to
			 * the terminal in time for the next stipple().
			 */
			cursor.y = 0;	/*avoid fake slices */
			yoffset = 0;
			if(firstpage++) {
				if(page(PAGE))
					return;
			}
			else {
				/*
				 * find start page after initial troff
				 * statements have been processed.
				 */
				if (pageSpecified) {
					curpage = pageseek (startpage);
					pageSpecified = False;
				}
			}
			/*
			 * Clear the old page count because we will be starting
			 * to fill the new current page.
			 */
			oldpage = 0;
			break;
		case 's':
			newsize();
			break;
		case 'f':
			newfamily();
			break;
		case 'n':	/* end of line */
			eatline();
			break;
		case 'w':	/* word space */
			newword = 1;
			break;
		case 'x':	/* device control */
			if(devcntrl())
				return;
			break;
		case '#':	/* comment */
		default:
			/*
			 * It used to say:
			eatline();
			 *
			 * But I want to detect when somebody inadvertently
			 * sends troff input, instead of output.
			 */

			(void)fprintf (stderr, "proof: invalid input '%c' (%x).  Have you troffed?\n", c, c);

			/* tell term to go away */
			outchar (EXIT);

#			ifdef DEBUG
			    if (debug) {
				*op = '\0';	/* make sure str is termed */
				(void)fprintf (debug,
				    "%d unsent on error exit: %s\n",
				    op - obuf, obuf);
			    }
#			endif /* DEBUG */

			return;
		}
	}
	outchar(ASCII);	/* tell blit to go away */
	oflush();

	sleep(2);	/* give it time to do so */
}

page(c)
{
	int pagenum;

	/*
	 * tell the terminal what's happening on the host side
	 * (PAGE or SLICE), so it knows to expect mouse input.
	 * (except when in benchmark mode, which is for comparing
	 * the speeds of two different proofs: just charge through
	 * the input files without waiting for user input).
	 */
	if (!benchmark)
	    outchar(c);

	while (True) {
	    if (benchmark)
		c = ACK;
	    else
		c = interm();
	    switch(c) {
	    case EXIT:
		    return(1);
	    case ACK:
		    sendframe ();	/* doesn't include benchmark support */
		    return(0);
	    case PAGE:
		    /*
		    ** Replace this line with 2 lines below.
		    **
		    ** pagenum = inkbd() + (inkbd()<<8);
		    */
		    pagenum = inkbd() << 8;
		    pagenum += inkbd();
		    curpage = pageseek(pagenum);	/* calls sendframe() */
		    sendframe ();	/* doesn't include benchmark support */
		    return(0);
	    case SCALE:
	    case WINDOW:
		    scaled = c == SCALE;
		    /*
		     * Reset the current page if we have just processed a troff
		     * page command (i.e., user is changing modes at the bottom
		     * of a page).
		     */
		    if (oldpage > 0)
			curpage = oldpage;
		    pageseek(curpage);		/* calls sendframe() */
		    cursor.y = yoffset = 0;
		    sendframe ();	/* doesn't include benchmark support */
		    return(0);
	    default:
#		    ifdef DEBUG
			if (!ignoreGarbage)
			    /*
			     * I don't know when this is ever executed, but
			     * I guess it's better to be safe than sorry.
			     */
			    (void)abort ();
#		    else
			(void)abort ();
#		    endif /* DEBUG */
	    }
	}	/* end while */

	/*NOTREACHED*/
}


sendframe ()
{
	Point newwin;

	newwin = scalept (frame);
#	ifdef DEBUG
	    if (debug) {
		(void)fprintf (debug,
		    "frame.x=%d, frame.y=%d\n", frame.x, frame.y);
		(void)fprintf (debug,
		    "newwin.x=%d, newwin.y=%d\n", newwin.x, newwin.y);
	    }
#	endif
	outchar (FRAME);
	putpoint (newwin);
}


special(p)
Point p;
{
	char name[20], *cp = specchar;

	/*
	 * read the "xyz" (troff(5)) name of the special character.
	 * Note that the manpage seems to indicate that we should
	 * expect 3, not just 2 char names.
	 */
	*cp++ = inchar();
	*cp++ = inchar();
	*cp++ = 0;
 
	/*
	  The way proof apparently works is that it maintains its own idea of
	  where the cursor is at any point, but if a movement command comes
	  in from troff, that overrides proof's own position calculation for
	  the next character.  Since troff generally sends movement commands,
	  proof is constantly adjusting its own cursor, trying to track
	  troffs.

	  Apparently, the reason it does this is because the term side always
	  updates its notion of where the next character should go - as
	  determined by the width information in the font - and the host
	  side knows this is going on.  If an explicit request comes from
	  troff, the host side sends the *difference* between troff's request
	  and the characters width as stored in the font.  The only reason
	  I can think for doing it this way is ... so that proofterm can
	  do its own spacing when the host - and troff - are not around.


	  The idea way to simulate ligatures would be to use a factor
	  calculated by dividing the actual width of the ligature, as
	  determined by troff, by the sum of the widths of all characters,
	  as determined by proof from the font widths.  This factor would be
	  applied to the widths of each character when it incremented its
	  own cursor.

	  The problem is, proof has no idea how much space troff is going to
	  allocate for a ligature until *after* it has drawn the ligature.
	  Therefore, I think that the only solution is to use the widths
	  stored with the fonts for the individual characters of the ligature.
	  Then, when troff subsequently issues a horizontal movement command,
	  proof will have to ignore it.  This will result in lines that are
	  too long, but I'm not sure how else to handle the problem.

	  This approach makes the assumption that there are only three types
	  of horizontal movement commands: implicit, absolute, and relative.
	  After a ligature is simulated, the algorithm would set a flag called
	  "ignoreLigSpace".  Implicit and relative horizontal movements
	  would check this flag, and not space if its set.  Absolute
	  horizontal movements would merely clear it.


	  An even simpler approach would be to just assume that ligatures
	  space about 3/4 the width that normal characters do.  This will
	  usually look better than the full widths would, and will probably
	  eliminate the need to do any complicated recovery for the difference
	  in real vs. simulated ligature width.


	  Note that since p is a local variable, the amount of horizontal
	  motion added to it by the ligature will not be reflected in the
	  p in host.troff().  Thus, when troff finally sends the next char
	  with its implicit horizontal motion specifier, troff()'s p
	  will still have the position of the *origin* of the ligature.
	 *
	 */
	if (ligsim) {
#ifdef	    DEBUG
		if (debug)
		    (void)fprintf (debug,
		        "special: the width of f is %d and of %c is %d\n",
			curfont.width['f'],
			specchar[1], curfont.width[specchar[1]]);
#endif	    /* DEBUG */
	    /*
	     * First, test for fi, ff, fl ligatures.
	     */
	    if ((strcmp(specchar, "fi") == 0)
	     || (strcmp(specchar, "ff") == 0)
	     || (strcmp(specchar, "fl") == 0))
	    {
		    drawchar(p, 'f');
		    p.x += (curfont.width['f']/xFactor)*LigFactor;

		    drawchar(p, specchar[1]);
		    p.x += (curfont.width[specchar[1]]/xFactor)*LigFactor;

		    return;
	    }
     
	    /*
	     * Now test for ffi and ffl ligatures, which troff sees
	     * as Fi and Fl.  Convert to ascii characters
	     */
	    else if ((strcmp(specchar, "Fi") == 0)
		  || (strcmp(specchar, "Fl") == 0))
	    {
		    drawchar(p, 'f');
		    p.x += (curfont.width['f']/xFactor)*LigFactor;

		    drawchar(p, 'f');
		    p.x += (curfont.width['f']/xFactor)*LigFactor;

		    drawchar(p, specchar[1]);
		    p.x += (curfont.width[specchar[1]]/xFactor)*LigFactor;

		    return;
	    }
	}

	/*
	 * build the name, corresponding to this special character
	 * at the current point size.
	 */
	(void)sprintf(name, "S.%d/%s", ptsz, specchar);

	/*
	 * This "font" must be loaded.  Do it.  If, after trying
	 * to load it, its width is equal to the defont width,
	 * assume that it wasn't found, so don't bother to try
	 * to draw something from the wrong character set.
	 *
	 * Presumably, if the second term of the following boolean
	 * expression were eliminated, missing special chars would
	 * print out with the block cursor character (which is at
	 * char cell 1 of defont, I believe).  That may be desirable
	 * (so documents don't go out the door with holes in them),
	 * however, perhaps that wouldn't work after all: findfont()
	 * normally returns false if a font file can't be found -
	 * indeed, probably the only time that it doesn't is when
	 * the "there" flag in the host-side's font table is false:
	 * only in that case would the cursor character be used.
	 * But if that were a desirable feature, something could
	 * be done.
	 */
	if (findfont(name) && (curfont.width != defont.width))
		/*
		 * Presumably, in special character files, the special
		 * character will be in character cell 1.
		 */
		drawchar(p,1);

	/*
	 * Now, don't forget to switch back to the standard char set
	 * before printing again.
	 */
	newfont = 1;
}

/*
 *  jsegment(p,q)
 *  Point p,q;
 *  {
 *    outchar(LINE);
 *    putpoint(p);
 *    putpoint(q);
 *  }
 */


drawchar(p,c)
Point p;
{
	register int dx;
	char s[2];

	/* build a string out of the input char */
	s[0] = c; s[1] = 0;

	/*
	 * if anyone has requested that a new font be loaded,
	 * do it before drawing again.
	 */
	if (newfont)
		(void)findfont(fname);

	/*
	 * convert troff coordinate to terminal coordinate,
	 * and do any necessary scaling.
	 */
	p = scalept(p);

	/* x distance to traverse */
	dx = p.x-curpoint.x;

	/*
	 * If large x movement, or any y, send the whole point, 
	 * Otherwise make a relative movement.  In any case, the
	 * new position becomes the current position.  I don't yet
	 * know what purpose newword serves.
	 */
	if (p.y != curpoint.y || (iabs(dx) > 127)) {
		outchar(POS);
		putpoint(p);
#		ifdef DEBUG
		    if(debug)
			(void)fprintf(debug,"goto (%d,%d)\n",p.x,p.y);
#		endif /* DEBUG */
		curpoint = p;
		newword = 0;
	}
	else if (newword == 1 || iabs(dx) > 0) {
		outchar(RIGHT);
		outchar(dx);
#		ifdef DEBUG
		    if(debug)
			(void)fprintf(debug,
			    "go right %d\n",(dx>127)?128-(dx&0x7f):dx);
#		endif /* DEBUG */
		curpoint = p;
		newword = 0;
	}

	/* now actually send the character to print */
	outchar(c);

#	ifdef DEBUG
	    if(debug)
		(void)fprintf(debug,"%c$",c);
#	endif /* DEBUG */

	/*
	 * Set the host cursor to be the same as the terminal's
	 * cursor.  The terminal maintains its own cursor so it's
	 * not dependent on the host - in order to support proofterm.
	 * Therefore, the host has to track where the term's cursor
	 * is.
	 */
	curpoint.x += curfont.width[c];
}

devcntrl()
{
	skip();
	switch(inchar()) {
	case 'f':
		getfamily();
		p_unchar('\n');	/* anti-hunger */
		break;
	case 'r':
		deviceResolution = inint();
		xFactor = (double)termXres/(double)deviceResolution;
		yFactor = (double)termYres/(double)deviceResolution;
#ifdef		DEBUG
		    if (debug)
			(void)fprintf (debug,
				"xFactor is %f, yFactor is %f\n",
				xFactor, yFactor);
#endif		/* DEBUG */
		frame.x = PageWidth * deviceResolution;
		frame.y = PageLength * deviceResolution;

		sendframe ();		/* doesn't support benchmark opt */
		/* cause the new frame to be created */
		outchar(CLEAR);

		break;
	case 's':	/* stop - time for a breather */
		if(page(PAGE))
			return(1);
		break;
	default:
		break;
	}
	eatline();		/* hunger */
	return(0);
}


/*
 * returns the new current page
 */
pageseek(wantedPage)
	int wantedPage;
{
	long startLoc, lastPageLoc;
	char buf[512];
	int foundPage = 0;
	short rewound = False;
	short higherPage = False;	/* it's known that wantedPage > curpage */

	/*
	 * The current implementation does not support reverse seeking
	 * on piped input.  If anybody tries it, don't even carry it
	 * through, because pageseek() will read through the whole
	 * file, leaving the user out in the cold.
	 */
	if (noRevSeeks && (wantedPage <= curpage))
		return curpage;

	/* save our current location in the troff file */
	startLoc = ftell(stdin);

	/* search forward for a troff page command */
	while(True) {
		if (gets(buf) == NULL) {
			/*
			 * if foundPage is 0, we're on the last page.
			 * Thus, we'll never find a 'p' command, and
			 * the search will fail.  In that case, it is
			 * clear that we must rewind first.
			 */
			if (foundPage == 0 && !rewound) {
				rewind(stdin);
				rewound = True;
				continue;
			}
			else
				/*
				 * otherwise, the entire file has been
				 * searched, and nothing has turned up.
				 */
				break;
		}

		/*
		 * search for the next page to check its number.
		 */
		if(buf[0] == 'p') {
			/* save location following found troff page command */
			lastPageLoc = ftell(stdin);

			/* read the page number of this page */
			(void)sscanf(&buf[1], "%d", &foundPage);

			/* is this the right page? */
			if(foundPage == wantedPage) {
				/*
				 * if so, throw away any pending (put-back)
				 * chars ... but why?  Who is suspected of
				 * p_unchar-ing?
				 */
				p_unchar(0);

				cursor.y = yoffset = 0;
				return wantedPage;
			}
			else if (foundPage < wantedPage)
				/*
				 * if the foundPage is ever < wantedPage,
				 * then it is known that the wantedPage
				 * is higher than the startPage.  This
				 * is important for missing pages: there
				 * is no need to rewind.
				 */
				higherPage = True;
			else if (wantedPage != 0) {		/* foundPage > wantedPage */
				if (rewound || higherPage)
					break;
				else {
				    /* go back to its beginning */
				    rewind(stdin);
				    rewound = True;
				}
			}
		}
	}

	/*
	 * The sought page was not found.
	 */
	/*
	 * Apparently, if the user specifies (from the mouse) that he
	 * wants to see page 0, he is returned to the start of the
	 * last page.  Otherwise, he should be returned to where he was.
	 * The user can specify page 0 only by giving a '$' sign.
	 */
	if (wantedPage == 0) {
		(void)fseek(stdin, lastPageLoc, 0);

		cursor.y = yoffset = 0;		/* start at top of page */

		return foundPage;
	}

	(void)fseek(stdin, startLoc, 0);
	return curpage;
}
