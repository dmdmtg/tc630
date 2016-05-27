/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)driver.c	1.1.1.4	(11/13/87)";

/* includes */
#include <stdio.h>
#include "jplot.h"

/* defines */
#define Usage "Usage: %s [-VDcdfs:tz]\n"
#define Options "VDcdfs:tz"

/* externals */
extern char *optarg;
extern int optind;
#ifdef lint
	short verno = 0;
	short subno = 0;
#else
	extern short verno;
	extern short subno;
#endif

/* globals */
char obuf[BUFSIZ];
short cacheText = 0;
short dumpMode = 0;
short debugMode = 0;
short stopped = 0;
short frameMode = 0;
short termSimulation = 0;
FILE *dbfp;

/* procedures */
extern void Flush ();
extern void point ();
extern void exit ();
extern void perror ();

char *Pgm_name;

main (argc, argv)
	int argc;
	char **argv;
{
	short option;
	short argx;
	FILE *fin;

	Pgm_name = &argv[0][strlen(argv[0])];
	while(--Pgm_name > &argv[0][0]) {
	    if (*Pgm_name == '/') {
		Pgm_name++;
		break;
	    }
	}

	/*
	 * use obuf as the buffer for stdout instead of the
	 * system supplied buffer.  I'm not sure why.
	 */
	setbuf(stdout, obuf);

	while ((option = getopt (argc, argv, Options)) != EOF)
	    switch (option) {
	    case 'V':
		(void)printf("xtdmd: version %d.%d\n", verno, subno);
		exit(0);
	    case 'D':
		dumpMode = 1;
		break;
	    case 'c':
		cacheText = 1;
		break;
	    case 'd':
		debugMode = 1;
		break;
	    case 'f':
		frameMode = 1;
		break;
	    case 't':
		termSimulation = 1;
		break;
	    case 'z':
		stopped = 1;
		break;
	    default:
		(void)fprintf (stderr, Usage, Pgm_name);
		exit (1);
		break;
	    }

	if (debugMode || dumpMode)
	    if ((dbfp = fopen (DbFile, "w")) == NULL) {
		perror ("open debug file");
		exit (1);
	    }

	if (optind == argc) {
	    fplt( stdin );
	    exit(0);
	}

	for (argx = optind; argx < argc; argx++) {
		if ((fin = fopen(argv[argx], "r")) == NULL) {
			(void)fprintf(stderr, "%s: can't open %s\n",
				Pgm_name, argv[argx]);
			exit(1);
		}
		fplt(fin);
	}

	exit(0);	/*NOTREACHED*/
}


fplt(fin)
	FILE *fin;
{
	int c;
	char s[256];
	int xi,yi,x0,y0,x1,y1,r,i;
	short texture1;
	short num;
	Point points[128];

	openpl();
	while((c=getc(fin)) != EOF){

#		ifdef DEBUG
		    if (debugMode)
			(void)fprintf (dbfp, "\ncommand is %c ", c);
#		endif /* DEBUG */

		switch(c){
		/* unknown, undocumented command */
		case 'P':
			(void)getsi (fin);
			break;
		/* unknown, undocumented command */
		case 'o':
			(void)getsi (fin);
			break;
		/* unknown, undocumented command */
		case 'z':
			(void)getsi (fin);
			(void)getsi (fin);
			break;
		case 'm':
			xi = getsi(fin);
			yi = getsi(fin);
			move(xi,yi);
			break;
		case 'l':
			x0 = getsi(fin);
			y0 = getsi(fin);
			x1 = getsi(fin);
			y1 = getsi(fin);
			line(x0,y0,x1,y1);
			break;
		case 'x':
			Flush();
			break;
		case 'd':
			delay();
			break;
		case 'F':
			texture1 = getsi(fin);
			num = getsi(fin);
			for (i = 0 ; i < num ; i++) {
				points[i].x = getsi(fin);
				points[i].y = getsi(fin);
			}
			fill (texture1, num, points);
			break;
		case 't':
			newgets(s,fin);

#			ifdef DEBUG
			    if (debugMode)
				(void)fprintf (dbfp, ": %s", s);
#			endif /* DEBUG */

			if (*s >= ' ')
				label(s);
			break;
		case 'e':
			erase();
			break;
		case 'p':
			xi = getsi(fin);
			yi = getsi(fin);
			point(xi,yi);
			break;
		case 'n':
			xi = getsi(fin);
			yi = getsi(fin);
			cont(xi,yi);
			break;
		case 's':
			x0 = getsi(fin);
			y0 = getsi(fin);
			x1 = getsi(fin);
			y1 = getsi(fin);
			space(x0,y0,x1,y1);
			break;
		case 'a':
			xi = getsi(fin);
			yi = getsi(fin);
			x0 = getsi(fin);
			y0 = getsi(fin);
			x1 = getsi(fin);
			y1 = getsi(fin);
			arc(xi,yi,x0,y0,x1,y1);
			break;
		case 'c':
			xi = getsi(fin);
			yi = getsi(fin);
			r = getsi(fin);
			circle(xi,yi,r);
			break;
		case 'f':
			newgets(s,fin);

#			ifdef DEBUG
			    if (debugMode)
				(void)fprintf (dbfp, ": %s", s);
#			endif /* DEBUG */

			linemod(s);
			break;
		}
	}
	closepl();
}


getsi(fin)
FILE *fin;
				/* get an integer stored in 2 ascii bytes. */
{
	short a, b;

	if((b = getc(fin)) == EOF)
		return(EOF);
	if((a = getc(fin)) == EOF)
		return(EOF);

	a = a<<8;

#	ifdef DEBUG
	    if (debugMode)
		(void)fprintf (dbfp, " %d", a|b);
#	endif /* DEBUG */

	return(a|b);
}


newgets(s,fin)
char *s;
FILE *fin;
{
	int i;
	for(i = 0 ; (i < 256) && (*s = getc(fin)); i++, s++)
		if(*s == '\n')
			break;
	*s = '\0';
	return;
}
