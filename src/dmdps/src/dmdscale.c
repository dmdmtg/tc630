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
#include <pwd.h>
#include "impress.h"
#include "udmd.h"
#include "bit_h.h"

static char	Usage[] =
	"usage:  %s [-w <widthScale>] [-h <heightScale>] [<infile> [<outfile>]]\n";

extern	void	exit();

int	debug = 0;
int	wscale = 1;	/* width scale factor */
int	hscale = 1;	/* height scale factor */

struct bithead bh;	/* stores bitmap header. */

char *argv0;

main(argc, argv)
	int	argc;
	char	**argv;
{
	extern	int	optind;
	extern	char	*optarg;
	FILE	*in, *out;
	int	c;
	
	argv0=argv[0];
	while ((c = getopt (argc, argv, "dh:w:")) != EOF)
		switch (c) {
		case 'd':
			debug = 1;
			break;
		case 'h':
			hscale = atoi(optarg);
			break;
		case 'w':
			wscale = atoi(optarg);
			break;
		default:
			fprintf(stderr, Usage, argv[0]);
			exit(1);
		}
	
	/* default input and output files */
	in=stdin; out=stdout;

	if (optind < argc) {
		/* remaining argument is input file */
		if ((in = fopen(argv[optind], "r")) == NULL) {
			fprintf(stderr, "%s: unable to open %s for input\n",
				argv[0], argv[optind]);
			exit(1);
			}
		optind++;
	}
	if (optind < argc) {
		/* remaining argument is output file */
		if ((out = fopen(argv[optind], "w")) == NULL) {
			fprintf(stderr, "%s: unable to open %s for output\n",
				argv[0], argv[optind]);
			exit(1);
			}
		optind++;
	}
	if (optind < argc) {
		fprintf(stderr, Usage, argv[0]);
		exit(1);
	}

	if (glyphpage(in,out) == EOF)
		fprintf(stderr, "%s: unexpected end of file\n", argv[0]);

	fclose(in);
	fclose(out);
	exit(0);
}


int
glyphpage(in, out)
	FILE *in, *out;
/*
** Read in, convert, and output one bitmap.
*/
{
	int	nrasters,	/* number of scan lines */
		rastwid,	/* width of scan line (as 16-bit words) */
		rastbyt;	/* bytes in scan line */

	if (readhdr(in,&bh) == EOF)
		return(EOF);

	nrasters=bh.nrasters;
	rastwid=bh.rastwid;
	rastbyt = rastwid * 2;

	bh.nrasters=nrasters*hscale;
	bh.rastwid=rastwid*wscale;
	fillhdr(&bh);
	writehdr(out,&bh);

	fprintf(stderr,"%s: input:  type %d, %d bits wide, %d bits long\n",argv0,bh.type,rastbyt*8, nrasters);

	if ( nrasters <= 0 || rastbyt <= 0 )
		fprintf(stderr, "bad size specification.\n");

	if (rastbyt * wscale > MAXWID) {
		fprintf(stderr, "too wide (%d bits after *%d scaling)\n",
		  rastbyt*8 * wscale, wscale);
		return	EOF;
	}
	fprintf(stderr,"%s: output: type %d, %d bits wide, %d bits long\n",argv0,bh.type,bh.rastwid*16, bh.nrasters);

	for (; nrasters > 0; nrasters--) {
		static short	rastline[MAXWID],  outrast[MAXWID];
		int	outwid;	/* # of significant bytes in 'outrast' */
		int	count;

		if (getrast(in, rastline, rastwid) == EOF)
			return	EOF;

		scalerast(rastline,rastwid,wscale,outrast,&outwid);

		for (count = 0; count < hscale; count++)
			putrast(out, outrast, outwid);
	}

	return(0);
}


scalerast(dataline, Width, scale, outline, outwidp)
	short	dataline[];	/* input data */
	int	Width;	/* width of dataline */
	int	scale;
	short	outline[];	/* OUT: buffer for output */
	int	*outwidp;	/* OUT: width of 'outline' */
{
	register int	bitWidth = Width * 16;
	register int	i,j;

	if (bitWidth/16 * scale >= MAXWID) {
		fprintf(stderr, "too wide to expand by %d\n",bitWidth,scale);
		return;
	}

	for (i = 0; i < (Width * scale); i++)	/* clear output buffer */
		outline[i] = 0;

#define bit_is_set(dataline,i) 	(( ((char *)dataline)[i/8] ) &  (0x80 >> (i%8))) 
#define set_bit(outline,i) 	(( ((char *) outline)[j/8] ) |= (0x80 >> (j%8)))

	/* expand dataline to rawbuf by 'scale' factor */
	for (i = 0; i < bitWidth; i++) {
		if (bit_is_set(dataline,i) ) {
			register limit = (i+1) * scale;
			for (j = i*scale; j < limit; j++)
				set_bit(outline,i);
		}
	}

	*outwidp = bitWidth/16 * scale;	/* size of raw output */
}
