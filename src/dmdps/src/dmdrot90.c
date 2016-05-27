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
#include "udmd.h"
#include "bit_h.h"

/*
 * Rotate a bitmap 90 degrees clockwise.
 */

static char	Usage[] =
	"usage:  %s [ <infile> [ <outfile> ] ]\n";

int	debug = 0;
struct	bithead	bh1,bh2;
char	*argv0;

main(argc, argv)
int	argc;
char	**argv;
	{
	extern	int	optind;
	extern	char	*optarg;
	FILE	*in, *out;
	int	c;
	
	argv0 = argv[0];
	while ((c = getopt (argc, argv, "d")) != EOF)
		switch (c) {
		case 'd':
			debug = 1;
			break;

		default:
			fprintf(stderr, Usage, argv0);
			exit(1);
		}
	
	/*
	 * Default input and output files
	 */
	in = stdin; out = stdout;

	if (optind < argc) {
		/*
		 * Next argument is input file
		 */
		if ((in = fopen(argv[optind], "r")) == NULL) {
			fprintf(stderr, "%s: unable to open %s for input\n", 
			    argv0, argv[optind]);
			exit(1);
		}
		optind++;
	}
	if (optind < argc) {
		/*
		 * Remaining argument is output file
		 */
		if ((out = fopen(argv[optind], "w")) == NULL) {
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

	if (rot(in, out) == EOF)
		fprintf(stderr, "%s: unexpected end of file\n", argv0);

	exit(0);
}

short	align;	/* to align the following buffer */
unsigned char	rastline[2*MAXWID];

/*
 * Read in, convert, and output one rotated bitmap.
 */
int
rot(in, out)
FILE	*in, *out;
	{
	unsigned char	srcbit, destbit;
	register int	row, col, i, bitno;
	char	bitmp[40];
	register char	*bitmap;
	char	*malloc();
	int	j, k;
	long	offset;
	FILE	*temp;
	int	nrasters, 	/* number of scan lines */
		rastwid, 	/* width of scan line (as 16-bit words) */
		rastbyt;	/* bytes in scan line */
	int	length,		/* Same for output, rotated ... */
		widthw,		/*	     ....		*/
		widthb;		/*	  ... bitmap.		*/

	if (readhdr(in,&bh1)==EOF)
		return(EOF);

	nrasters=bh1.nrasters;
	rastwid=bh1.rastwid;

	rastbyt = rastwid * 2;
	/*
	 * Determine length and width(s) of rotated bitmap.
	 */
	length = rastbyt * 8;
	widthw = (nrasters + 15) / 16;
	widthb = 2 * widthw;

	fprintf(stderr, "%s: input:   %4d bits wide; %4d bits long\n", 
	    argv0, rastbyt * 8, nrasters);

	bh2.type=bh1.type;
	bh2.rastwid=widthw;
	bh2.nrasters=length;
	fillhdr(&bh2);
	writehdr(out,&bh2);

	fprintf(stderr, "%s: output:  %4d bits wide; %4d bits long\n", 
	    argv0, widthb * 8, length);

	if (nrasters <= 0 || rastbyt <= 0)
		fprintf(stderr, "%s: bad size specification.\n",argv0);
	/*
	 * Try to malloc enough memory to hold the unpacked bitmap.
	 * If that fails, create a temporary, unpacked file containing
	 * the input bitmap.  This temp file is then read column-wise and
	 * written row-wise (chunked) to rotate the bitmap.  (No
	 * one said it would be efficient...)
	 */
	k = nrasters * rastbyt;
	bitmap = malloc(k);
	if (bitmap)
		fprintf(stderr, "Got enough memory for bitmap (%d bytes)\n", k);
	else {
		fprintf(stderr, "Have to use work file (%d bytes)\n", k);
		sprintf(bitmp, "/tmp/bit.%d", getpid());
		temp = fopen(bitmp, "w");
		if ( ! temp) {
			fprintf(stderr, "No temp output file\n");
			exit(1);
		}
	}
	for (i = 0; i < nrasters; i++) {

		if (getrast(in, rastline, rastwid) == EOF)
			return EOF;

		if (bitmap) {
			k = i * rastbyt;
			for (j = 0; j < rastbyt; j++)
				bitmap[k + j] = rastline[j];
		} else
			fwrite(rastline, 2, rastwid, temp);
	}
	/*
	 * If the length of the input bitmap is not a multiple of 16,
	 * pad it out.
	if (nrasters % 16) {
		for (i = 0; i < rastbyt; i++)
			rastline[i] = '\0';
		for (i = 16 - nrasters%16; i > 0; i--) {
			if (debug)
				fprintf(stderr, "Pad length - %2d\n", i);
			fwrite(rastline, 2, rastwid, temp);
		}
	}
	 */
	if ( ! bitmap) {
		temp = freopen(bitmp, "r", temp);
		if ( ! temp) {
			fprintf(stderr, "No temp input file\n");
			exit(1);
		}
		unlink(bitmp);
	}
	/*
	 * Now read bitmap or temp column-wise and build a 
	 * run encoded output row.
	 */
	for (col = 0; col < (rastbyt * 8); col++) {
		/*
		 * Scrub the output buffer.
		 */
		for (i = 0; i < widthb; i++)
			rastline[i] = '\0';

		for (row = 0; row < nrasters; row++) {
			/*
			 * Find the byte in the temp file.
			 */
			offset = row*rastbyt + col/8;
			if (debug)
				fprintf(stderr, "Row: %4d, col: %4d, offset: %5d, ",
				    row, col, offset);
			if ( ! bitmap) {
				if (fseek(temp, offset, 0) < 0) {
					fprintf(stderr, "Fseek failed\n");
					exit();
				}
				srcbit = getc(temp);
			} else
				srcbit = bitmap[offset];
			/*
			 * Isolate the bit in question in that byte.
			 *
			 * col%8:  0  1  2  3  4  5  6  7
			 * srcbit: 7  6  5  4  3  2  1  0
			 */
			bitno = 7 - (col%8);
			srcbit &= 1 << bitno;
			if (debug)
				fprintf(stderr, "bitno: %d, srcbit: %02x",
				    bitno, srcbit);
			/*
			 * If the bit in the srcbit is set, 'or' in the
			 * corresponding bit into the output buffer.
			 */
			if (srcbit) {
				i = widthb-(row/8)-1;
				bitno = row%8;
				destbit = 1 << bitno;
				if (debug)
					fprintf(stderr, ", oring %02x in [%d]",
					    destbit, i);
				rastline[i] |= destbit;
			}
			if (debug)
				putc('\n', stderr);
		}
		/*
		 * Now write the newly rotated row.
		 */
		if (debug)
			fprintf(stderr, "Wrast: %x, %d\n", rastline, widthw);
		putrast(out, rastline, widthw);
	}
	return(0);
}
