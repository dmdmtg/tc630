/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


static char _2Vsccsid[]="@(#)32ld:version.c	1.2";

/* version.c - return the firmware version of the DMD terminal in
 *		the exit code.  Optionally, (if no -s option)
 *		interpret this code and display the Terminal Version.
 *		
 *	FW vers id
 *	(8;7;X)		Terminal Version
 *	-------		----------------
 *	0	-	unknown terminal
 *	1	-	this probably means that this program was not found
 *	2	-	1.0 terminal
 *	3	-	1.1 terminal
 *	4	-	1.2 terminal
 *	5	-	2.0 terminal
 *	6	-	1.1 terminal with 2.0 patch
 *
 * Add 10 to the FW version id if Encoding is indicated (e.g., 14
 * indicates 1.2 terminal with Encoding).
 *
 * Usage: 32version [-l] [-e] [-s]
 *
 *		-l	LAN check.  Find out if LAN Encoding is set.
 *			Forces the -e option.
 *		-e	ESC seq.  Use an escape sequence instead of ioctl.
 *			(Not useful in layers unless windowproc down there).
 *		-s	Silent.  Only supply return value.  Otherwise
 *			the corresponding ROM version (e.g. 2.0) is printed.
 *
 *
 */

#include <termio.h>
#include "quiet.h"
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <sys/jioctl.h>
#include "proto.h"	/* build version in the 32ld directory... */
#ifndef TIOCEXCL
#include <sgtty.h>
#endif

#if sun
/* for compatibility with older pseudo-tty based layers */
#ifndef USE_LIBWINROMVERSION
#define USE_LIBWINROMVERSION
#endif
#endif

#ifdef USE_HOSTAGENT
/* newer libwindows */
#ifndef USE_LIBWINROMVERSION
#define USE_LIBWINROMVERSION
#endif
#endif

#define SENDTERMID "\033[c"
#define TERM_DMD "\033[?8;7;"
#define TERMIDSIZE 9
#define ENC_CHK "\033[F"
#define	ENC_LEN	4
#define	HEX_MODE 10

extern char *getenv();
extern int Time_out();
extern int	optind;

#ifndef NO_AGENTVERSION
#ifndef USE_LIBWINROMVERSION
#define DADDR /* some agent.h's key off of this to define agentRectangle */
typedef struct Point {
	short	x;
	short	y;
} Point;

typedef struct Rectangle {
	Point origin;
	Point corner;
} Rectangle;
#include "agent.h"
extern union	bbuf	{
		struct	agentrect	ar;
		char	buf[MAXPKTDSIZE];
};
extern union	bbuf	ret;
#endif
#endif


int	forbgs;	
int	cntlfd;			/* FD for xt control channel */

int eflag, sflag, lflag, errflag;
int retval, ttyfd, debug;

#ifdef NO_TERMIO
struct sgttyb	ttysave,	/* save the state of tty */
		ttyraw;
#else
struct termio	ttysave,	/* save the state of tty */
		ttyraw;
#endif

main (argc, argv)
int argc;
char **argv;
{
	while ((retval = getopt (argc, argv, "desl")) != EOF)
		switch ( retval ) {
		case 'e':
			eflag++;
			break;
		case 's':
			sflag++;
			break;
		case 'l':
			lflag++;
			eflag++;
			break;
		case 'd':
			debug++;
			break;
		default:
			errflag++;
			break;
		}

	if (optind < argc || errflag) {
		fprintf(stderr,"usage: version [-les]\n");
		exit(1);
	}

	if (!eflag) {
		retval = byioctl();		/* may set eflag!!! */
		if (debug)	printf("retval(ioctl) = %d\n",retval);
	}
	if (eflag) {
		for (ttyfd = 0; ttyfd <= 2; ttyfd++)
			if (isatty(ttyfd))
				break;
		if (ttyfd > 2) {
			fprintf(stderr, "Not on a tty\r\n");
			return(1);
		}
		raw();
		retval = byescseq();
		cooked();
		if (debug)	printf("retval(escseq) = %d\n",retval);
	}
	
	if (!sflag) {
		interpret(retval);
	} else {
		exit(retval);
	}
}

int
byioctl() 
{
#ifdef NO_AGENTVERSION
	eflag++;
#else
    
#ifdef USE_LIBWINROMVERSION
	char versbuf[9];
#endif

	/* find rom version by agent ioctl  -- the most reliable way */

	if ( (cntlfd = openagent()) == -1 )  { 
		if (debug) {
			fprintf(stderr, "Error:  couldn't open control channel to determine firmware version.\nError number %d\n",errno);
		}
		eflag++;	/* resort to using an escape seq */
	}
#ifdef USE_LIBWINROMVERSION
	else if (forbgs = Romversion(cntlfd, versbuf, sizeof(versbuf)) == -1) {
		if (debug) {
			fprintf(stderr, "Error: Romversion returned %d (error %d).\n   Cannot determine firmware version.\n", forbgs, errno);
		}
		closeagent();
		eflag++;	/* resort to using an escape seq */
	} else {
		closeagent();
		return( versbuf[4]-'0' );  /* the last digit of x;y;z */
	}
#else
	else if( forbgs = ioctlagent(cntlfd, A_ROMVERSION,0,0,0,0,0)) {
		if (debug) {
			fprintf(stderr, "Error: A_ROMVERSION returned %d (error %d).\n   Cannot determine firmware version.\n", forbgs, errno);
		}

		eflag++;	/* resort to using an escape seq */
	} else {

		swab(ret.buf, ret.buf, sizeof(struct agentrect));

		if (debug) {
			fprintf(stderr,"Rom version id is ");
			for (forbgs=4; forbgs<9; forbgs++)
				fprintf(stderr,"%c",ret.buf[forbgs]);
			fprintf(stderr,"\n");
		}
		return( ret.buf[8]-'0' );  /* the last digit of x;y;z */
	}
#endif
#endif
	return (0);
}


int
byescseq()
{
	char	*dwnldflag;		/* for use in determining hex mode */
	short	romversion;		/* last digit of rom version id string */
	char scr_buf[30];
	char termid[TERMIDSIZE+1];
	int count;
	int lpindex;

	signal(SIGALRM, Time_out);
	alarm(4);
	write(ttyfd,SENDTERMID,strlen(SENDTERMID));
	count = 0;
	while (count == 0 || termid[count-1] != 'c') {
		lpindex = read(ttyfd,&termid[count],TERMIDSIZE-count);
		if(lpindex > 0)count += lpindex;
	}
	alarm(0);

	termid[count] = '\0';
	if (strncmp(termid,TERM_DMD,strlen(TERM_DMD)) != 0)
		return(0);				/* not a DMD */

	romversion = termid[strlen(termid)-2] - '0';

	if (romversion == 1 || romversion == 2)
		return(romversion);			/* 1.0 terminal */

	if (romversion == 3 || romversion == 4) {	/* 1.1 or 1.2 */

	    if (((dwnldflag = getenv("DMDLOAD")) != NULL) &&
		(dwnldflag[0] != NULL)) {
		    if(strcmp(dwnldflag, "hex") == 0)
			return(HEX_MODE + romversion);/* 1.1 terminal with encoding */
	    }
	    return(romversion);			/* 1.1 terminal without encoding */
	}
	else if (romversion == 5 || romversion == 6) {	/* 2.0 terminal w/wo patch */
		write(ttyfd,ENC_CHK,strlen(ENC_CHK));
		count = 0;
		while(count < ENC_LEN){
			lpindex = read(ttyfd,&scr_buf[count],ENC_LEN);
			if(lpindex > 0)count += lpindex;
		}
		if (scr_buf[2] == '1' )
			return(HEX_MODE + romversion);
		else {
			if (((dwnldflag = getenv("DMDLOAD")) != NULL) && (dwnldflag[0] != NULL)) {
				if (strcmp(dwnldflag, "hex") == 0)
				return(HEX_MODE + romversion);
			}
		}
		return(romversion);
	}
	return(0);
}


interpret (code)
{
	if (!sflag) {
		switch(code%10) {
		case 0:
			fprintf(stderr,"Not a DMD terminal.");
			break;
		case 2:
			fprintf(stderr,"DMD terminal version 1.0");
			break;
		case 3:
			fprintf(stderr,"DMD terminal version 1.1");
			break;
		case 4:
			fprintf(stderr,"DMD terminal version 1.2");
			break;
		case 5:
			fprintf(stderr,"DMD terminal version 2.0");
			break;
		default:
			fprintf(stderr,"Unknown DMD version (%d)",code%10);
		}
		fprintf(stderr,"\n");
		if (code >= HEX_MODE)
			fprintf(stderr,"Encoding enabled.\n");
	}
	exit(code);
}

Time_out ()
{
	if (debug)	fprintf(stderr,"Timed out.\n");
	cooked();
	interpret (0);
}

raw ()
{
#ifdef TIOCEXCL
	(void)ioctl(ttyfd, TIOCEXCL, 0);
#endif
#ifdef NO_TERMIO
	(void)ioctl(ttyfd, TIOCGETP, &ttysave); /* get the current state */
	ttyraw = ttysave;
	ttyraw.sg_flags = RAW | ANYP;
	(void)ioctl(ttyfd, TIOCSETP, &ttyraw);
#else
	(void)ioctl(1, TCGETA, &ttysave); /* get the current state */
	ttyraw.c_iflag = IGNBRK;
	ttyraw.c_cflag = (ttysave.c_cflag & CBAUD) |
			 (ttysave.c_cflag & CLOCAL) | CS8 | CREAD;
	ttyraw.c_cc[VMIN] = 1;
	(void)ioctl(1, TCSETAW, &ttyraw);
#endif
}

cooked ()
{	
#ifdef TIOCNXCL
	(void)ioctl(ttyfd, TIOCNXCL, 0);
#endif
#ifdef NO_TERMIO
	(void)ioctl(ttyfd, TIOCSETP, &ttysave);
#else
	(void)ioctl(1, TCSETAW, &ttysave);
#endif
}

/* swap bytes if necessary. target is big endian. */
swab(a, b, n)
	register char *a, *b;
	register n;
{
	register short space;

	n/=2;	/* n in bytes */
	while(n--){
		space = *(short *)a;
		*b++= space >> 8;  /* most significant byte first */
		*b++= space;
		a+=2;
	}
}

