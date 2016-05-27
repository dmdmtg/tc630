/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


/*
**	getpeid (prints programmable environemnt id of 
**	a 630 MTG (68000) object file.
*/


/*
** swapw	words must be swapped between host and Motorola 68000
** swapb	bytes must be swapped between host and Motorola 68000
*/

#ifdef	pdp11
#define	swapb	1
#define	swapw	0
#endif
#ifdef	vax
#define	swapb	1
#define	swapw	1
#endif
#ifdef	u3b
#define	swapb	0
#define	swapw	0
#endif
#ifdef	u3b2
#define	swapb	0
#define	swapw	0
#endif
#ifdef	u3b5
#define	swapb	0
#define	swapw	0
#endif
#ifdef  uts
#define swapb   0
#define swapw   0
#endif

/* objectfile must have text, data, and bss in following positions.
 * other noload sections following do not matter
 */
#define TEXTSECT 0
#define DATASECT 1
#define BSSSECT 2


#include <fcntl.h>
#include <termio.h>
#include <filehdr.h>
#include <scnhdr.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#define NULL 0  /* use stdio.h rather than dmd.h definition of NULL */



/* System dependencies */
#define NSECTS  12		/* maximum number of sections in m68a.out */
#ifndef MC68MAGIC		/* usually defined in fcntl.h */
#define MC68MAGIC 0x150		/* magic number for target machine, here MC68000 */
#endif

/* System functions and variables */
char *getenv(), *strcpy(), *malloc();
int open(), access();
extern int errno;


char Usage[] = "Usage: %s objectfile\r\n";

char *name, *e2s1, *e2s2;
struct filehdr fileheader;
struct scnhdr secthdrs[NSECTS];

int peidsect = -1;		/* peid section (if it exists) */

 
int	obj;		/* File descriptor for object file */
long	location;
char	file[128];	/* Name of file */
char	*curfile;
int	nargchars;	/* Number of characters, including nulls, in args */
long	longbuf[3];
int 	errflag;	/* cannot access or open objectfile */



main(argc, argv)
register int argc;
register char *argv[];
{
	register char *cp;	/* scratch char pointer */
	register int i;		/* scratch register */
	char c;			/* no register, &c will be used */
	long largc;
	extern char *optarg;
	extern int optind;

/*	
**	Get the command line arguments
**
*/

	name = *argv;


	if (argc != 2) {
		fprintf (stderr, "%s: ", name);
		fprintf (stderr, Usage, name);
		exit (2);
	}


/*
**	Check the accessibility of the file whose peid is desired.
*/
	errflag = 0;	/* redundant, but safe */
	optind=1;	/* start from argument 1*/
	curfile=argv[optind];
	if (jpath(argv[optind], access, 4)!=0)
		errflag = 1;

	if (!errflag) {
		obj = jpath(argv[optind], open, 0);
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
		Read (&fileheader, sizeof(struct filehdr));
		if (fileheader.f_magic!=MC68MAGIC)
			error2(0, "'%s' is not a MC68000 family a.out", curfile);

		if (fileheader.f_nscns > NSECTS)
			error2(0,"%s exceeds max number of sections", curfile);

/* Don't need it to be executable ????????? */
		if(!(fileheader.f_flags&F_EXEC))
			error2(0, "%s is not an executable file", curfile);


		for (i = 0 ; i < fileheader.f_nscns ; ++i) {
				
			/* make sure first 3 sections are text, data,
			** and bss in that order.
			*/
		
			Read (&secthdrs[i], sizeof(struct scnhdr));
				switch (i) {
				case TEXTSECT:
			    	if (!strequal(secthdrs[i].s_name,".text"))
				    error2(0, "%s not in proper text, data, bss format, missing: text", curfile);
				break;

				case DATASECT:
		  	    	if (!strequal(secthdrs[i].s_name,".data"))
				    error2(0, "%s not in proper text, data, bss format, missing: data", curfile);

		    		break;

				case BSSSECT:
		  		if (!strequal(secthdrs[i].s_name,".bss"))
			            error2(0, "%s not in proper text, data, bss format, missing: bss", curfile);
	
				break;

				default:
			 	if (strequal(secthdrs[i].s_name,".peid"))
					peidsect = i;
				break;

				}

		}
	}

              if (errflag == 1)
                  error (0, "cannot access '%s'", argv[optind]);
              else if (errflag == 2)
                  error (0, "cannot open '%s'", argv[optind]);
              else if (errflag == 3)
                   error (0, e2s1, e2s2);

	/* printf("piedsect =%3d\n",peidsect); */

	largc=getpeid();
	printf("%05d\n",largc);
	exit(0);

}/* end main*/

/*
**	Look for the object file whose peid is desired.
*/
jpath (f, fn, a)
register char *f;
register int (*fn)();
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
			if((o=(*fn)(file, a))!=-1)
				return o;
		}
	}
	return((*fn)(strcpy(file, f), a));
}



/* Read -       read from the file whose peid is desired.
*/
int
Read(a, n)
        char *a;
{
        register i;
        i=read(obj, a, n);
        if(i<0)
                error(1, "read error on '%s'", curfile);
        return(i);
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
	long thispeid = 0;

	if  (peidsect > -1) {
		offset=lseek(obj,0,1);
		lseek(obj,secthdrs[peidsect].s_scnptr,0);
		while (count <secthdrs[peidsect].s_size/sizeof(long) ) {
			Read(&thispeid,sizeof(long));
			if (thispeid>highestpeid) highestpeid=thispeid;
			count++;
		}
		
		lseek(obj,offset,0);
	}
	return(highestpeid);
}

error (pflag, s1, s2)
int pflag;
char *s1, *s2;
{
        long flushval = 0L;
        register int    n;
        register int    saverrno;
        char            buf[BUFSIZ];
        extern int      errno;

        saverrno = errno;

        if(pflag){
                errno=saverrno;
                perror(s2);
        }
        fprintf(stderr, "\n%s: ", name);
        fprintf(stderr, s1, s2);
        fprintf(stderr, "\r\n");
/*      if(psflag)
                pstats(stderr);	*/
        exit(1);

}

error2(i, s1, s2)
int i;
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

