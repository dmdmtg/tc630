/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)open.c	1.1.1.5	(11/13/87)";

/* includes */
#include	"jplot.h"
#include	<termio.h>
#include	<quiet.h>
#include	<signal.h>
#include	<stdio.h>
#include	<sys/jioctl.h>
#include	<string.h>
#ifndef TIOCEXCL
#include	<sgtty.h>
#endif

/* defines */
#define SENDTERMID "\033[c"
#define TERM_1_0 "\033[?8;7;1c"
#define TERMB_1_0 "\033[?8;7;2c"
#define TERM_1_1 "\033[?8;7;3c"
#define TERM_DMD "\033[?8;"
#define TERMIDSIZE 9
#define STR_EQUAL 0
#define ENC_CHK "\033[F"
#define	ENC_LEN	4
#define Eq(str1, str2) \
	(strcmp (str1, str2) == 0)

/* externals */
extern short cacheText;
extern short stopped;
extern short debugMode;
extern FILE *dbfp;
extern char *Pgm_name;

/* globals */
char *termtype;

struct termio
	cooked, fcooked,	/* cooked tty modes */
	ttysave,		/* save the state of tty */
	ttyraw
;

/* procedures */
extern char *getenv();
extern void exit ();
extern unsigned sleep ();

void
openpl()
{
	struct termio
		raw			/* raw tty modes */
	;

	char 
		*ttyname(),		/* return the tty name if found */
		cmd[120],		/* 32ld cmd to be */
		*dwnldflag,		/* for use in determining hex mode */
		romversion		/* last digit of rom version id string */
	;
	char scr_buf[30];
	char termid[TERMIDSIZE+1];
	int count;
	int lpindex;
	char tdmdmod [BUFSIZ];

	if ((termtype = getenv ("DMDTERM")) == NULL) {
	    termtype = "630";
	}

	/*
	 * torig[xy] is the usr's origin in 630 coordinates.
	 * porig[xy] is the usr's origin in usr coordinates.
	 * delt[xy] is the distance in screen coordinates to the usr's corner.
	 */
	if (Eq (termtype, "630")) {
	    torigy = 1024.;			/* term orig y */
	    torigx =    0.;			/* term orig x */

	    porigy =    0.;			/* plot orig y */
	    porigx =    0.;			/* plot orig x */

	    deltx =  1024.;			/* length of 630 x */
	    delty = -1024.;			/* length of 630 y */
	}
	else {
	    torigy = 800.;			/* term orig y */
	    torigx =   0.;			/* term orig x */

	    porigy =   0.;			/* plot orig y */
	    porigx =   0.;			/* plot orig x */

	    deltx =  800.;			/* length of 5620 x */
	    delty = -800.;			/* length of 5620 y */
	}

	/*
	 * scale[xy] is the ratio of 630 coordinates to usr coordinates.
	 */
	scalex =  1.0;			/* scale factor x */
	scaley = -1.0;			/* scale factor y */

	mpx = 0;			/* 0 if standalone, 1 if mpx */
	wantready = 0;			/* 0 if blast ahead, 1 if want READY */
	hex_mode = 0;			/* 1 if 6-bit path used */
	tojerq = -1;			/* file descriptor to jerq */
	fromjerq = -1;			/* file descriptor from jerq */
	lastx = -1;			/* current position x */
	lasty = -1;			/* current position y */
	for (fromjerq = 0; fromjerq <= 2; fromjerq++)
		if (isatty(fromjerq))
			break;
	if (fromjerq > 2) {
		(void)fprintf(stderr,"xtdmd: not on a tty\n");
		exit(2);
	}
	tojerq = dup(fromjerq);

	/*
	 * build the appropriate module name
	 */
	tdmdmod[0] = '\0';
	if (*Pgm_name == 'x')
		(void)strcat (tdmdmod, "x");
	(void)strcat (tdmdmod, "t");
	(void)strcat (tdmdmod, termtype);
	(void)strcat (tdmdmod, ".m");

	if (getenv("DMDLD") == NULL)
		putenv("DMDLD=dmdld");
	if (getenv("DMDLIB") == NULL) {
		static char dmdlibenv[128];
		char *dmd;

		if ((dmd = getenv("DMD")) == NULL) {
#ifdef DEFDMD
			dmd=DEFDMD;
#else
			fprintf(stderr, "$DMD must be set\n");
			exit(1);
#endif
		}
		sprintf(dmdlibenv, "DMDLIB=%s/lib", dmd);
		putenv(dmdlibenv);

	}
	/* build the appropriate download command */
#if (defined(USE_HOSTAGENT) || !defined(JMPX))
	if (inlayers())
#else
	if (ioctl(fromjerq, JMPX, 0) != -1)
#endif
	{
		mpx = 1;
		(void)sprintf(cmd, "$DMDLD %s $DMDLIB/%s %s <&%d >&0 2>&0",
			stopped? "-z" : "",
			tdmdmod,
			cacheText? "-c" : "",
			fromjerq);
#		ifdef DEBUG
			if (debugMode)
				(void)fprintf (dbfp, "%s\n", cmd);
#		endif /* DEBUG */
	} else {
		(void)ioctl(tojerq, TCGETA, &ttysave); /* get the current state */
#ifdef TIOCEXCL
		(void)ioctl(tojerq, TIOCEXCL, 0);
#endif
		ttyraw.c_iflag = IGNBRK;
		ttyraw.c_cflag = (ttysave.c_cflag & CBAUD) |
				 (ttysave.c_cflag & CLOCAL) | CS8 | CREAD;
		ttyraw.c_cc[VMIN] = 1;
		(void)ioctl(tojerq, TCSETAW, &ttyraw);

		(void)write(tojerq, SENDTERMID, (unsigned) strlen(SENDTERMID));
		count = 0;
		while(count < TERMIDSIZE){
			lpindex = read(fromjerq,&termid[count],TERMIDSIZE);
			if(lpindex > 0)count += lpindex;
		}
		if ((strcmp(termid,TERM_1_0) == STR_EQUAL) ||	/* equal strings */
			(strcmp(termid,TERMB_1_0) == STR_EQUAL))
			error("Error: Firmware not updated to 1.1 or greater\n");
		if (strncmp(termid,TERM_DMD,strlen(TERM_DMD)) != STR_EQUAL)
			error("Error: 32ld must be run on a 5620 terminal\n");
		if (strcmp(termid,TERM_1_1) == STR_EQUAL) {

		    if (((dwnldflag = getenv("DMDLOAD")) != NULL) &&
			(dwnldflag[0] != NULL)) {
			    if(strcmp(dwnldflag, "hex") == 0)
				error("Error: Encoding not supported for 1.1 firmware in stand-alone mode\n");
		    }
		}
		else {
		    (void)write(tojerq, ENC_CHK, (unsigned) strlen(ENC_CHK));
		    count = 0;
		    while(count < ENC_LEN){
			lpindex = read(fromjerq,&scr_buf[count],ENC_LEN);
			if(lpindex > 0)count += lpindex;
		    }
		    if( scr_buf[2] == '1' )
			hex_mode = 2;
		    else {
			    if (((dwnldflag = getenv("DMDLOAD")) != NULL) && (dwnldflag[0] != NULL)) {
				if(strcmp(dwnldflag, "hex") == 0)
				    hex_mode = 2;
			    }
		    }
		}

#ifdef TIOCNXCL
		(void)ioctl(tojerq, TIOCNXCL, 0);
#endif
		(void)ioctl(tojerq, TCSETAW, &ttysave);
		romversion = termid[strlen(termid)-2];
		if(strcmp(strrchr(getenv("DMDLD"),'/'), "32ld") == 0)
			(void)sprintf (cmd,
				"$DMDLD -v%c%c $DMDLIB/xt5620.j <&%d >&0 2>&0",
				romversion, hex_mode+'0', fromjerq);
		else
			(void)sprintf(cmd,
				"$DMDLD $DMDLIB/xt630.m <&%d >&0 2>&0",
				fromjerq);
	}

	/* download t630 */
	if (system(cmd)) {
		(void)fprintf(stderr,
			"\nxtdmd: Check if DMD is defined and exported.\n");
		exit(1);
	}

	(void)ioctl(tojerq, TCGETA, &cooked);
	(void)ioctl(fromjerq, TCGETA, &fcooked);
	raw.c_iflag = IGNBRK;
	if (hex_mode)
		raw.c_iflag |= IXON;
	raw.c_cflag = (cooked.c_cflag & (CBAUD | CLOCAL)) | CS8 | CREAD;
	raw.c_cc[VMIN] = 1;
	(void)ioctl(tojerq, TCSETAW, &raw);
	(void)ioctl(fromjerq, TCSETAW, &raw);

	(void)sleep(2);	/* kludge for data kit */

	start();
}



error(s)
	char *s;
{
#ifdef TIOCNXCL
	(void)ioctl(tojerq, TIOCNXCL, 0);
#endif
	(void)ioctl(tojerq, TCSETAW, &ttysave);
	(void)fprintf(stderr, "\nxtdmd: %s\n", s);
	exit(1);
}
