/*       Copyright (c) 1987, 1989 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */

/*
**	dmdld - Downloader for the 630 MTG - version 2.1
**
**	This program is used for:
**	   . mux download into layers window,
**	   . nonmux download into a non-layers window,
**	   . takeover download with or without relocation,
**	   . new takeover download that can download over Starlan
**	     and in a window.
**
**	Relocation will be performed if necessary.
**
**	This program is based on the 32ld for the 5620 DMD 
**	and 68ld for the Blit.
*/

/* load file must have text, data, and bss in following positions.
 * other noload sections following do not matter
 */
#define TEXTSECT 0
#define DATASECT 1
#define BSSSECT 2
#define ALLSECT 3

#define MINSTACK 2048		/* minimum allowable stack size */

#define JPATH_ACCESS 1
#define JPATH_OPEN 2

#include <fcntl.h>
#include <termio.h>
#include "quiet.h"
#include "filehdr.h"
#include "scnhdr.h"
#include "reloc.h"
#include <stdio.h>
#include <errno.h>
#include <sys/jioctl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef TIOCEXCL
#include <sgtty.h>
#endif

#include "load.h"
#include "proto.h"

#ifdef NO_TERMIO
/*
 * Handshaking doesn't work well with NO_TERMIO so don't use it if possible.
 */
#ifndef REALLY_NO_TERMIO
#undef NO_TERMIO
#endif
#endif

char Load_str[] = LOADTEMPLATE;		/* default: binary & nonmux */
int Loadtype = BINARY_LOAD;		/* default: binary */

char Usage[] =
"Usage: %s [-dfpnzNTVWx] [-Z stacksize] [-m maxpktsize] objectfile [arguments]\r\n";

char *name, *e2s1, *e2s2;
struct filehdr fileheader;
struct scnhdr secthdrs[NSECTS];
unsigned long imagesize;	/* bytes to be downloaded */
unsigned long relocinfosize;	/* size of relocation info in *.m file */
unsigned long reloc_offset;	/* address to relocate to */
unsigned long physaddr;		/* current physical address of file */

int peidsect = -1;		/* peid section (if it exists) */
int cprssect = -1;		/* cprs section (if it exists) */

 
#ifdef NO_TERMIO
struct sgttyb	ttysave,/* save the state of tty */
		ttyraw;
int	lttysave;	/* save state of local flags */
int	lttyraw;
#else
struct termio	ttysave,/* save the state of tty */
		ttyraw;
#endif
int	obj;		/* File descriptor for object file */
int	mpx;		/* Running under layers */
long	location;
char	file[1024];	/* Name of file */
int	nargchars;	/* Number of characters, including nulls, in args */
long	longbuf[3];
int	debug;		/* Show sizes etc. on stderr */
int	psflag;		/* Print error detection statistics */
short	maxpktdsize;
int 	errflag;	/* cannot access or open download file */
int	Noproto;	/* run nonmux download without protocol */
int	maxpkt = -1;	/* maximum packet size option for takeover proto load */
int	hexmode;	/* use hex encoding */

/* argument flags */
int	rflag;		/* true if relocation needed */
int	zflag;		/* Do a JZOMBOOT */
int	fflag;		/* forced a download */
int	takeover;	/* takeover donwload */
int	wtkload;	/* window takeover download */
int	Zflag;		/* stack siZe */

int	handshakeflag;	/* download handshake patch and wait for it */
char	*shakebuf;	/* pointer to buffer of code superceded by patch */
int	sizeshakebuf;	/* size of shakebuf */
extern	unsigned short Handshakecode[]; /* code of patch to download */
extern	int SizeHandshakecode; /* size of the patch */

int	booted;

/* BLIT - tmp for stack size command line option */
/* 2048 is STKSIZE from dmdproc.h - would rather do this symbolically
eventually */
unsigned long stksize = MINSTACK;


/* Terminal speed dependency:
** This is used for nonmux download:
** Nonmux download mimics the xt error correcting protocol, however
** it sets the maximum packet data size to up 120 bytes while the 
** true xt only sets up to 32 bytes.
*/
short speeds[16] = {
	 1,	5,	7,	10,	13,	15,	20,	30,
	60,	120,	180,	240,	480,	960,	1920,	1
};

/* NOTE: these must all be even numbers! (because of byte-swapping) */
unsigned char sizes[16] = {
	 16,	16,	16,	16,	16,	16,	16,	16,
	 16,	32,	32,	56,	56,	120,	120,	16
};

/* timeout_id -	Time out during identification step (nonmux only)
*/
void
timeout_id()
{
	error(0, "Timeout: not a windowing terminal or terminal failed to respond", (char *)0);
}

main(argc, argv)
register int argc;
register char *argv[];
{
	register char *cp;	/* scratch char pointer */
	register int i;		/* scratch register */
	char c;			/* no register, &c will be used */
	static char ebuf[5];	/* terminal's response to encoding inquiry */
	extern char *optarg;
	extern int optind;
	int sectstate;		/* for recording what sections have been read */

	extern short verno, subno;

/*	
**	Get the command line arguments
**
**	If the diagnostic flags (-d, -p) are given, 
**	the stderr has to be directed, otherwise the
**	output to it will be mistaken as downloading data.
**
**	The takeover download is initiated by these flags:
**		-T  : download absolute file, relocate if necessary
**		-N : ignore relocation, force download at absolute address
*/
	/* get the current state of tty line */
        /* set here so error() can restore it */
#ifdef NO_TERMIO
	(void)ioctl(1, TIOCGETP, &ttysave);
	(void)ioctl(1, TIOCLGET, &lttysave);
#else
	(void)ioctl(1, TCGETA, &ttysave);
#endif

	name = *argv;

	while ((i = getopt(argc, argv, "dpzfZ:TNVHnWm:xr")) != EOF) {
	    switch (i) {
		case 'd':		/* diagnosis needed */		
			debug++;
			break;
		case 'p':		/* protocol statistics needed */
			psflag++;
			debug++;	/* force debug msg's with p stats */
			break;
		case 'f':
			fflag++;	/* force download even when PEID do not match */
			break;
		case 'm':
			if ((*optarg < '0') || (*optarg > '9'))
				error(0,"Bad max packet size argument: %s",
							optarg);
			maxpkt = atoi(optarg);
			if (((maxpkt != 0) && (maxpkt < 16)) ||
						(maxpkt > MAXPKTDSIZE)) {
				char msg[80];
				sprintf(msg, "%s %d. %s %d and %d",
					"Bad max packet size:",maxpkt,
					"Must be between",16,MAXPKTDSIZE);
				error(0,msg,"");
			}
			break;
		case 'r':
			maxpkt = 0;
			break;
		case 'x':
			hexmode++;
			break;
		case 'z':		/* boot into a zombie state */
			zflag++;
			break;
		case 'T':			/* takeover download */
			Load_str[2] = '4';
			takeover++;		/* flag true */
			break;
		case 'N':
			rflag = -1;		/* no relocation */
			Load_str[2] = '4';
			takeover++;		/* flag true */
			break;
		case 'V':			/* version number */
			fprintf (stdout, "%s: version %d.%d\n", name, verno, subno);
			exit(0);
		case 'Z':			/* stack size overide */
			if ((*optarg < '0') || (*optarg > '9'))
				error(0,"Bad stack size argument: %s",optarg);
			stksize = (unsigned long) atoi(optarg);
			if (stksize < MINSTACK) stksize = MINSTACK;
			Zflag++;
			break;
		case 'W':		/* UNDOCUMENTED: window takeover download */
			wtkload++;
			break;
		case 'H':		/* UNDOCUMENTED: no handshake */
			handshakeflag = -1;
			break;
		case 'n':			/* null operation */
			break;
		case '?':
			errflag++;
			break;
	    }
	}

	if (optind >= argc || errflag) {
		fprintf (stderr, "%s: ", name);
		fprintf (stderr, Usage, name);
		exit (2);
	}

	if (takeover > 1) {
		fprintf (stderr, "%s: -T and -N exclude each other\r\n", name);
		exit (1);
	}

	if (debug)
		fprintf (stderr, "%s: version %d.%d\n", name, verno, subno);

	if (!hexmode) {
		char *enc_env = getenv("DMDLOAD");
		if (enc_env != NULL) {
			if (strncmp(enc_env, "hex", 3) == 0) {
				hexmode = 1;
				if (debug)
					fprintf (stderr,
						"DMDLOAD=hex recognized\n");
			}
			else if (strncmp(enc_env, "regular", 7) == 0) {
				maxpkt = 0;
				if (debug)
					fprintf (stderr,
						"DMDLOAD=regular recognized\n");
			}
		}
	}

	if (hexmode && (maxpkt == -1)) {
		/* Hex encoding implies using the xt protocol for standalone */
		/*  downloads.  That is indicated with a maxpkt size >= 0.  */
		maxpkt = 0;
		if (debug)
			fprintf (stderr, "hex encoding implies -m 0\n");
	}

	if (wtkload) {		/* remove any reference to "takeover" */
		if (!takeover) {
			fprintf (stderr,
			         "%s: -W needs -T or -N\r\n", name);
			exit (1);
		}
		Load_str[2] = '5';
	}

#ifdef DMDLDBUGSTUFF
	/* This was causing some problem with downloads stoping halfway
	** through. Since I no longer need the code, I just commented
	** it out. A setbuf for no buffering of stderr should be added
	** below -bob
	*/
	/* This is a way of passing args to dmdld through applications
	** like dmdpi and jim that exec dmdld internally. If the shell
	** variable DMDLDBUG is set and exported to:
	**
	**	p:/path/name  turn on -p and append to /path/name
	**	d:/path/name  turn on -d and append to /path/name
	**	z	      turn on -z
	**
	** This might also just be handier than having to redirect
	** stderr all the time.
	*/
	{har *value;
	FILE *tmpfile, *fopen();
	if( (value=getenv("DMDLDBUG")) != NULL ) {
		switch(value[0]) {
		case 'p':		/* protocol statistics needed */
			psflag++;
			/* no break - p implies d */
		case 'd':		/* diagnosis needed */		
			debug++;
			if( (tmpfile=fopen(&value[2], "a")) == NULL)
				error(1, "Cannot append file %s pointed to by $DMDLDBUG", &value[2]);
			fclose(tmpfile);
			freopen(&value[2], "a", stderr);
			break;
		case 'z':		/* boot into a zombie state */
			zflag++;
			break;
		default:
			error(0, "Bad $DMDLDBUG string", "");
		}
	}
	}
#endif

	++rflag;	/* relocate on all cases except takeover w/o relocation */

/*
**	Check the accessibility of the file to be downloaded.
**	We just remember the error here but will wait until 
**	finding out if the invalid file is not already cached
**	in the terminal (see load()).
*/
	errflag = 0;	/* redundant, but safe */
	if (jpath(argv[optind], JPATH_ACCESS, 4)!=0)
		errflag = 1;

	if (!errflag) {
		obj = jpath(argv[optind], JPATH_OPEN, 0);
		if (obj<0)
			errflag = 2;
	}

/*
**	Reads the headers for the m68a.out
**	file and stores the data read into the global
**	structures declared for this purpose
**
**	Unlike m32a.out, m68a.out has no aouthdr 
**	(optional header information)
*/

	if (!errflag) {
		Read ((char *) &fileheader, sizeof(struct filehdr));
		if (fileheader.f_magic!=MC68MAGIC)
			error2("'%s' is not a MC68000 family a.out", file);

		if (fileheader.f_nscns > NSECTS)
			error2("%s exceeds max number of sections", file);

		if(!(fileheader.f_flags&F_EXEC))
			error2("%s is not an executable file", file);


		if(rflag && fileheader.f_flags&F_RELFLG)
			error2("%s has been stripped of relocation information", file);
	
		imagesize = relocinfosize = 0;
		physaddr = NOTINIT;
		sectstate = TEXTSECT;
		for (i = 0 ; i < (int) fileheader.f_nscns && i < NSECTS && !errflag; ++i) {
				
			/* make sure first 3 sections are text, data,
			** and bss in that order, IF we are not
			** running a takeover download.
			**
			** Takeover download may start with .vector section.
			*/
		
			Read ((char *) &secthdrs[i], sizeof(struct scnhdr));
			if(strequal(secthdrs[i].s_name,".text"))
				if(sectstate == TEXTSECT) {
					sectstate = DATASECT;
					secthdrs[i].s_flags &= ~STYP_DSECT;
				}
				else
					error2("%s has text section out of order", file);
			else if(strequal(secthdrs[i].s_name,".data"))
				if(sectstate == DATASECT) {
					sectstate = BSSSECT;
					secthdrs[i].s_flags &= ~STYP_DSECT;
				}
				else
					error2("%s has data section out of order", file);
			else if(strequal(secthdrs[i].s_name,".bss"))
				if(sectstate == BSSSECT) {
					sectstate = ALLSECT;
					secthdrs[i].s_flags &= ~STYP_DSECT;
				}
				else
					error2("%s has bss section out of order", file);
			else if(strequal(secthdrs[i].s_name,".peid"))
				peidsect = i;
			else if(strequal(secthdrs[i].s_name,".cprs"))
				cprssect = i;
			/* else					    */
			/*	secthdrs[i].s_flags &= ~STYP_DSECT; */

			if (checksect (&secthdrs[i])) {
				imagesize += secthdrs[i].s_size;
				relocinfosize += secthdrs[i].s_nreloc * sizeof(struct reloc);

				/* Find the physical address of the first section to
				** be downloaded. We assume all downloaded sections
				** are consecutive in memory.
				**
				** For mux and nonmux download, this value is probably
				** 0, since the file needs to be relocated, therefore
				** it is not relevant.
				**
				** For takeover download, the file has been physically
				** link-loaded, so the value is probably non-zero.
				** This value is used for takeover download to check if
				** the start of the download is below the Free_RAM address
				** proposed by the terminal.  
			 	*/
				if (physaddr == NOTINIT)
					physaddr = secthdrs[i].s_paddr;
			 }
		}
		if(sectstate != ALLSECT)
			error2("%s miss text, data, and/or bss section", file);
	}

	if(cprssect > -1) {
		/* no relocation when compression is on */
		rflag = 0;
	}

	if (handshakeflag == -1)
		handshakeflag = 0;
	else if (rflag && /* !zflag && */ !takeover) {
		/* do handshake if relocating, not zombie boot, */
		/*    and not takeover			        */
		handshakeflag = 1;
	}

/*
**	Initialize the terminal parameters for downloading
** 	Terminal input condition: ignore BREAK, same baud, same line status,
**	8-bit character, and receiver enabled
*/

#ifdef TIOCEXCL
	(void)ioctl(1, TIOCEXCL, 0);
#endif
#ifdef NO_TERMIO
	ttyraw = ttysave;
	ttyraw.sg_flags = RAW | ANYP;
#else
	ttyraw.c_iflag = IGNBRK;
	ttyraw.c_cflag = (ttysave.c_cflag & CBAUD) | 
			 (ttysave.c_cflag & CLOCAL) | CS8 | CREAD;
	ttyraw.c_cc[VMIN] = 1;
#endif


	/* There is a bug in the SVR2 xt driver:
	** TCSETAW is supposed to make dmdld wait for the output
	** to drain, well it doesn't. SVR3 fixes the bug though.
	** The consequence is that if before issuing dmdld, some
	** I/O command (cat, echo, etc..) is executed, the output
	** of the command is not drained and when dmdld starts up,
	** the last data from the command may be mistaken by the
	** terminal as information from dmdld, and God knows where
	** we end up.
	** This problem also exists with pseudo-ttys on at least Suns.
	** Quick solution: a sleep. 
	*/
#ifdef NO_TERMIO
	(void)ioctl(1, TIOCSETP, &ttyraw);	/* set new terminal parameters */
#else
	(void)ioctl(1, TCSETAW, &ttyraw);	/* set new terminal parameters */
#endif
	/* sleep(2); If needed, the user can do it himself. */

/*
**	Terminal environment dependencies: mux or nonmux?
**
**	If it is mux, we don't have to worry. The device drivers for xt and hex
**	encoding should be there when layers(1) is booted, so those things are
**	transparent to dmdld.
**
**	If it is nonmux, we have to implement the xt-like error correcting protocol
**	ourselves. In addition, we have to inquire the terminal if it has hex
**	encoding set or not.
*/	
#ifdef USE_HOSTAGENT
	mpx = (inlayers() > 0);
#else
	mpx = (ioctl(1, JMPX, 0) != -1);
#endif

	/* Set up a timeout for reading things from the terminal (mux, nonmux)
	** This is needed if the terminal is not a 630 such as 5620 etc..
	*/
	signal (SIGALRM, timeout_id);

	if (!mpx) {		/* nonmux */
		if(takeover && errflag)
			if (errflag == 1)
				error (0, "cannot access '%s'", file);
			else if (errflag == 2)
				error (0, "cannot open '%s'", file);
			else if (errflag == 3)
				error (0, e2s1, e2s2);
		write (1, ENC_CHK, 3);		/* inquire encoding */
		if (debug) {
			fprintf (stderr,"\nSend request encoding sequence: ");
			cp = ENC_CHK;
			while (*cp != '\0')
				fprintf (stderr, "<%x>", *cp++);
			fprintf (stderr, "\nReceive from terminal: ");
		}

		alarm (15);			/* 10 secs should be enough */
		for (i=0; i<4; ++i) {
			ebuf[i] = (char)getchar();
			if (debug)
				fprintf (stderr, "[%x]", ebuf[i]);
		}
		alarm (0);			/* cancel alarm */
		if (hexmode || (ebuf[2] == '1')) {
			/* Hex encoding set */
			Load_str[4] = '2';
			Loadtype = HEX_LOAD;
#ifdef NO_TERMIO
			ttyraw.sg_flags = ANYP | CBREAK;
			(void)ioctl(1, TIOCSETP, &ttyraw);
			lttyraw = lttysave;
			lttyraw |= LITOUT |LDECCTQ | PASS8;
			(void)ioctl(1, TIOCLSET, &lttyraw);
#endif
			if (debug)
				fprintf (stderr, "\nHex encoding on");
		}


		/* init terminal side for nonmux download */
		write (1, Load_str, 6);

		if (debug) {
			fprintf (stderr,"\n\nSend download escape sequence:\n");
			cp = Load_str;
			while (*cp != '\0')
				fprintf (stderr, "<%x>", *cp++);
		}

	 	/* init protocol (host side) */
		if (maxpkt > 0)
			maxpktdsize = maxpkt;
		else {
#ifdef NO_TERMIO
			maxpktdsize = min(sizes[ttysave.sg_ospeed&017],
						(long)MAXPKTDSIZE);
#else
			maxpktdsize = min(sizes[ttysave.c_cflag&CBAUD],
						(long)MAXPKTDSIZE);
#endif
		}
		if (debug)
			fprintf (stderr, "\nMaximum packet size for download: %d", maxpktdsize);
		if (takeover && !wtkload)		/* old takeover scheme */
			maxpktdsize -= PKTASIZE;	/* address field */

#ifdef NO_TERMIO
		pinit(speeds[ttysave.sg_ospeed&017], maxpktdsize);
#else
		pinit(speeds[ttysave.c_cflag&CBAUD], maxpktdsize);
#endif
		booted++;	/* for nonmux, the terminal side has been booted */

		/* wait for terminal to send back an "a" 
		** to inform the host it is ready for the download 
		**
		** if the terminal can support no protocol download
		** in non-layers, it will send back a "b" first, then
		** the "a".
		*/
		c = '\0';
		while (c != 'a' && c != 'b')
			Uread (&c, 1);

		if (c == 'b') {
			if (debug)
				fprintf (stderr, "\nReceive non-error correcting character: %c", c);
			if (maxpkt != -1) {
				if (debug)
					fprintf (stderr, " (ignored because of -m)");
			}
			else
				Noproto = 1;

			while (c != 'a')
				Uread (&c, 1);
		}

		if (debug)
			fprintf(stderr,"\nReceive acknowledge character: %c",c);
	}
		
	else {			/* mux */
	    maxpktdsize = DATASIZE;
	    if (Noproto)
		Noproto = 0;	/* does not make any sense, but it is 
				** .. not worth to generate an error.
				** .. let's silently ignore it.
				*/

	    if (takeover)	/* no takeover download in mux */
		error(0,"%s [-W][-T][-N]: takeover download not supported in layers", name);
	}


/*
**	Starts the download
**
**	The mux download is running on top of the xt protocol.
**	For the nonmux download, we have to fake the xt protocol on sending
**	in order to get an error correcting protocol
*/ 
	boot();			/* ask the terminal to start the boot process */
	load (argc-optind, &argv[optind]);	/* pump, pump, pump */

/*
**	Download termination
*/

#ifdef NO_TERMIO
	/*
	 * Need to restore the original terminal settings before waiting
	 *  for the handshake because changing the settings flushes the
	 *  input buffer.  The handshake character is a newline so it
	 *  doesn't need to be in raw mode.  Unfortunately the newline
	 *  will then be echoed.
	 */
	(void)ioctl(1, TIOCSETP, &ttysave);
	(void)ioctl(1, TIOCLSET, &lttysave);
#endif

	if (handshakeflag > 1) {
		if (debug) {
			fprintf(stderr, "\nwaiting for completion handshake\n");
			fflush(stderr);
		}
		if (read(0, &c, 1) < 0) {
			if (debug)
				perror("error reading handshake byte");
		}
		else if (debug) {
			if (c == '\n')
				fprintf(stderr, "received handshake\n");
			else
				fprintf(stderr,
					"invalid handshake character: %c\n", c);
		}
	}

	/*
	** Give a little time for the downloaded program to initialize
	*/
	sleep (2); 

#ifndef NO_TERMIO
	/* recover original terminal setting */
	(void)ioctl(1, TCSETAW, &ttysave);
#endif
#ifdef TIOCNXCL
	(void)ioctl(1, TIOCNXCL, 0);
#endif

	if ((!mpx || Noproto) && psflag) /* print diagnostics if requested */
		pstats(stderr);

	return(0);
}


/*
**	Look for the object file to be downloaded.
*/
jpath (f, fun, a)
register char *f;
{
	char *getenv(), *strcpy();
	register char *jp, *p;
	register o;
	if (*f != '/' && strncmp(f, "./", 2) && strncmp(f, "../", 3) && 
	    (jp=getenv("JPATH"))!=0){
		while(*jp){
			for(p=file; *jp && *jp!=':'; p++,jp++)
				*p= *jp;
			if(p!=file)
				*p++='/';
			if(*jp)
				jp++;
			(void)strcpy(p, f);
			if (fun == JPATH_OPEN)
				o=open(file, a);
			else
				o=access(file, a);
			if(o!=-1)
				return o;
		}
	}
	if (fun == JPATH_OPEN)
	    return(open(strcpy(file, f), a));
	else
	    return(access(strcpy(file, f), a));
}

/* rpath -	Remove the path name from the file
**
**	It is used to send down to the terminal to see if such a file
**	is in the cache. Only the file name is used.
*/
char *
rpath (s)
register char *s;
{
	static char jfile[1024];
	register i;

	for (i=0; *s; s++) {
		if (*s == '/')
			i = 0;
		else
			jfile[i++] = *s;
	}
	jfile[i] = '\0';
	return (&jfile[0]);
}





/* boot -	Inform the terminal to start the boot process
**
**	For mux, use ioctl commands.
**	For nonmux, send a control packet to demux() to start the boot
**	process on the nonmux window which fakes as xt channel 1.
*/
boot()
{
	if (!takeover) {
		if (mpx) {		/* mux */
#ifdef USE_HOSTAGENT
			if (zflag) {
				if (Jzomboot() < 0)
					error(0, "Jzomboot failed", 0);
			}
			else {
				if (Jboot() < 0)
					error(0, "Jboot failed", 0);
			}
#else
			if (ioctl(1, zflag?JZOMBOOT:JBOOT, 0) < 0)
				error(0, "Jboot failed", 0);
#endif
		}
		else {	/* nonmux */
			if (debug)
				fprintf (stderr, "\n\nSend boot command: ");
			nonmuxioctl (zflag?JZOMBOOT_CHAR:JBOOT_CHAR, 1);
		}
#ifdef NO_TERMIO
		(void)ioctl(0, TIOCFLUSH, 0);	/* throw away type-ahead! */
#else
		(void)ioctl(0, TCFLSH, 0);	/* throw away type-ahead! */
#endif
	}

	booted++;
}

/* load -	This is the downloader
*/
load (argc, argv)
int argc;
char *argv[];
{
	register char *argp;
	register i, n;
	register int isin = 0;

	unsigned char tmp;
	long largc, getpeid();
	long bssstart, bssend;
	char *jfile[1];
	char c;
	char *bldargs();

	if(debug)
		fprintf(stderr, "\n\nDownload type: %s %s %s %s %s",
			mpx ? "layers" : "nonlayers",
			wtkload ? "window" : "",
			takeover ? "takeover" : "window",
			rflag ? "with relocation" : "without relocation",
			(!mpx && Noproto) ? ", non-error correcting" : "");

	if (wtkload) {

		largc = (long)0;
		if (Noproto)
			largc |= 0x20000;
		if (debug)
			fprintf (stderr, "\n\nSend version id: %lx", largc);
		Uwriteswap ((char *)&largc, 4);
		Uread (&tmp, 1);
		if (debug)
			fprintf (stderr, "\nRead terminal response: %c", tmp);
		switch (tmp & 0x7f) {
		case 'y':	/* good */
			break;
		case 'n':
			error (0, "'%s' is not compatible with terminal", argv[0]);
		}
	}

	if (!takeover) {

		largc = (long)0;
		if (debug)
			fprintf (stderr, "\n\nSend sync number: %lx", largc);
		Uwriteswap ((char *)&largc,4);

		/* Send the "clipped" name of the file to be download to the
		** terminal to see if that program is in the application cache.
		*/
		jfile[0] = rpath (argv[0]);
		argp = bldargs(1,jfile);
		if (debug)
			fprintf (stderr, "\n\nSend filename: '%s'", jfile[0]);
		for (i=nargchars; i>0; i-=maxpktdsize, argp+=n) {
			n = min (i, maxpktdsize);
			Uwrite (argp, n);
		}

		/* Read the terminal's response:
		** Y: the application is in the cache and bootable (8;8;6 ROMS)
		** T: the application is in the cache and bootable (post 8;8;6 ROMS)
		** F: the application is found in the cache but is not bootable:
		**    - no memory to do the boot,
		**    - the application is not shared and is currently used,
		**    - the cartridge application has wrong PEID
		** N: the application is not in the cache
		** other: wrong terminal
		**
		** In 8;8;6 ROMS, when we see that the application is in the cache,
		** we boot it right away and have dmdld exits. The problem is that
		** the user also wants argv's to be downloaded. This problem should
		** be solved in post-8;8;6 fw's, but let's put the remedy in here
		** first so we don't have to release a new dmdld.
		*/
		Uread (&tmp, 1);
		if (debug)
			fprintf (stderr, "\nRead terminal response: %c", tmp);
		switch (tmp & 0x7F) {
			case 'Y':		/* application is in cache */
				return;		/* boot without download */

			case 'T':		/* application is in cache */
				isin = 1;
				break;

			case 'F':		/* found but cannot boot */
			case 'N':		/* does not find in the cache */ 
				/* see if the application is accessible in the host */
				if (errflag == 1)	/* remember the errflag? */
					error (0, "cannot access '%s'", argv[0]);
				else if (errflag == 2)
					error (0, "cannot open '%s'", file);
				else if (errflag == 3)
					error (0, e2s1, e2s2);
				break;

			default:		/* not anything known */
				error (0, "dmdld is not compatible with terminal", (char *)0);
		}


		/* If the forced download is in effect, use PEID 0
		** which works with any released 630 firmware.
		** Otherwise, read the PEID from file.
		**
		** NOTE: 
		** PEID = 0 : works with all fw releases
		** PEID = 1 : works with all fw releases.
		** PEID = 2 : official PEID of 8;8;6 ROMS.
		**
		** For the enhanced dmdld, the PEID is also used to pass to the
		** terminal the following information by setting the corresponding
		** bits on the high word of PEID:
		**	0x10000: compressed download
		**	0x20000: nonmux, no-protocol download
		*/
		if (fflag || isin) 
			largc = 0x0;
		else 
			largc = getpeid();

		if (cprssect > -1)
			largc |= 0x10000;
		if (Noproto)
			largc |= 0x20000;

		if (debug) 
			fprintf (stderr, "\n\nSend programming environment ID: %lx", largc);
		Uwriteswap ((char *)&largc,4);

		if(cprssect != -1) {
			char getcprsid();

			tmp = getcprsid();
			Uwrite(&tmp,1);
			if (debug)
				fprintf (stderr, "\nSend compression ID: 0x%x", tmp);
		}

		/* Read the terminal's response:
		** y: PEIDs of download application and terminal match
		** n: PEIDs of download application and terminal mismatch
		** o: compression not compatible
		*/
		Uread(&tmp, 1);
		if (debug)
			fprintf (stderr, "\nRead terminal response: %c", tmp);

		switch( tmp&0x7F ) {
		case 'n':
			error (0, "'%s' is not compatible with terminal", argv[0] );
			break;
		case 'o':
			error (0, "compression algorithm not compatible with terminal", (char *)0l);
			break;
		}

		if(cprssect > -1) {
		/* an addition to the protocol for compressed files */
			largc = secthdrs[cprssect].s_size - 1;
			Uwriteswap(&largc, 4);
			if (debug)
				fprintf(stderr, "\n\nSend size of compressed text: 0x%lx", largc);
			largc = secthdrs[cprssect].s_nreloc;
			Uwriteswap(&largc, 4);
			if (debug)
				fprintf(stderr, "\n\nSend number of relocation entries: 0x%lx", largc);
		}
		/* send info about arguments */
		argp = bldargs(argc, argv);
		largc = argc;
		if (debug)
			fprintf (stderr, "\n\nSend number of arguments: ");
		Uwriteswap ((char *)&largc,4);
		largc = nargchars;
		if (debug)
			fprintf (stderr, "\n\nSend total size of arguments: ");
		Uwriteswap ((char *)&largc,4);

		if (isin) {	/* don't have to download, application is in cache */
			if (debug)
				fprintf (stderr, "\n\nSend program arguments: ");
			for (i = nargchars; i > 0; i -= maxpktdsize, argp += n) {
				n =  min (i, maxpktdsize);
				Uwrite (argp, n);
			}

			return;		/* early exit for load() */
		}

		if (handshakeflag)
			/* actually going through with handshake */
			handshakeflag = 2;

		for (i = 0; i < (int) fileheader.f_nscns; ++i) {
		    if ( (strequal(secthdrs[i].s_name,".text")) ||
		         (strequal(secthdrs[i].s_name,".data")) ||
		         (strequal(secthdrs[i].s_name,".bss"))     ) {
			largc = secthdrs[i].s_size;
			if (handshakeflag) {
			    if (strequal(secthdrs[i].s_name,".text")) {
				if (largc < SizeHandshakecode)
					error(0, ".text must be at least %d bytes long for handshake mode",
							SizeHandshakecode);
				sizeshakebuf = SizeHandshakecode +
						(4 * sizeof(short));
				shakebuf = malloc(sizeshakebuf);
				if (shakebuf == 0)
					error(0, "can't allocate handshake buf", "");
			    }
			    else if (strequal(secthdrs[i].s_name,".data")) {
				largc += sizeshakebuf;
				if (debug)
				    fprintf(stderr,
					"\nAdding %d bytes to .data for handshake code",
						    sizeshakebuf);
			    }
			    else if (strequal(secthdrs[i].s_name,".bss")) {
				if (sizeshakebuf > largc) {
				    /*
				     * The handshake code first copies itself
				     *  to the after bss and we need to make
				     *  sure that doesn't overlap with the
				     *  stuff in the 'shakebuf' which is
				     *  loaded at the end of .data.
				     */
				    if (debug)
					fprintf(stderr,
					    "\nAdding %d bytes to .bss to accomodate handshake code",
							sizeshakebuf - largc);
				    largc += sizeshakebuf - largc;
				}
				bssstart = secthdrs[i].s_paddr;
				bssend = bssstart + largc;
			    }
			}
			if (debug)
				fprintf (stderr, "\n\nSend size of header %d: ", i);
			Uwriteswap ((char *)&largc, 4);
		    }
		}
		if (!Zflag && cprssect == -1) getstacksize();
		if(debug)
			fprintf(stderr, "\n\nStack size is %ld", stksize);
		Uwriteswap ((char *)&stksize, 4);
	}

	if (debug)
		fprintf (stderr, "\n\nRead download address: ");
	reloc_offset = readaddr(); /* the terminal sends us the address to download */
	if(reloc_offset == 0)
		error(0, "no memory in terminal", (char *)0);

	if (handshakeflag) {
		/*
		 * Fill in extra 2 pointers at the end of the shakebuf:
		 *  a pointer to the real bss start and the real end.
		 */
		bssstart += reloc_offset;
		bssend += reloc_offset;
		n = SizeHandshakecode/sizeof(short);
		((short *)shakebuf)[n++] = (short) (bssstart >> 16);
		((short *)shakebuf)[n++] = (short) bssstart;
		((short *)shakebuf)[n++] = (short) (bssend >> 16);
		((short *)shakebuf)[n++] = (short) bssend;
	}
	if (Noproto) { 			/* nonmux, no-protocol */
		if (debug)
		    fprintf (stderr, "\n\nInform terminal to remove protocol module:");

		Uwrite ("x", 1);	/* tell terminal to get rid of protocol */

		/* Now wait for the terminal to send back an 'x' to
		** indicate it has removed the xt I/O block. Note
		** that this 'x' is sent without xt packaging.
		*/
		mpx = 1;
		maxpktdsize = DATASIZE;
		signal (SIGALRM, SIG_IGN);	/* remove retransmission */

		if (debug)
			fprintf (stderr, "\n\nScan for the terminal response:\n");
		c = '\0';
		while (c != 'x') {
			Uread (&c, 1);
			if (debug)
				fprintf (stderr, "[%x]", c);
		}

		if (debug)
			fprintf (stderr, "\n\nFrom now on, data will be sent without xt packaging");
	}

	/* relocinit() is the most time consuming part of relocation. We
	** could play with where we do it to overlap this with any kinds
	** of delays waiting for queues to drain. With this reasoning,
	** it should be before the JBOOT, but I want it after boot()
	** so the user gets visual reinforcement of the coffee cup as
	** soon as possible. If it was before JBOOT, everything would just
	** appear dead for a while.
	*/
	if(rflag)
		relocinit();


	/* Mux and Nonmux: 
	** start sending the arguments to the program 
	** remember that there is a maximum on the packet size
	** for nonmux protocol.
	*/
	if (!takeover) {
		if (debug)
			fprintf (stderr, "\n\nSend program arguments: ");
		for (i = nargchars; i > 0; i -= maxpktdsize, argp += n) {
			n =  min (i, maxpktdsize);
			if (debug) {
			   int zup;
			   for (zup = 0; zup <n; ++zup)
				fprintf (stderr, "%c", argp[zup]);
			   fprintf (stderr, "\n");
			}
			Uwrite (argp, n);

		}
	}
	/* Takeover: 
	** the file to be downloaded is usually already link-loaded to an
	** absolute physical address. If this address is lower than the
	** address proposed by the terminal, we will relocate the file
	** to the address proposed by the terminal, otherwise the
	** download may write over firmware or cartridge .bss section.
	**
	** However if the -N flag is specified, we will not do any relocation.
	*/ 
	else {
		if (debug)
			fprintf (stderr, "\nCurrent absolute address of file: 0x%lx", physaddr);

		if (wtkload) {		/* only for window takeover download */
			Uwriteswap ((rflag?(char *)&reloc_offset:(char *)&physaddr), 4);
			if (debug)
				fprintf (stderr, "\n\nSend download address: %lx",
				         (rflag?reloc_offset:physaddr));
		}

		if (reloc_offset > physaddr) {	/* overwrite detected */  
			if (!rflag && debug)	/* boy, no relocation */
			    fprintf (stderr, "\nWARNING: relocation may be needed!");
			else
			    reloc_offset -= (physaddr);
		}  
		else {
			rflag = 0;
			reloc_offset = 0;
		}

		if (debug)
			fprintf (stderr, "\n\nSend total size of download: %lx", imagesize);
		Uwriteswap ((char *)&imagesize, 4);

		if (wtkload) {
			Uread (&tmp, 1);
			if (debug)
				fprintf (stderr, "\n\nRead terminal response: %c", tmp);
			switch (tmp&0x7f) {
			case 's':
				error (0, "no memory in terminal", (char *)0);
			case 'g':
				break;
			}
		}
	}

	if (debug) {
		int offset = 0;
		fprintf(stderr,"\n\nRelocation offset: 0x%lx", reloc_offset);
		if (rflag) {
			fprintf (stderr, "\nRelocation initialization done");
			offset += reloc_offset;
		}
		else
			fprintf (stderr, "\nNo relocation initialization done");
		if (handshakeflag) {
			fprintf (stderr,
			    "\n%d bytes of handshake code will be loaded",
					SizeHandshakecode);
		}
		fprintf(stderr, "\n\nFile: %s", file);
		fprintf(stderr,"\nSection:\taddress:\tsize:\t\taction:\n");
		for (i = 0; i < (int) fileheader.f_nscns; ++i) {

			fprintf (stderr, "%s\t\t0x%06lx\t0x%lx\t\t",
				secthdrs[i].s_name, 
				secthdrs[i].s_paddr+offset,
				secthdrs[i].s_size);
			if (checksect (&secthdrs[i]))
				fprintf (stderr, "LOAD\n");
			else
				fprintf (stderr, "NOLOAD\n");
		}
		if (takeover)
			fprintf(stderr,"Size of takeover download: 0x%lx\n", imagesize);
		buzz();
	}

	sendfile();

	/* In nonmux download, the last packets have to be acknowledged 
	*/
	if (!mpx) {
		getlastacks (1);
	}
}


char *
bldargs(argc, argv)
int argc;
char *argv[];
{
	register i;
	register char *argp, *p, *q;
	for(nargchars=0, i=0; i<argc; i++)
		nargchars+=strlen(argv[i])+1;
	if((argp=malloc(nargchars))==0)
		error(0, "can't allocate argument chars", "");
	/* this loop is probably not necessary, but it's safe */
	for(i=0, q=argp; i<argc; i++){
		p=argv[i];
		do; while(*q++ = *p++);
	}
	return argp;
}

/* readaddr -	Read the address the terminal sends back
*/
long
readaddr()
{
	long *address;
	long caddress;
	char p[4];		/* should be long aligned */

	Uread (p, 4);
	address = (long *)p;

	swab((char *)address, (char *)&caddress, 4);
	swaw((short *)&caddress, (short *)address, 4);
	if (debug)
		fprintf (stderr, "\nAddress received: 0x%lx", *address);
	return (*address);
}

/* Read -	read from the file to be downloaded
*/
int
Read(a, n)
	char *a;
{
	register i;
	i=read(obj, a, n);
	if(i<0)
		error(1, "read error on '%s'", file);
	return(i);
}


/* Actual download of code. Relocates a section at a time if needed (rflag true).
**
** If rflag, code to be loaded is already in memory. Otherwise, read directly
**  from the file. This adds a little complexity but will give fastest
**  startup when relocation not necessary. The alternative would be to
**  read all code into memory even if relocation is not needed but this
**  would lead to some startup time waiting for this to happen.
*/
int senddummy; /* to force the following into an even address */
char sendbuf[DATASIZE+PKTASIZE];
sendfile()
{
	int i;
	register unsigned n;
	register unsigned long strloc;
	register unsigned long endloc;
	char *relbuf;
	char *relocseg();

	if(cprssect > -1) {
		lseek(obj,secthdrs[cprssect].s_scnptr+1,0);
		strloc = secthdrs[cprssect].s_paddr;
		endloc = secthdrs[cprssect].s_paddr + secthdrs[cprssect].s_size;
		if(psflag)
		    fprintf(stderr, "\nLoading section %s to address %lx\n",
				     secthdrs[cprssect].s_name, strloc);
		n = min(maxpktdsize, endloc-strloc);
		while(n > 0) {
			Read(&sendbuf[PKTASIZE], n);

			senddlpkt(strloc, sendbuf, n);
			strloc += n;
			n = min(maxpktdsize, endloc-strloc);
		}
		if(psflag)
			fprintf(stderr, "\nLoading relocation information\n");
		/* lseek(obj,secthdrs[cprssect].s_relptr,0); */
		strloc = 0;
		endloc = secthdrs[cprssect].s_nreloc * 3;
		n = min(maxpktdsize, endloc-strloc);
		while(n > 0) {
			Read(&sendbuf[PKTASIZE], n);

			senddlpkt(strloc, sendbuf, n);
			strloc += n;
			n = min(maxpktdsize, endloc-strloc);
		}
	}
	else for (i=0; i<(int)fileheader.f_nscns; ++i) {
		if (checksect (&secthdrs[i])) {
			if(rflag)
				relbuf = relocseg(&secthdrs[i]);
			strloc = secthdrs[i].s_paddr;
			endloc = secthdrs[i].s_paddr + secthdrs[i].s_size;

			if(psflag) {
			    fprintf(stderr, "\nLoading section %s to address %lx\n",
					     secthdrs[i].s_name, strloc);
#ifdef VERBOSE
			    if(rflag) {
				unsigned long count;
				unsigned char *p = (unsigned char *)relbuf;
				fprintf(stderr, "\nRaw data:");
				for(count=0 ; count < secthdrs[i].s_size ; ++count) {
					if(count%20 == 0) putc('\n', stderr);
					fprintf(stderr, "%02x ", (int)*p++);
				}
			    }
#endif
			}
			
			if(handshakeflag &&
				strequal(secthdrs[i].s_name,".text")) {
				/*
				 * Overwrite beginning of .text with handshake
				 *   code.  Save the original .text to load
				 *   at the end of .data.
				 */
				if (psflag) {
				    fprintf(stderr,
					"\nFirst %d bytes of text being superceded by handshake code\n",
						SizeHandshakecode);
				}
				memcpy(shakebuf, relbuf, SizeHandshakecode);
				memcpy(relbuf, (char *) Handshakecode, 
							SizeHandshakecode);
			}

			if(!rflag) lseek(obj,secthdrs[i].s_scnptr,0);
			n = min(maxpktdsize, endloc-strloc);
			while(n > 0) {
				if(rflag) {
					memcpy(&sendbuf[PKTASIZE], relbuf, n);
					relbuf += n;
				}
				else
					Read(&sendbuf[PKTASIZE], n);

				senddlpkt(strloc, sendbuf, n);
				strloc += n;
				n = min(maxpktdsize, endloc-strloc);
			}

		}
		if (handshakeflag && strequal(secthdrs[i].s_name,".bss")) {
			/* add on stuff saved in shakebuf */
			relbuf = shakebuf;
			strloc = reloc_offset + secthdrs[i].s_paddr;
			endloc = strloc + sizeshakebuf;
			if (psflag) {
			    fprintf(stderr,
				"\nLoading %d bytes of superceded code to address %lx\n",
					sizeshakebuf, strloc);
			}
			n = min(maxpktdsize, endloc-strloc);
			while(n > 0) {
				memcpy(&sendbuf[PKTASIZE], relbuf, n);
				relbuf += n;
				senddlpkt(strloc, sendbuf, n);
				strloc += n;
				n = min(maxpktdsize, endloc-strloc);
			}
		}
	}
}


/* senddlpkt() - Send one download packet in mux/non-mux/takeover style.
**
**	In the case of download in the VAX, we have to swap data.
*/
senddlpkt(strloc, buf, n)
unsigned long strloc;
char *buf;
unsigned n;
{
	long pkt;
	char tmpbuf[DATASIZE+PKTASIZE];
	char *ptr;
	char *pswab();

	ptr = &tmpbuf[PKTASIZE];
	swab (&buf[PKTASIZE], ptr, n);	/* data */

	if (mpx) {
	    if (psflag)
		    fprintf(stderr, "writing %d bytes\n", n);
	    Uwrite(ptr, n);
	}
	else {
	    if (takeover && !wtkload) {		/* "old" takeover download */
		ptr -= PKTASIZE;	/* go to the beginning of the array */
		swab((char *)&strloc, (char *)&pkt, PKTASIZE);  /* PKTASIZE=4 */
		swaw((short *)&pkt, (short *)ptr , PKTASIZE); /* address */
		Psend(ptr, n+PKTASIZE);
	    }
	    else {			/* nonmux download or new window takeover */
		Psend(ptr, n);
	    }
	}
}


/* checksect -	check if a section is going to be downloaded
**	Returns:
**	0 if the section is not downloadable
**	1 otherwise
*/
checksect (sectp)
struct scnhdr *sectp;
{

	if ((sectp->s_scnptr <= 0) ||
	    (sectp->s_flags & STYP_NOLOAD) ||
	    (sectp->s_flags & STYP_DSECT) ||
  	    (strequal(sectp->s_name,".peid")))
		return (0);
	else
		return (1);
}


/* not your average min() */
min(a, b)
long b;
{
	return(a<b? a : (int)b);
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

/* swap words if necessary. target is big endian. */
swaw(a, b, n)
	register short *a, *b;
	register n;
{
	register long space;

	n/=4;	/* n in bytes */

	while(n--){
		space = *(long *)a;
		*b++= space >> 16;  /* most significant word first */
		*b++= space;
		a+=2;
	}
}

buzz(){
	sleep(2);	/* sleep(1) not necessarily long enough */
}



/* 
**	Error handler
*/

/*
**	Error detection statictics.
**	Print header and optional address fields of 
**	a nonmux protocol packet.
**
**	This function is called from Write() and works
**	for nonmux download only.
*/
trace (a)
char *a;
{
	register int n = PKTHDRSIZE;
	register int i;
	register int j = 0;

	n = a[1]+PKTHDRSIZE+PKTCRCSIZE;	/* xt protocol has second byte as length */
	if (n > 10) {
		n = 10;
		j = 1;
	}

	fprintf (stderr, "\nsend: ");
	for (i = 0; i < n; i++)
		fprintf (stderr, "<%x>", a[i]&0xff);
	if (j)
		fprintf (stderr, "...");
}


error (pflag, s1, s2)
int pflag;
char *s1, *s2;
{
	long flushval = 0L;
	register int	saverrno;
	char		buf[BUFSIZ];
	extern int	errno;

	saverrno = errno;
	if (booted) {
		/* tell dmd side to give up */
		if (mpx) {				/* mux */
#ifdef USE_HOSTAGENT
			(void)Jterm();
#else
			(void)ioctl (1, JTERM, 0);
#endif			
		}
		else if (!takeover) 		/* nonmux */
			nonmuxioctl (JTERM_CHAR, 0);
		else					/* takeover */
			Psend ((char *)&flushval, sizeof(long)); /* ??? */
	}
#ifdef TIOCNXCL
	(void)ioctl(1, TIOCNXCL, 0);
#endif
#ifdef NO_TERMIO
	(void)ioctl(1, TIOCSETP, &ttysave);
	(void)ioctl(1, TIOCLSET, &lttysave);
#else
	(void)ioctl(1, TCSETAW, &ttysave);
	sleep (2);
#endif

	if(pflag){
		errno=saverrno;
		perror(s2);
	}
	fprintf(stderr, "\n%s: ", name);
	fprintf(stderr, s1, s2);
	fprintf(stderr, "\r\n");
	if(psflag)
		pstats(stderr);
	exit(1);

}

error2(s1, s2)
char *s1, *s2;
{
	if(errflag != 3)
	{
		errflag = 3;
		e2s1 = s1;
		e2s2 = s2;
	}
}


/*  strequal compares 2 strings */

int strequal(s1,s2)
 	char s1[], s2[];
{
	int i = 0;
	while ( (s1[i] == s2[i]) && (s1[i] != '\0') 
				 && (s2[i] != '\0') )  ++i;

	if ( (s1[i] == '\0') && (s2[i] == '\0') )
		return(1);
	else
		return(0);
}




long 
getpeid()
{
/*  getpeid returns the programming environment id.  If the 
 *  peid section exists, the section is scanned and the largest peid
 *  is returned.  If the section does not exist, 0 is returned as default.
 */

	register long offset;
	register long highestpeid = 0;
	register int count = 0;
	long thispeid[2];

	if  (peidsect > -1) {
		offset=lseek(obj,0,1);
		lseek(obj,secthdrs[peidsect].s_scnptr,0);
		while (count <secthdrs[peidsect].s_size/sizeof(long) ) {

			/* Version 2:
			** Get the peid from .peid section:
			** Note that the peid number is in MC68000 format,
			** so when we read it, we have to swap the word ordering
			** according to the machine running this downloader.
			*/
			Read(&thispeid[0],sizeof(long));
			swaw ((short *)&thispeid[0], (short *)&thispeid[1], 4);

			/* Get max */
			if (thispeid[1]>highestpeid) highestpeid=thispeid[1];
			count++;
		}
		
		lseek(obj,offset,0);
	}
	return(highestpeid);
}


char
getcprsid()
{
	long offset;
	unsigned char c = 0xFF;

	if(cprssect >= -1) {
		offset = lseek(obj,0,1);
		lseek(obj, secthdrs[cprssect].s_scnptr, 0);
		Read(&c, 1);
		lseek(obj, offset, 0);
	}
	return (char) c;
}


getstacksize()

/*  This routine is called if the user did not specify a stack size
 *  when he/she invoked dmdld.  It reads the first long word of data
 *  (which is the first word of crt.m) for the stack size that dmdcc
 *  placed there.  An old file will have a 0 there, so taking the max of
 *  the word and MINSTACK is done.
 */
{
	long storedsize;
	long offset;

	offset=lseek(obj,0,1);
	lseek(obj,secthdrs[DATASECT].s_scnptr,0);

	/* Version 2:
	** The stack size is stored in the MC68000 format, so
	** we should swap the word ordering depending on the
	** machine running this downloader program. This has not
	** been done in pre-V2 dmdld, and the quick fix solution
	** has been to run the 630 application through "zap630" which
	** does the swapping.
	**
	** The solution is to check for an absurdly large (>1 meg) or absurdly
	** small number (<16).  This allows a valid range of 16 bits.
	** If the number's out of range, we do the swap.  This will apply for
	** non-zap630'ed and already-zap630'ed 630 applications.
	*/
	storedsize = 0;
	Read(&storedsize,4);

	if ((storedsize > 0x100000) || (storedsize <= 0x10)) {
		/* absurdly large or small */
		storedsize = (storedsize >> 16) + (storedsize << 16);
	}

	/* Minimum is 2K */
	if (storedsize < MINSTACK)
		stksize = MINSTACK;
	else
		stksize = storedsize;

	lseek(obj,offset,0);
}
