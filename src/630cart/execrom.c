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
#include <stdio.h>
#include <sys/types.h>
#include <filehdr.h>
#include <scnhdr.h>
#include <string.h>
#include <fcntl.h>

/*******************************************************************
**
**	author: Ish Niazi
**
**	date: 4/23/88
**
**	This program takes a "c" language  source file and appends
**	the logic to initialize the Appl structure used for caching.
**
**	The output of this program is to be utilized by the make_cart
**	scripts that link (locates) the applications at the correct
**	cartridge address in the 630 MTG.
**
**
*********************************************************************/

#include "cartglob.h"

char newoutfile[40];

main(argc, argv)
int argc;
char **argv;
{
	int i, c;
	struct scnhdr scnhdr;
	struct filehdr filehdr;
	extern char *optarg;
	extern int optind, opterr;

	if(argc < 2)
		exit(0);

	strcpy(shared, "PERMANENT");	/*ROM appls perm but shre optional*/
	cartopt(argc,argv);

	/*
	** start outputing the initialization logic for Appl structure.
	*/
	if (!nflag) {
	   printglobals();
  	   printinit();
	}


	fprintf(realstdout, "%s null init_appl 2 %01d %s\n", infile,dflag,newoutfile);


	exit(0);
}

cartopt(argc,argv)
int argc;
char **argv;
{
	int i,c,fd,len;
	char chr,*cp;

	progname = argv[0];
	infile = argv[1];
	opterr = 1;	/*disable optional error message display */
	optind = 2;	/*start with second argument*/
	while((c = getopt(argc, argv, "dm:M:bntTcClLsSr:R:v:V:z:Z:i:I:")) != EOF)
		switch(c) {
		case 'b':
		case 'B':
			bflag++;
			break;
		case 'd':
			dflag++;
			break;
		case 'm':
		case 'M':
			strcpy(menu_name, optarg);
			mflag++;
			break;
		case 'n':
			nflag++;
			break;
		case 't':
		case 'T':
			tflag++;	/*shared application*/
			bflag++;	/*per cache can't clear BSS */
			strcat(shared, "|SHARED");
			break;
		case 'c':
		case 'C':
			if(lflag) {
				fprintf(stderr, "%s: both 'L' and 'C' flag specifiled for file '%s'\n",
					argv[0], infile);
				exit(1);
			}
			cflag++;
			break;
		case 'l':
		case 'L':
			if(cflag) {
				fprintf(stderr, "%s: both 'L' and 'C' flag specifiled for file '%s'\n",
					argv[0], infile);
				exit(1);
			}
			lflag++;
			break;
		case 's':
		case 'S':
			sflag++;
			break;
		case 'r':
		case 'R':
			if(sscanf(optarg, "%d,%d", &width, &height) != 2)
			{
				fprintf(stderr, "%s: default rectangle given bad argument\n",
				    argv[0]);
				exit(1);
			}
			rflag++;
			break;
		case 'v':
		case 'V':
			vflag++;
			argvs = optarg;
			break;
		case 'z':
		case 'Z':
/*
			if ( (len = strlen(optarg)) > 8 ) {
			   fprintf(stderr,"%s: stack: more than 8 characters in input\n", argv[0]);
			   exit(1);
			}
*/
			cp=optarg;
			if ( *cp == '+' || *cp == '-') cp++;
			while ( (chr = *cp++) ) {
			   if ( chr <  '0' || chr >  '9' ) {
			   	fprintf(stderr,"%s: stack: illegal character\n", argv[0]);
			   	exit(1);
			   }
			}
			zflag=atol(optarg);
/*
			if ( zflag > 9999999l  ) {
		 	   fprintf(stderr,"%s: stack: value out of bound\n", argv[0]);
			   exit(1);
			}
*/
			break;	
		case 'i':
		case 'I':
			iflag++;
			strcpy(istring, optarg);
			break;
		case '?':
			fprintf(stderr, "%s: bad option\n", argv[0]);
			exit(1);
			break;
		}

	/* done parsing options, now get to work */

/* **** NO READ ANYMORE freopen(infile, "r", stdin); */

	if((realstdout = fdopen(fd = dup(1), "w")) == 0)
		error("couldn't copy standard out");
	fcntl(fd, F_SETFD, 1);	/* close on exec */
	if(strrchr(infile, '/'))
		strcpy(outfile, strrchr(infile, '/') + 1);
	else
		strcpy(outfile, infile);
	if(!vflag)
	{
		argvs = argvbuf;
		strcpy(argvbuf, "\"");
		strcat(argvbuf, outfile);
		strcat(argvbuf, "\"");
	}
	if(!mflag)
		strcpy(menu_name, outfile);

/* useless next 3 lines, but leave them as be */
	if(strlen(outfile) > 6)
		outfile[6] = '\0';

/*
** The next few lines were added to that a new file is created
** instead of editing an existing file.
**  -jrg
*/
	strcpy(newoutfile, "/tmp/");
	strcat(newoutfile, outfile);
	strcat(newoutfile, "XXXXXX");
	mktemp(newoutfile);
	strcat(newoutfile, ".c");
	freopen(newoutfile, "w", stdout);

/* Next four lines of code not needed anymore */
	strcpy(ofile, outfile);
	ofile[strlen(ofile) - 1] = 'o';
	strcpy(Cfile, ofile);
	Cfile[strlen(Cfile) - 1] = 'C';

	if(!rflag)
		sflag++;	/* default to sweepable */
}

