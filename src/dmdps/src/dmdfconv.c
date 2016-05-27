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
/*
 *
 *	bitmap convert filter  --  convert/convert.c
 *
 *	convert a bitmap from DMDPS format to BITFILE.
 *
 *
 */

#include <stdio.h>
#include "udmd.h"
#include "bit_h.h"

#define Usage "%s: [-n | -o] [ infile [ outfile ]]\n"

short buffer[MAXWID];

struct bithead bh;

main( argc,argv )
	int argc;
	char **argv;
{
	int c;
	FILE *src = stdin;
	FILE *dst = stdout;

	extern int optind;
	char errflg=0;
	char nflag=0;
	char oflag=0;
	char *argv0;

	argv0=argv[0];
	
	while ((c = getopt (argc, argv, "on")) != EOF)
		switch (c) {
		case 'n': nflag++;
			break;
		case 'o': oflag++;
			break;
		case '?': errflg++;
			break;
		}
	if (errflg || ( (nflag + oflag) > 1) ) {
		fprintf(stderr,Usage,argv0);
		exit(1);
	}

	src = stdin; dst = stdout;

	if (optind < argc) {
		if ((src = fopen(argv[optind], "r")) == NULL) {
			fprintf(stderr, "%s: unable to open %s for input\n", 
			    argv0, argv[optind]);
			exit(1);
		}
		optind++;
	}
	if (optind < argc) {
		if ((dst = fopen(argv[optind], "w")) == NULL) {
			fprintf(stderr, "%s: unable to open %s for output\n", 
			    argv0, argv[optind]);
			exit(1);
		}
		optind++;
	}
	if (optind < argc) {
		fprintf(stderr, Usage, argv0);
		exit(1);
	}
	     
	if (readhdr( src, &bh ) == EOF) {
		fprintf(stderr,"%s: can't read header.\n",argv0);
		exit(1);
	} else 
	fprintf(stderr,"%s: input:  type %d, %d bits wide, %d bits long\n",argv0,bh.type,bh.rastwid*16,bh.nrasters);
	if ( (bh.type == NEWFMT) && !nflag )
		bh.type=OLDFMT;
	else if ( (bh.type == OLDFMT) && !oflag )
		bh.type=NEWFMT;
	writehdr(dst, &bh);
	fprintf(stderr,"%s: output: type %d, %d bits wide, %d bits long\n",argv0,bh.type,bh.rastwid*16,bh.nrasters);
	convert(src,dst);
	exit(0);
}

convert(in,out)	
FILE *in,*out;
{
	register i;

	for ( i=0 ; i<bh.nrasters; i++ ) { 
		if ( getrast( in, (short *)buffer,bh.rastwid) ==EOF ) break;
		putrast( out,(short *)buffer,bh.rastwid);
	}
}
