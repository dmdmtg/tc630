/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)sysmon.c	1.1.1.7	(5/27/88)";

#include <stdio.h>
#include <a.out.h>
#include <core.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/signal.h>
#ifdef u3b
#	ifndef PAGING
#		include <sys/istk.h>
#	endif
#endif
#ifdef u3b2
#	include <sys/psw.h>
#	include <sys/pcb.h>
#endif
#include <sys/user.h>
#include <sys/stat.h>
#ifdef u3b
#	include <sys/seg.h>
#endif
#ifndef PAGING
#	ifdef u3b2
#		include <sys/immu.h>
#		include <sys/region.h>
#	endif
#	include <sys/proc.h>
#endif
#include <ctype.h>
#include <quiet.h>
#include <termio.h>                              /* UTS 4/16/86 */
#define index strchr
#include <sys/sysinfo.h>
#ifdef PAGING
#	ifndef uts
		/*
		 * This is probably only a temporary fix: some new
		 * release will require some other combination of
		 * include files.  In any case, on a 3b2/SVR3.1 system,
		 * page.h is virtually empty while immu.h contains the
		 * vital definition of pde_t.  Whether other machines
		 * require page.h is not for me to judge (mellman880526).
		 */
#		ifdef u3b2
#			include <sys/immu.h>
#		else
#			include <sys/page.h>
#		endif
#		include <sys/region.h>
#	endif
#endif
#ifndef TIOCEXCL
#include <sgtty.h>
#endif

#ifdef u3b5 
struct nlist nl[] ={
	{"sysinfo"},
	{ "" },
};
#endif
#ifdef UTS                                    /* UTS 4/16/86 */
struct nlist nl[] ={                          /* UTS 4/16/86 */
	{"sysinfo"},                          /* UTS 4/16/86 */
	{ "" },                               /* UTS 4/16/86 */
};                                            /* UTS 4/16/86 */
#endif
#ifdef u3b 
struct nlist nl[] ={
	{"sysinfo"},
	{ "" },
};
#endif
#ifdef u3b2
struct nlist nl[] ={
	{"sysinfo"},
	{ "" },
};
#endif
#ifdef vax
struct nlist nl[] ={
	{"_sysinfo"},
	{ "" },
};
#endif

#include	"errno.h"	/* used for 32ld path check */

#ifdef UTS                                 /* UTS 4/16/86 */
struct t {                                 /* UTS 4/16/86 */
	cpu_t	cp_time[5];                /* UTS 4/16/86 */
} ocp, ncp;                                /* UTS 4/16/86 */
#else                                      /* UTS 4/16/86 */
struct t {                                 /* UTS 4/16/86 */
	long	cp_time[5];                /* UTS 4/16/86 */
} ocp, ncp;                                /* UTS 4/16/86 */
#endif                                     /* UTS 4/16/86 */
/*
	"idle",
	"user",
	"kernel",
	"wait"
*/
struct sysinfo sysinfo;
long oldrun;
long oldrunque, oldrunocc;

char *sys = "/unix";
char *core = "/dev/kmem";
char *newmail();
int mem;
int ci ;
char vec[]="01234\n";
int prton = 0;
static short cached = 0;

#define equaln		!strncmp

char mpath[128];	/* path to xsysmon.m */
char path32ld[128];	/* path to 32ld */

#define	SYSMON	"/xsysmon.m"

/* struct sgttyb sttybuf; */         /* UTS 4/16/86 */
struct termio pterm, term ;               /* UTS 4/16/86 */

main(argc, argv)
	char *argv[];
{
	register i;
	/* long l; */                        /* UTS 4/16/86 */
	int init=1;
	int clicks=0;
	int ticks=3;

	while ( --argc > 0 )
	{
		++argv;
		if ( !strcmp( *argv, "-s" ) )
			prton = 1;
		else if ( strcmp (*argv, "-c") == 0)
			cached = 1;
		else if ( ! strcmp( *argv, "-V" ) )
		{
			extern verno, subno;
			printf("xsysmon: version %d.%d\n", verno, subno);
			exit(0);
		}
		else
		{
			ticks= -atoi(*argv);
			if(ticks<5)
				ticks=5;
		}
	}
	nice(5);
	nlist(sys, nl);
	mem = open(core, 0);
	if (mem<0) {
		printf("can't open %s\n", core);
		exit(1);
	}
	mailinit();

	if (getdmdlib(mpath))
		exit(1);
	if (getdmdld(path32ld))
		exit(1);
	strcat(mpath,SYSMON);

	if (access(path32ld,01))
	{
		switch(errno)
		{
		case ENOTDIR:
			fprintf(stderr,"Error: %s contains bad dir\n",path32ld);
			fprintf(stderr," check for correct value of $DMD\n");
			break;
		case EACCES:
			fprintf(stderr,"Error: %s: no permission\n",path32ld);
			break;
		default:
			fprintf(stderr,"Error: cannot access %s errno %d\n",path32ld,errno);
			fprintf(stderr," check for correct value of $DMD\n");
			break;
		}
		exit(1);	/* couldnt stat the path32ld */
	}

	if (access(mpath,04))
	{
		switch(errno)
		{
		case ENOTDIR:
			fprintf(stderr,"Error: %s contains bad dir\n",mpath);
			fprintf(stderr," check for correct value of $DMD\n");
			break;
		case EACCES:
			fprintf(stderr,"Error: %s: no permission\n",mpath);
			break;
		default:
			fprintf(stderr,"Error: cannot access %s errno %d\n",mpath,errno);
			fprintf(stderr," check for correct value of $DMD\n");
			break;
		}
		exit(1);	/* couldnt stat the path32ld */
	}
	if(!boot(mpath))
		exit(1);
#ifdef TIOCEXCL
	ioctl(0, TIOCEXCL, 0);
#endif
	/* get rid of old tty stuff */            /* UTS 4/16/86 */
	/* ioctl(0, TIOCGETP, &sttybuf); */       /* UTS 4/16/86 */
	/* sttybuf.sg_flags|=RAW; */              /* UTS 4/16/86 */
	/* sttybuf.sg_flags&=~ECHO;  */           /* UTS 4/16/86 */
	/* ioctl(0, TIOCSETP, &sttybuf); */       /* UTS 4/16/86 */
	/* replace with termio stuff */           /* UTS 4/16/86 */

	ioctl(0, TCGETA, &pterm) ;                /* UTS 4/16/86 */
	term.c_iflag = IGNBRK ;                   /* UTS 4/16/86 */
	term.c_cflag = (pterm.c_cflag&(CBAUD|CLOCAL))|CS8|CREAD ;      /* UTS 4/16/86 */
	term.c_cc[VMIN] = 1;                       /* UTS 4/16/86 */
	ioctl(0, TCSETAW, &term ) ;                /* UTS 4/16/86 */

	nl[0].n_value &= ~(0x80000000);
	lseek(mem, (long)nl[0].n_value, 0);
	read(mem, &sysinfo, sizeof(sysinfo));
	oldrunque = sysinfo.runque;
	oldrunocc = sysinfo.runocc;
	dotime();
	doload();
	lseek(mem, (long)nl[0].n_value, 0);
	read(mem, &ocp, sizeof(ocp));
	for (;;) {
		sleep(ticks);
		lseek(mem, (long)nl[0].n_value, 0);

#ifdef UTS  /* We are reporting the sum of times for all processors
	     * in the MP system.  This was only tested 
	     * on a VM system.  Transmitting the times in units of
	     * char will probably not be sufficient, but I am only
             * porting this code and not rewritting it.
	     *
	     */

		read(mem, &sysinfo, sizeof(sysinfo)) ;
#	define times sysinfo.cpustats
#	ifdef cpuinfo
		ncp.cp_time[0] =0 ;
		ncp.cp_time[1] =0 ;
		ncp.cp_time[2] =0 ;
		ncp.cp_time[3] =0 ;

		for(ci=0; ci < NCPU; ci++ ) {
			if (times[ci].swtchs) {
				ncp.cp_time[0] += (times[ci].widle >>12)*60LL/1000000LL ;
				ncp.cp_time[1] += (times[ci].utime >>12)*60LL/1000000LL ;
				ncp.cp_time[2] += (times[ci].stime >>12)*60LL/1000000LL;
				ncp.cp_time[3] += ((times[ci].wtime-
						   times[ci].widle) >>12)*60LL/1000000LL ;
			}
		}
#	else /* we are not on a MP based system */
		ncp.cp_time[0] = ( sysinfo.widle >>12) *60LL/1000000LL ;
		ncp.cp_time[1] = ( sysinfo.utime >>12) *60LL/1000000LL ;
		ncp.cp_time[2] = ( sysinfo.stime >>12) *60LL/1000000LL ;
		ncp.cp_time[3] = ((sysinfo.wtime - sysinfo.widle) >>12)
                                  *60LL/1000000LL ;

#	endif cpuinfo
#else /* we are not on a UTS system */
		read(mem, &ncp, sizeof(ncp));
#endif UTS
		ncp.cp_time[4] = ncp.cp_time[0];	/* idle */
		ncp.cp_time[0] = ncp.cp_time[1];	/* user */
		ncp.cp_time[1] = 0;			/* nice */
		for (i=0; i<5; i++) {
			vec[i] = ncp.cp_time[i] - ocp.cp_time[i];
			ocp.cp_time[i] = ncp.cp_time[i];
		}
		if(init)
			init=0;
		else
			write(1, vec, 6);
		if(clicks++ >= 60/ticks){
			char *p;
			dotime();
			doload();	/* must be immediately after dotime() */
			if(p=newmail())
				sendmail(p);
			clicks = 0;
		}
	}
}

dotime(){
	char *p, *ctime();
	long l, time();

	time(&l);
	p = ctime(&l);
	p[16] = 'T';
	write(1, &p[11], 6);
}
doload(){
	long run;
	char buf[16];
	nl[0].n_value &= ~(0x80000000);
	lseek(mem, (long)nl[0].n_value, 0);
	read(mem, &sysinfo, sizeof(sysinfo));
	run = (sysinfo.runque - oldrunque);
	if ( sysinfo.runocc - oldrunocc )
		run = run / (sysinfo.runocc - oldrunocc);
	sprintf(buf, " %ld %c%ld\n", run,
			"-+"[run>oldrun],
				abs(run-oldrun));
	write(1, buf, strlen(buf));
	oldrun = run;
	oldrunque = sysinfo.runque;
	oldrunocc = sysinfo.runocc;
}
boot(s)
	char *s;
{
	if(system(path32ld, "32ld", s))
		return(0);
	return(1);
}
/* change u to uu because of define in user.h */      /* UTS 4/16/86 */
system(s, t, uu )
char *s, *t, *uu ;
{
	int status, pid, l;
	int getuid(), getgid();

	if ((pid = fork()) == 0) {
		if ( setgid(getgid()) == -1 || setuid(getuid()) == -1 )
		{
			printf("can't setgid/setuid");
			return (-1);
		}
		execl(s, t, uu, cached? "-c" : 0, 0);
		_exit(127);
	}
	while ((l = wait(&status)) != pid && l != -1)
		;
	if (l == -1)
		status = -1;
	return(status);
}
/*
 * Stuff for mail
 */

char mailfile[100], line[200], from[100];
long lastsize;

mailinit(){
	char *getlogin(), *getenv();
	char *n;
	struct stat buf;
	n=getenv("MAIL");
	if(n==0){
		n=getlogin();
		if(n==0){
			printf("sysmon: can't determine login name\n");
			exit(1);
		}
		sprintf(mailfile, "/usr/spool/mail/%s", n);
	}else
		strcpy(mailfile, n);
	lastsize = (stat(mailfile, &buf) < 0)? 0 : buf.st_size;
}
char *
newmail(){
	struct stat buf;
	FILE *f;
	char *n, *p, *q, *index();
	int i;

	if(stat(mailfile, &buf)<0)
		return 0;
	if(buf.st_size<=lastsize){
		lastsize = buf.st_size;
		return 0;
	}
	f = fopen(mailfile, "r");
	fseek (f, lastsize, 0);
	lastsize = buf.st_size;
	for ( i=0, p=from; i<100; i++)
		*p++ = '\0';
	strcpy(from, "somebody?\n");
	while (fgets(line, sizeof line, f) != NULL)
	{
		if ( Isfrom(line) ) {
			p=line+5;
			if(line[0] == '>')
				p++;
			for(n=p; *n!=' ' && *n; n++)
				;
			*n++='\n';
			*n=0;
			if(q=index(p, '!')){
				q++;
				while(index(q, '!')){
					p=q;
					q=index(q, '!')+1;
				}
			}
			strncpy(from, p, strlen(p));
		}
		if( prton && equaln(line, "Subject", 7))
			sprintf(index(from,'\n'), " --> %.58s\n", line+9);
	}
	fclose(f);
	return from;

}
sendmail(p)
	char *p;
{
	char buf[6];
	/*  register char *s; */                /* UTS 4/16/86 */
	strncpy(buf, p, 5);
	buf[5] = 'M';
	write(1, buf, 6);
	if(strlen(p) > 5)
		write(1, p+5, strlen(p)-5);
}
/*
 *	Determine if line is genuine article.
 *	Should be of 2 forms:
 *		From.*[0-9][0-9]:[0-9][0-9].*
 *			or
 *		>From.*[0-9][0-9]:[0-9][0-9].*
 *
 *	The [0-9] stuff is the date representation by ctime.
 *	Some systems use only the 09:43 part of the time (i.e.
 *	no seconds representation).
 */
Isfrom(line)
register char *line;
{
	register char *p;

	if(*line == '>')
		line++;
	if(equaln(line, "From ", 5))
	{
		if((p=strchr(line, ':')) != NULL)
		{
colontest:
			if(	isdigit(p[-2]) &&
				isdigit(p[-1]) &&
				isdigit(p[1]) &&
				isdigit(p[2]))      return(1);
		}
		/*
		 *	If more colons, try again.
		 */
		if(p=strchr(++p, ':'))
			goto colontest;
	}
	return(0);
}
getdmdld(str)
char *str;
{
	char *s;

	s = getenv("DMDLD");
	if (s == (char *)0 || s[0] == '\0')
		s = "dmdld";
	strcpy(str, s);
	return(0);
}
getdmdlib(str)
char *str;
{
	char *s;

	s = getenv("DMDLIB");
	if (s == (char *)0 || s[0] == '\0')
	{
		s = getenv("DMD");
		if (s == (char *)0 || s[0] == '\0')
		{
#ifdef DEFDMD
			s = DEFDMD;
#else
			fprintf(stderr,
				"Must have DMD set in your environment\n");
			return(1);
#endif
		}
		strcpy(str, s);
		strcat(str, "/lib");
	}
	else
		strcpy(str, s);
	return(0);
}
