/* */
/*									*/
/*	Copyright (c) 1987,1988,1989,1990,1991,1992   AT&T		*/
/*			All Rights Reserved				*/
/*									*/
/*	  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.		*/
/*	    The copyright notice above does not evidence any		*/
/*	   actual or intended publication of such source code.		*/
/*									*/
/* */
/* Exchange the first two shorts of the .data section because the convert */
/* programs don't know that it's special for the 630.  Used when copying  */
/* 630 executables from one machine to another with reversed byte orders. */
/* Dave Dykstra, 7/28/92.						  */


#include <stdio.h>
#include <filehdr.h>
#include <scnhdr.h>
#include <fcntl.h>

/*
 *   Magic Numbers
*/

	/* Motorola 68000 */
#ifndef MC68MAGIC
#define	MC68MAGIC	0520
#endif

extern short verno, subno;
extern int optind;
extern char *optarg;

main(argc,argv)
int argc;
char **argv;
{
    int fd;
    long c;
    int i;
    int exitcode = 0;
    int eflag,c1;
    int fflag;
    struct filehdr fileheader;
    struct scnhdr sectionheader;

    eflag = 0;
    fflag = 0;
    while ((c1 = getopt(argc, argv, "fV")) != EOF) {
      switch (c1) {
	case 'V':
	    printf("%s: version %d.%d\n", argv[0],verno, subno);
	    exit(0);

	case 'f':
	    fflag++;
	    break;

	case '?':
	    eflag++;
	    break;
      }
    }

    if (eflag || optind >= argc ) {
       fprintf(stderr, "Usage: zap630 [-f] [-V] objectfiles \n");
       exit(2);
    }

    argc -= (optind - 1);
    argv += (optind - 1);

    while(--argc)
    {
	++argv;
	exitcode++;

	if ((fd = open(*argv,O_RDWR)) < 0) {
	    fprintf(stderr,"zap630: cannot open %s for read/write\n",*argv);
	    continue;
	}
	if (read(fd, &fileheader, sizeof(struct filehdr)) <= 0) {
	    fprintf(stderr,"zap630: cannot read %s file header\n",*argv);
	    goto cont;
	}
	if (fileheader.f_magic != MC68MAGIC) {
	    fprintf(stderr,"zap630: %s is not a MC68000 family a.out\n",*argv);
	    goto cont;
	}
	if (!(fileheader.f_flags & F_EXEC)) {
	    fprintf(stderr,"zap630: %s is not an executable file\n",*argv);
	    goto cont;
	}
	for (i = 0 ; i < (int) fileheader.f_nscns; ++i) {
	    if (read (fd, &sectionheader, sizeof(struct scnhdr)) <= 0) {
		fprintf(stderr,"zap630: cannot read %s section header\n",*argv);
		goto cont;
	    }
	    if(strcmp(sectionheader.s_name,".data") == 0) {
		goto gotit;
	    }
	}
	fprintf(stderr,"zap630: .data section not found in %s\n",*argv);
	goto cont;
gotit:
	if (lseek(fd, (long) sectionheader.s_scnptr, 0) <= 0) {
	    fprintf(stderr,"zap630: cannot seek in %s to byte %d\n",*argv,
					sectionheader.s_scnptr);
	    goto cont;
	}
	c = 0;
	if (read(fd, &c, 4) <= 0)
	{
	    fprintf(stderr,"zap630: cannot read first 4 bytes in .data of %s\n",
								*argv);
	    goto cont;
	}
	if (!fflag && ((c <= 0x100000) && (c > 0x10))) {
	    fprintf(stderr,
	     "zap630: %s: stack size was already reasonable (%d), not zapped\n",
							    *argv,c);
	    goto cont;
	}
	c = (c >> 16) + (c << 16);
	if (lseek(fd, (long) sectionheader.s_scnptr, 0) <= 0) {
	    fprintf(stderr,"zap630: cannot seek in %s to byte %d\n",*argv,
					sectionheader.s_scnptr);
	    goto cont;
	}
	if (write(fd, &c, 4) <= 0)
	{
	    fprintf(stderr,"zap630: cannot write first 4 bytes in .data of %s\n",
								*argv);
	    goto cont;
	}
	exitcode--;
	printf("zap630: stack size successfully zapped to %d in %s\n",c,*argv);
cont:
	close(fd);
    }
    exit(exitcode);
}

