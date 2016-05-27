/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 *	MAC-32 (down)loader - release 2.0 software specific
 */


static char _2Vsccsid[]="@(#)32ld:32ld.c	2.17";

/* 
 * Normally, use the new, fast relocator.  (3B5's get the dregs...)
 * But users can compile with -DNORELOCATOR to use the old m32ld.
 * NOTE: We now look for m32ld in $DMD/bin, not $DMDSGS/<type>/bin
 */
#ifndef NORELOCATOR
#define RELOCATOR	"/bin/32reloc"
#else
#define M32LD		"/bin/m32ld"
#endif

#include <fcntl.h>
#include <termio.h>
#include "quiet.h"
#include "aouthdr.h"
#include "filehdr.h"
#include "scnhdr.h"
#include <stdio.h>
#include <errno.h>
#include <sys/jioctl.h>
#include "proto.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
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

#define	MAXRETRIES	10
#define	DATASIZE	512

#define NSECTS  12

#define MPX_VER 0x5620
#define FBOMAGIC 0560
#define SENDTERMID "\033[c"
#define TERM_1_0 "\033[?8;7;1c"
#define TERMB_1_0 "\033[?8;7;2c"
#define TERM_DMD "\033[?8;"
#define TERMIDSIZE 9
#define STR_EQUAL 0
#define ENC_CHK "\033[F"
#define	ENC_LEN	4

#define BINARY_LOAD	0
#define HEX_LOAD	1
#define	ENC_ENABLE	2		/* Value that enables terminal encoding */

char *getenv(), *strcpy(), *malloc();


char	scr_buf[30];
char	termid[TERMIDSIZE+1];
char	Load_str[] = "\033[0;0v";  /* default download (binary & standalone*/
int	Layerflag = 0;
int lpindex;	/* for getting rom version */
int count;	/* for getting rom version */
int Loadtype = BINARY_LOAD;

char	Usage[]		= "Usage: %s [-d] [-p] [-z] [-s size] objectfile";
		/* also [-v, l] flags, but the user never sees these */
char *name;
struct stat	Statbuf, *Statptr;
struct filehdr fileheader;
struct aouthdr aoutheader;
struct scnhdr secthdrs[NSECTS];
char	*errname;	/* name of error file for relocator */

#ifdef NO_TERMIO
struct sgttyb	ttysave,/* save the state of tty */
		ttyraw;
int	lttysave;	/* save for local flags */
int	lttyraw;
#else
struct termio	ttysave,/* save the state of tty */
		ttyraw;
#endif
int	obj;		/* File descriptor for object file */
int	mpx;		/* Running under layers */
long	location;
char	file[128];	/* Name of file */
char	reloc[128];	/* path name to reloc command */
int	nargchars;	/* Number of characters, including nulls, in args */
long	longbuf[3];
int	debug;		/* Show sizes etc. on stderr */
int	psflag;		/* Print error detection statistics */
short	maxpktdsize;
int	rflag;		/* relocate? */
int	zflag;		/* Do a JZOMBOOT */
int	vflag;		/* is firmware version known? */
int	booted;
int	errfile;
int	retries;
int	stkflag;	/* changing stack size -- value is new size */
char	romversion;	/* last digit of rom version id string */

#ifndef NO_AGENTVERSION
#ifndef USE_LIBWINROMVERSION
int	agentsize;	/* size of agent structure */
/* agent structure for getting rom version, etc */
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
/* agent stuff: copied from hostagent.c since needed globally */
struct 	bagent cmd;
union	bbuf	{
		struct	agentrect	ar;
		char	buf[MAXPKTDSIZE];
};
extern union	bbuf	ret;
#endif
#endif

#if (defined(NO_AGENTVERSION) || !defined(A_STACKSIZE))
#define NO_AGENTSTACKSIZE
#endif



char	*cp,*dcp;
int	forbgs;	
int	mychan;		/* xt channel number download being done on */
int	cntlfd;			/* FD for xt control channel */

short speeds[16]={
	 1,	5,	7,	10,	13,	15,	20,	30,
	60,	120,	180,	240,	480,	960,	1920,	1
};

unsigned char sizes[16]={
	 16,	16,	16,	16,	16,	16,	16,	16,
	 16,	32,	32,	56,	56,	120,	120,	16
};

void	Psend();
void	Precv();

extern int	errno;
extern char *getenv();

#define JPATH_ACCESS 1
#define JPATH_OPEN 2

timeout_id()
{
	error(0, "Error: Invalid terminal type:\n\t not a DMD terminal or DMD failed to respond",
		(char *) 0);
}

main(argc, argv)
int argc;
register char *argv[];
{
	extern short verno, subno;
	char 	*dwnldflag;
#ifdef USE_LIBWINROMVERSION
	char	versbuf[9];
#endif


	/* Start out by checking that download is going */
	/* to a DMD with at least 1.1 firmware (not 1.0) */
	for (lpindex=0; lpindex<=TERMIDSIZE; lpindex++)		/* should move this down */
		termid[lpindex] = 0;

	reloc[0] = '\0';	/* make sure string is null */
	if (getdmd(reloc))	/* now look for $DMD */
		exit(1);
#ifndef RELOCATOR
	strcat(reloc, M32LD);	/* attach m32ld to head */
#else
	strcat(reloc, RELOCATOR);		/* $DMD/32reloc is normal*/
#endif
	if (access(reloc,01))
	{
		switch(errno)
		{
		case ENOTDIR:
			fprintf(stderr,"Error: %s contains bad dir\n",reloc);
			fprintf(stderr," check for correct value of $DMD\n");
			break;
		case EACCES:
			fprintf(stderr,"Error: %s: no permission\n",reloc);
			break;
		default:
			fprintf(stderr,"Error: cannot access %s errno %d\n",reloc,errno);
			fprintf(stderr," check for correct value of $DMD\n");
			break;
		}
		exit(1);	/* couldn't stat the relocator */
	}

#ifdef NO_TERMIO
	(void)ioctl(1, TIOCGETP, &ttysave); /* get the current state */
	(void)ioctl(1, TIOCLGET, &lttysave);
#else
	(void)ioctl(1, TCGETA, &ttysave); /* get the current state */
#endif

	name = *argv;
	while(argc>1 && argv[1][0]=='-'){
		switch(argv[1][1]){
		case 'd':
			debug++;
			break;
		case 'e':
			fprintf(stderr,"-e flag no longer used: always assumed\n");
			break;
		case 'p':
			psflag++;
			break;
		case 'z':
			zflag++;
			break;
		case 'l':
			Layerflag++;
			break;
		case 's':
#ifndef NO_AGENTSTACKSIZE
			if( (stkflag=strtol(argv[2], (char *)0, 0)) == 0 )
			 {
				fprintf(stderr,"%s: Illegal stack size: %s",name,argv[2]);
				error(0, Usage, name);
				return 1;
			   }
#else
			fprintf(stderr,"%s: -s not supported on this system\n",
							name);
#endif
			argv++; argc--;
			break;
		case 'v':
			vflag++;
			romversion=argv[1][2];
			Loadtype = argv[1][3] - '0';
			break;
		case 'V':			/* version number */
			fprintf (stdout, "%s: version %d.%d\n", name, verno, subno);
			exit(0);
		case '\0':
			break;
		default:
			error(0, Usage, name);
			return 1;
		}
		argv++; argc--;
	}
	if(argc<2){
		error(0, Usage, name);
		return 2;
	}

	Load_str[4] = Loadtype + '0';
	
#ifdef TIOCEXCL
	(void)ioctl(1, TIOCEXCL, 0);
#endif
#ifdef NO_TERMIO
	ttyraw = ttysave;
	ttyraw.sg_flags = RAW | ANYP;
	(void)ioctl(1, TIOCSETP, &ttyraw);
#else
	ttyraw.c_iflag = IGNBRK;
	ttyraw.c_cflag = (ttysave.c_cflag & CBAUD) | (ttysave.c_cflag & CLOCAL) | CS8 | CREAD;
	ttyraw.c_cc[VMIN] = 1;
	(void)ioctl(1, TCSETA, &ttyraw);
#endif

#ifdef USE_HOSTAGENT
	mpx = (inlayers() > 0);
#else
	mpx = (ioctl(1, JMPX, 0) != -1);
#endif

#ifndef NO_AGENTVERSION
	/* identify ROM version */
	if (!mpx && !vflag) {
		getromvers();
		if( stkflag)
			error(0,"'-s' flag supported in layers only.");

	} else if ( mpx && !vflag) {
		/* find rom version by agent ioctl */
		if ( (cntlfd = openagent()) == -1 )  { 
			if (debug) {
				fprintf(stderr, "Error:  couldn't open control channel to determine firmware version.\nError number %d\n",errno);
			}
			getromvers();	/* resort to using an escape seq */
		}
#ifdef USE_LIBWINROMVERSION
		else if (forbgs = Romversion(cntlfd, versbuf, sizeof(versbuf)) == -1) {
			if (debug) {
				fprintf(stderr, "Error: Romversion returned %d (error %d).\n   Cannot determine firmware version.\n", forbgs, errno);
			}
			getromvers();	/* resort to using an escape seq */
		} else {
			romversion = versbuf[4];  /* the last digit of x;y;z */
		}
		closeagent();
#else
		else if( forbgs = ioctlagent(cntlfd, A_ROMVERSION,
							0, 0, 0, 0, 0) ) {
			if (debug) {
				fprintf(stderr, "Error: A_ROMVERSION returned %d (error %d).\n   Cannot determine firmware version.\n", forbgs, errno);
			}

			getromvers();	/* resort to using an escape seq */
		} else {
			swab(ret.buf, ret.buf, sizeof(struct agentrect));

			if (debug) {
				fprintf(stderr,"Rom version id is ");
				for (forbgs=4; forbgs<9; forbgs++)
					fprintf(stderr,"%c",ret.buf[forbgs]);
				fprintf(stderr,"\n");
			}
		}

		romversion = ret.buf[8];  /* the last digit of x;y;z */
#endif
	}
#else
	if (!vflag)
		getromvers();
#endif
	if( (romversion < '5') && (stkflag != 0) )
		error(0, "Error: Stacksize not supported by your terminal firmware.\n", (char *)0);

	if( !mpx && !vflag ) {		/* check encoding */

		/* NOTE: version 8;7;5 roms may not properly check
		   for encoding from the keyboard */

		if( romversion >= '5' ) {
			write(1,ENC_CHK,strlen(ENC_CHK));
			count = 0;
			while(count < ENC_LEN){
				lpindex = read(0,&scr_buf[count],ENC_LEN);
				if(lpindex > 0)count += lpindex;
			}
			if( scr_buf[2] == '1' ) {
				Loadtype = ENC_ENABLE;
			}
			else {
			    if (((dwnldflag = getenv("DMDLOAD")) != (char *)0)
						&& (dwnldflag[0] != '\0')) {
				if(strcmp(dwnldflag, "hex") == 0)
				    Loadtype = ENC_ENABLE;
			    }
			}
		} else
			if(((dwnldflag = getenv("DMDLOAD")) != (char *)0) &&
					(dwnldflag[0] != '\0')) {
			if(strcmp(dwnldflag, "hex") == 0)
				Loadtype = HEX_LOAD;
		}
	}
	if (Loadtype != BINARY_LOAD) {
		Load_str[4] = Loadtype + '0';
#ifdef NO_TERMIO
		ttyraw.sg_flags = ANYP | CBREAK;
		(void)ioctl(1, TIOCSETP, &ttyraw);
		lttyraw = lttysave;
		lttyraw |= LITOUT | LDECCTQ;
		(void)ioctl(1, TIOCLSET, &lttyraw);
#else
		ttyraw.c_iflag |= IXON;
		(void)ioctl(1, TCSETAW, &ttyraw);
#endif
	}
	if(jpath(argv[1], JPATH_ACCESS, 4)!=0)
		error(1, "no such file '%s'", argv[1]);

	/* check for an empty file before download */
	Statptr= &Statbuf;
	stat(argv[1], Statptr);
	if(Layerflag && (Statbuf.st_size == 0)) {
		Load_str[2] = '2';
		write(1,Load_str, 6);
		goto cleanup;
	}

	obj=jpath(argv[1], JPATH_OPEN, 0);
	if(obj<0)
		error(1, "cannot open '%s'", file);
/********************************************************/
/*							*/
/*	reads the headers for the m32a.out		*/
/*	file and stores the data read into the global	*/
/*	structures declared for this purpose		*/
/*							*/
/********************************************************/

	Read ((char *) &fileheader, sizeof(struct filehdr));
	if(fileheader.f_magic!=FBOMAGIC)	/* FBOMAGIC is 0560 */
		error(0, "'%s' is not a WE-32000 family a.out", file);
	Read ((char *) &aoutheader, fileheader.f_opthdr); 

	if (fileheader.f_nscns > NSECTS)
		error(0,"%s: exceeded max number of sections -- see system administrator", name);
	if((aoutheader.vstamp==MPX_VER) ^ mpx)	/* MPX_VER is 0x5620 */
{
fprintf(stderr,"aoutheader.vstamp: %x\r\n", aoutheader.vstamp);
		error(0, mpx? "'%s' compiled stand-alone": "'%s' compiled for layers", file);
}
	
	/* does this a.out have empty text/data/bss sections? */
	if(((aoutheader.tsize + aoutheader.dsize + aoutheader.bsize) == 0) && Layerflag) {
		if (debug)
			fprintf(stderr,"Downloading type 2 (no patch)\n");
		Load_str[2] = '2';
		write(1,Load_str, 6);
		goto cleanup;
	}
	if(Layerflag) {
		if (debug)
			fprintf(stderr,"Downloading type 1 (a patch)\n");
		/*
		* if it got here then we want to download layers with a patch
		*/
		Load_str[2] = '1';
	}

/*	signal(SIGHUP,error);
	signal(SIGINT,error);	Doing this tends to be more buggy than it's worth
	signal(SIGQUIT,error); 	This has been taken out several times before! */

	if(boot() && rflag==0)
		rflag++;
	if(!mpx){
#ifdef NO_TERMIO
		maxpktdsize = min(sizes[ttysave.sg_ospeed & 017],
				(long)MAXPKTDSIZE);
		pinit(speeds[ttysave.sg_ospeed & 017], maxpktdsize, ACKON);
#else
		maxpktdsize = min(sizes[ttysave.c_cflag&CBAUD], (long)MAXPKTDSIZE);
		pinit(speeds[ttysave.c_cflag&CBAUD], maxpktdsize, ACKON);
#endif
	}

	load(/*argv[1],*/ argc-1, argv+1);
	if(!mpx){	/* ACKON is always true */
		buzz();
#ifdef NO_TERMIO
		(void)ioctl(0, TIOCFLUSH, (struct sgttyb *)0);
#else
		(void)ioctl(0, TCFLSH, (struct termio *)0);
#endif
	}

cleanup:

#ifdef TIOCNXCL
	(void)ioctl(1, TIOCNXCL, 0);
#endif
#ifdef NO_TERMIO
	(void)ioctl(1, TIOCSETP, &ttysave);
	(void)ioctl(1, TIOCLSET, &lttysave);
#else
#ifdef u3b
	sleep(1);				/* layers -f problem */
#endif
	(void)ioctl(1, TCSETAW, &ttysave);
#endif
	if(psflag)
		pstats(stderr);

	return(0);
}


char *
bldargs(argc, argv)
	char *argv[];
{
	register i;
	register char *argp, *p, *q;
	for(nargchars=0, i=0; i<argc; i++)
		nargchars+=strlen(argv[i])+1;
	if((argp=malloc(nargchars))==0)
		error("can't allocate argument chars", "");
	/* this loop is probably not necessary, but it's safe */
	for(i=0, q=argp; i<argc; i++){
		p=argv[i];
		do; while(*q++ = *p++);
	}
	return argp;
}


load(/*f,*/ argc, argv)
/*	char *f;*/
	char *argv[];
{
	char *argp;
	long largc;
	int i;
	if(mpx){
		argp=bldargs(argc, argv);
		largc=argc;
		writeswap((char *)&largc, 4);	/* number of arguments */
		largc=nargchars;
		writeswap((char *)&largc, 4);	/* number of chars in arguments */
		writeswap((char *)&aoutheader.tsize, 12);

#ifndef NO_AGENTSTACKSIZE
		/* do they want to specify the #/bytes on the stack? */
		if( (romversion >= '5') && (stkflag != 0)) {

			if( forbgs = ioctlagent(cntlfd, A_STACKSIZE, 
					(short)(stkflag&0xffff),    /* origin.x */
					(short)(stkflag>>16)&0xffff,/* origin.y */
					0, 0, mychan) ) {
				if (debug) {	/* SHOULD be an error!!! */
			     		fprintf(stderr,"Could not set stack size (returned %d)\n",forbgs);
				}
			}
		}
#endif
	}
	if(rflag)
		relocate();
	else
		location = aoutheader.entry;
	if(mpx)
		Write(argp, nargchars);
	for (i = 0; i < (int) fileheader.f_nscns; ++i)     /* read section header array */
		Read ((char *) &secthdrs[i], sizeof(struct scnhdr));

	if(debug){
		fprintf(stderr, "%s:\nSection:\taddr:\tsize:\n", file);
		for ( i = 0; i < (int) fileheader.f_nscns; ++i)
			fprintf(stderr,"%s\t\t0x%lx\t0x%lx\n",
			secthdrs[i].s_name,secthdrs[i].s_paddr,secthdrs[i].s_size);
		buzz();
	}
	sendfile();
	if(!mpx){
		long	startaddr;

		retries = 0;
		while(freepkts != NPBUFS)
			Precv();
		location = aoutheader.entry;
		swaw((short *)&location, (short *)&startaddr, PKTASIZE);
		psend((char *)&startaddr, PKTASIZE);
		retries = 0;
		while(freepkts != NPBUFS)
			Precv();
	}
}

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

error(pflag, s1, s2)
	char *s1, *s2;
{
	long flushval = 0L;
	register int	n;
	register int	saverrno;
	char		buf[BUFSIZ];
	extern int	errno;

	saverrno = errno;
	if(booted){
		if (mpx)		/* tell dmd side to give up */
#ifdef USE_HOSTAGENT
			(void)Jterm();
#else
			(void)ioctl (1, JTERM, 0);
#endif			
		else
		{
			psend((char *)(&flushval),sizeof(long));	
		}
		if(errfile>0){
			buzz();
			while((n=read(errfile, buf, sizeof buf))>0)
				write(2, buf, n);
			unlink(errname);
		}
	}
#ifdef TIOCNXCL
	(void)ioctl(1, TIOCNXCL, 0);
#endif
#ifdef NO_TERMIO
	(void)ioctl(1, TIOCSETP, &ttysave);
	(void)ioctl(1, TIOCLSET, &lttysave);
#else
	(void)ioctl(1, TCSETAW, &ttysave);
#endif
	if(pflag){
		errno=saverrno;
		perror(s2);
	}
	fprintf(stderr, "\n%s: ", name);
	fprintf(stderr, s1, s2);
	fprintf(stderr, "\n");
	if(psflag)
		pstats(stderr);
	exit(1);
}
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
void
Write(a, n)
	char *a;
{
	if(realwrite(a, n)!=n)
		error(1, "write error to DMD", (char *)0);
	if(psflag && !mpx)
		trace(a);
}
writeswap(a, n)
	char *a;
{
	char buf1[DATASIZE+PKTASIZE], buf2[DATASIZE+PKTASIZE];
	swaw((short *)a, (short *)buf1, n);
	swab(buf1, buf2, n);
	Write(buf2, n);
}
trace(a)
	char *a;
{
	register int	i;

	for(i=0; i<(PKTHDRSIZE+PKTASIZE); i++)
		fprintf(stderr, "<%o>", a[i]&0xff);
	fprintf(stderr, "\n");
}

sendfile()
{
	register int i;
	for ( i = 0; i<(int)fileheader.f_nscns; ++i) {
		if(secthdrs[i].s_scnptr > 0)  {
			if ((secthdrs[i].s_flags & STYP_NOLOAD) ||
			    (secthdrs[i].s_flags & STYP_DSECT))
				continue;
			lseek(obj,secthdrs[i].s_scnptr,0);
			sendseg(secthdrs[i].s_paddr,secthdrs[i].s_paddr+secthdrs[i].s_size);
		}
	}
}

sendseg(strloc,endloc)
long strloc;
long endloc;
{
	char buf[DATASIZE+PKTASIZE], buf2[DATASIZE];
	char tmpbuf[DATASIZE+PKTASIZE];
	register n;
	while((n=Read(&buf[PKTASIZE], min(!mpx?maxpktdsize:DATASIZE, endloc-strloc)))>0){
		if(mpx){
	/*		swab(&buf[PKTASIZE], buf2, n); NOT RITE with m32 sgs*/
			Write(&buf[PKTASIZE], n);
		}else{
/*local swab?*/		swab(&buf[PKTASIZE], &tmpbuf[PKTASIZE], n);
			swaw((short *)&strloc, (short *)&tmpbuf[0], PKTASIZE);
			Psend(tmpbuf, n+PKTASIZE);
		}
		strloc+=n;
	}
}
void
Psend(bufp, count)
	char *bufp;
	int count;
{
	retries = 0;
	while(freepkts == 0)
		Precv();
	psend(bufp, count);
}
void
Precv()
{
	char c;

	alarm(3);		/* sleep at least 2 seconds */
	if(realread(&c, 1) == 1){
		alarm(0);
		if(psflag)
			fprintf(stderr, "recv <%o>\n", c&0xff);
		precv(c);
	}else if(errno != EINTR )
		error(1, "read error", (char *)0);
	else if(++retries >= MAXRETRIES)
		error(0, "load protocol failed", (char *)0);
	else if(psflag)
			fprintf(stderr, "recv timeout.. retries=%d\n",retries);
}

min(a, b)
	long b;	/* not your average min() */
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

relocate(){
	long address;
	char buf[200];
	char *mktemp();
	long caddress;
	register i;
	register char *p=(char *)&address;
	char	*tmpname;	/* name of temporary file for reloc */
#ifndef RELOCATOR
	char	*ifilename;	/* name of temporary ifile for reloc */
#endif
	for(i=0; i<4; i++)
		realread(p++, 1);
	swab((char *)&address, (char *)&caddress, 4);
	swaw((short *)&caddress, (short *)&address, 4);
	location=address;
	if(location==0)
		error(0, "no memory in DMD", (char *)0);

#ifndef RELOCATOR
 	/* Now must copy up default ifile in ??lib, putting in the */
 	/* correct group start address, depending on relocation  */
 	/* This address should be "location"	*/
	ifilename = mktemp(strdup("/tmp/IFXXXXXX"));
	sprintf(buf, "echo \"SECTIONS { GROUP 0x%lx: { .text :{} .data :{} .bss :{} }}\" > %s 2> /dev/null",
		location,ifilename);
	
	if(system(buf)){
		unlink(ifilename);
		error(0, "create errors in /tmp", (char *)0);
	}
		/* paths.h from the real sgs dir has the M32LD path (LD) */
 	sprintf(buf, "%s -o %s %s %s>%s 2>&1", reloc,
		tmpname=mktemp(strdup("/tmp/6XXXXXX")),
 		file, ifilename, errname=mktemp(strdup("/tmp/6EXXXXXX")));
#else
 	sprintf(buf, "%s -b %lx %s >%s 2>%s", reloc, location, file,
		tmpname=mktemp(strdup("/tmp/32XXXXXX")),
 		errname=mktemp(strdup("/tmp/32EXXXXXX")));

#endif
	if(system(buf)){
		errfile=open(errname, 0);
		if (!debug)
			unlink(tmpname);
		unlink(errname);
#ifndef RELOCATOR
		unlink(ifilename);
#endif
		error(0, "%s errors", reloc);
	}
	close(obj);
	unlink(errname);
#ifndef RELOCATOR
	unlink(ifilename);
#endif
	obj=open(tmpname, 0);
	if(obj<0)	/* HELP!! */
		error(1, "tmp file vanished! (%s)", tmpname);
	if (!debug)
		unlink (tmpname);
	Read ((char *) &fileheader, sizeof(struct filehdr));
	Read ((char *) &aoutheader, fileheader.f_opthdr);
}

boot(){
	char c = 0;

	if(mpx){
#ifdef NO_TERMIO
		(void)ioctl(0, TIOCFLUSH, 0);	/* throw away type-ahead! */
#else
		(void)ioctl(0, TCFLSH, 0);	/* throw away type-ahead! */
#endif

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
	}else{
		write(1, Load_str,6);	/* esc sequence for download */
		while(c != 'a')
			read(0, &c, 1);	/* wait for terminal to be ready */
	}
	booted++;
	return mpx;
}

buzz(){
	sleep(2);	/* sleep(1) not necessarily long enough */
}
getdmd(str)
char *str;
{
	char *cpt;

	if (((cpt = getenv("DMD")) == (char *)0) || (*cpt == '\0')) {
#ifdef DEFDMD
		strcpy(str, DEFDMD);
#else
		fprintf(stderr,"Must have DMD set in your environment\n");
		return(1);
#endif
	}
	else
		strcpy(str, cpt);
	return(0);
}

/*
* This routine is the lowest level write routine to the dmd.  It provides a 
* simple way to implement a safer download protocol through networks. 
* This requires that a shell varariable will be set if this extra precaution
* is to be taken.
*/

realwrite(a,n)
char *a;
{
	char cbuf[(MAXPKTSIZE + PKTASIZE) * 2], c;
	int i, j, maxsize;

	if((Loadtype == BINARY_LOAD) || mpx){
		return(write(1,a,n));
	}
	else {
		/*
		* do a hex load
		*/
		j = n;
		maxsize = ((MAXPKTSIZE + PKTASIZE) / 2);
		for(i = 0;i < n*2; i++){
			c = *a++;
			cbuf[i]=(c & 0xf) | 0x40;
			cbuf[++i]=((c >> 4) & 0xf) | 0x40;
		}
		i = 0;
		while(n > 0) {
			if(n > maxsize) {
				if(write(1, &cbuf[i*maxsize*2], maxsize*2) != maxsize*2)
					return(-1);
				n -= maxsize;
				i++;
			}
			else {
				if(write(1, &cbuf[i*maxsize*2], n*2) != n*2)
					return(-1);
				n=0;	/* last buffer so don't loop anymore */
			}		
		}
		return(j); /* all correct so return number of actual characters sent */
	}
}

realread(a, n)
char *a;
{
	char	cbuf[2];
	int 	i;
	int rev;

	if((Loadtype == BINARY_LOAD) || mpx){
		return(read(0, a, n));
	}
	else {
		for(i = 0; i < n ; i++) {
			if ((rev = read(0, cbuf, 2)) == 1)
				rev = read(0, &cbuf[1], 1);
			if (rev < 1)
				return(-1);
			*a++ = (cbuf[0] & 0xf) | ((cbuf[1] & 0xf) << 4);
		}
		return(n);
	}
}

/*	Get the rom version by escape sequence.
 */
	
getromvers() 
{
	write(1,SENDTERMID,strlen(SENDTERMID));
	count = 0;
	while (count == 0 || termid[count-1] != 'c') {
		lpindex = read(0,&termid[count],TERMIDSIZE-count);
		if(lpindex > 0)count += lpindex;
	}
	if ((strcmp(termid,TERM_1_0) == STR_EQUAL) ||	/* equal strings */
		(strcmp(termid,TERMB_1_0) == STR_EQUAL))
		error(0,"Error: Firmware not updated to 1.1 or greater\n",
			(char *) 0);
	if (strncmp(termid,TERM_DMD,strlen(TERM_DMD)) != STR_EQUAL)
		error(0, "Error: %s must be run on a DMD terminal\n",
			(char *) name);
	romversion = termid[strlen(termid)-2];
	if (debug) {
		fprintf(stderr,"ROM version (by escape seq) is %s\n",termid);
	}
}
