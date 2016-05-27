/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */

/*

static char _2Vsccsid[]="@(#)jx.c	1.1.1.6	(7/21/91)";
*/

/*
 * jx.c - plumbing and setup for system call interpreter
 *
 */

#include <stdio.h>
#include <termio.h>
#include "quiet.h"
#include <sys/jioctl.h>

#define NAMLEN  256

#define SENDTERMID "\033[c"
#define TERM_1_0 "\033[?8;7;1c"
#define TERMB_1_0 "\033[?8;7;2c"
#define TERM_1_1 "\033[?8;7;3c"
#define TERM_DMD "\033[?8;"
#define TERMIDSIZE 9
#define STR_EQUAL 0
#define ENC_CHK "\033[F"
#define ENC_LEN	4

#ifdef NO_TERMIO
#include <sgtty.h>
struct sgttyb sttymodes, sttysave, sttychars;
int lttysave, lttyraw;
struct sgttyb	ttysave,	/* save the state of tty */
		ttyraw;
#else
struct termio sttymodes, sttysave, sttychars;
struct termio	ttysave,	/* save the state of tty */
		ttyraw;
#endif
int jerq,used,ttyout,ttyerr;
short mpx;
extern char *ttyname();
extern short verno, subno;
char *getenv();
int zflag,dflag,pflag,fflag,nflag,Zflag,eflag;
char *stks;
char *malloc();
char *strrchr();
char	outfile[NAMLEN];
char	errfile[NAMLEN];
char	loader[NAMLEN];
char	sysint[NAMLEN];
int hex_mode;

char *pgm;

main(argc,argv)
int argc;
char *argv[];
{
	int fd;			/* File descriptor for getting rom version */
	char scr_buf[80];
	char termid[TERMIDSIZE+1];
	int c;
	extern int optind;
	extern char *optarg;

	int status;
	int i,pid,w;
	char **args;

	pgm = &argv[0][strlen(argv[0])];
	while(--pgm > &argv[0][0]) {
	    if (*pgm == '/') {
		pgm++;
		break;
	    }
	}

	while ((c = getopt(argc, argv, "dpzfnZ:V")) != EOF) {
		switch (c) {
		case 'V':
			printf("%s: version %d,%d\n", pgm, verno, subno);
			exit(0);
		case 'd':
			dflag++;
			break;
		case 'p':
			pflag++;
			break;
		case 'z':
			zflag++;
			break;
		case 'f':
			fflag++;
			break;
		case 'n':
			nflag++;
			break;
		case 'Z':
			Zflag++;
			stks = optarg;
			break;
		case '?':
			eflag++;
			break;
		}
	}


	if(eflag || optind >= argc)
		usage();
	else
		argv = &argv[optind-1];

	if (getdmd(loader,sysint))
		exit(1);
	*scr_buf = 0;
	jerq = -1;
	for (fd=0; fd<3; fd++) {	/* Find a file descriptor which */
		if (isatty(fd)) {	/* has not been redirected. */
			jerq = dup(fd);
			break;
		}
	}
	if (jerq == -1) {
		fprintf (stderr, "%s: can't get tty - stdin, stdout and sterr all redirected\n", pgm);
		exit (1);
	}
#ifdef USE_HOSTAGENT
	if ((mpx = (inlayers() > 0)) == 0)
#else
	if ((mpx = (ioctl(fd, JMPX, 0) != -1)) == 0)
#endif
	{
		fprintf (stderr, "%s: must be in layers to execute\n", pgm);
		exit(1);
	}

	pushmode();
	if ((args = (char **)malloc((argc+6)*sizeof(char *)))==0) /* 6 enough? */
		error("can't allocate argument area");
	if ((pid = fork()) == 0) {
		char **startargs=args;
		close(0);
		close(1);
		/* close(2); don't reset stderr because of -p and -d options */
		dup(jerq);dup(jerq);/* dup(jerq); */
		close(jerq);

#ifdef DMD5620
		args[0] = "32ld";
#else
		args[0] = "dmdld";
#endif
		if (*scr_buf) {
			args++;
			args[0] = scr_buf;
		}
		if(zflag){
			args++;
			args[0] = "-z";
		}
		if(dflag){
			args++;
			args[0] = "-d";
		}
		if(pflag){
			args++;
 			args[0] = "-p";
		}
		if(nflag){
			args++;
			args[0] = "-n";
		}
		if(fflag){
			args++;
			args[0] = "-f";
		}
		if(Zflag){
			args++;
			args[0] = "-Z";
			args++;
			args[0] = stks;
		}
		for (i = 1; argv[i] != NULL; i++)
			args[i] = argv[i];
		args[i] = NULL;

		execv(loader, startargs);
		fprintf(stderr, "%s: can't exec %s\n\r", pgm, loader);
		_exit(127);
	} else if(pid == -1)
		error("can't fork");
	while ((w = wait(&status)) != pid && w != -1)
		;
	if(status!=0)
	{
		popmode();
		exit(1);
	}
	ttyout = isatty(1);
	sprintf(outfile,"/tmp/jxout%d",getpid());
	ttyerr = isatty(2);
	sprintf(errfile,"/tmp/jxerr%d",getpid());
	if ((pid = fork()) == 0) {
		if (ttyout)
			freopen(outfile,"w",stdout);
		if (ttyerr)
			freopen(errfile,"w",stderr);
		if (jerq != 3) {
		    /* pass open tty file descriptor in fd 3 */
		    /* fd 0 might not be a tty (e.g. with "icon") */
		    close(3);
		    dup(jerq);
		    close(jerq);
		}
		args[0] = "sysint";
		if (hex_mode) {
			args[1] = "-h";
			args[2] = NULL;
		}
		else
			args[1] = NULL;
		execv(sysint,args);
		fprintf(stderr, "%s: can't exec %s\n\r",pgm, sysint);
		_exit(127);
	} else if(pid == -1)
		error("can't fork");
	while ((w = wait(&status)) != pid && w != -1)
		;
	terminal();
	sleep(2);
	if(status!=0)
		fprintf(stderr,"%s: %s didn't exit normally, exit code %d\n\r",
					pgm, sysint, status);
	if (!mpx)
		sleep(2);
	if (ttyout) {
		cat(outfile,stdout);
		unlink(outfile);
	}
	if (ttyerr) {
		cat(errfile,stderr);
		unlink(errfile);
	}
	return 0;
}

#ifdef NO_TERMIO
pushmode()
{
	ioctl(jerq, TIOCGETP, &sttysave);
	sttymodes = sttysave;
	ioctl(jerq, TIOCLGET, &lttysave);
	if (hex_mode) {
		sttymodes.sg_flags = ANYP | CBREAK;
		(void)ioctl(jerq, TIOCSETP, &sttymodes);
		lttyraw = lttysave;
		lttyraw |= LITOUT | LDECCTQ | PASS8;
		(void)ioctl(jerq, TIOCLSET, &lttyraw);
	} else {
		sttymodes.sg_flags = RAW | ANYP;
		(void)ioctl(jerq, TIOCSETP, &sttymodes);
	}
}

popmode()
{
	ioctl(jerq, TIOCSETP, &sttysave);
	ioctl(jerq, TIOCLSET, &lttysave);
}
#else
pushmode()
{
	ioctl(jerq, TCGETA, &sttysave);
	sttymodes.c_iflag = IGNBRK;
	if (hex_mode)
		sttymodes.c_iflag |= IXON;
	sttymodes.c_cflag = (sttysave.c_cflag & (CBAUD | CLOCAL)) | CS8 | CREAD;
	sttymodes.c_cc[VMIN] = 1;
	ioctl(jerq, TCSETAW, &sttymodes);
}

popmode()
{
	ioctl(jerq, TCSETAW, &sttysave);
}
#endif

terminal(){
#ifdef USE_HOSTAGENT
	Jterm();
#else
	ioctl(jerq, JTERM, 0);
#endif
	popmode();
}

error(s)
char *s;
{
	fprintf(stderr,"%s: %s\n", pgm, s);
	terminal();
	exit(1);
}

cat(s,ouf)
char *s;
FILE *ouf;
{
	register c;
	FILE *inf;
	if ((inf = fopen(s,"r")) != NULL) {
		while ((c = getc(inf)) != EOF)
			putc(c,ouf);
		fclose(inf);
	}
}

/*
 * getdmd - 	returns pointer to the loader and sysint
 * The choices for ldr are $DMDLD if *pgm == 'x', otherwise $DMD/bin/dmdld
 * The choices for sysint are $DMDLIB/../dmdtools/xsysint if *pgm == 'x',
 *	otherwise $DMD/lib/sysint.
 */
getdmd(ldr,sys)
char *ldr, *sys;
{
	char *dmd, *dmdld, *dmdlib, *s;

	if (*pgm == 'x') {
		/* xjx */
		dmdld = getenv("DMDLD");
		if (dmdld == (char *)0)
		{
			fprintf(stderr,"%s: DMDLD missing from environment\n", pgm);
			return(1);
		}
		strcpy(ldr,dmdld);

		dmdlib = getenv("DMDLIB");
		if (dmdlib == (char *)0)
		{
			fprintf(stderr,"%s: DMDLIB missing from environment\n", pgm);
			return(1);
		}
		strcpy(sys,dmdlib);
		s = strrchr(sys,'/');
		if (s == (char *)0)
		{
			fprintf(stderr, "%s: DMDLIB not properly set in environment\n", pgm);
			return(1);
		}
		*s = '\0';		/* truncate the last dir in pathname */
		strcat(sys,"/dmdtools/xsysint");
	}
	else {
		/* jx */

		dmd = getenv("DMD");
#ifdef DEFDMD
		if (dmd == (char *)0) 
			dmd = DEFDMD;
#endif
		if (dmd == (char *)0) 
		{
			fprintf(stderr,"%s: Must have DMD set in your environment\n", pgm);
			return(1);
		}
		strcpy(ldr,dmd);
#ifdef DMD5620
		strcat(ldr,"/bin/32ld");
#else
		strcat(ldr,"/bin/dmdld");
#endif
		strcpy(sys,dmd);
		strcat(sys,"/lib/sysint");
	}
	return(0);
}


usage()
{
	fprintf(stderr, "Usage: %s [-Vpdznf] [-Z stacksize] objectfile [arguments]\n", pgm);
	exit(2);
}

