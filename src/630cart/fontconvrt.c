/*       Copyright (c) 1989 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define MAXBITS		50000	/* maximum size of bitmap image */
#define MAXCHARS	260	/* maximum number of characters in the font */
#define FONTCHARSIZE	8	/* WARNING! Not == sizeof(struct Fontchar)
				   because of compiler wierdness */
#define WORDSIZE	32	/* bitmap width is rounded up to WORDSIZE
				   bits */

typedef struct Font
{
	short n;
	unsigned char height;
	unsigned char ascent;
	long unused;
} Font;

typedef struct Fontchar
{
	short x;
	unsigned char top;
	unsigned char bottom;
	char left;
	unsigned char width;
	char buf[2];
} Fontchar;

short bits[MAXBITS];	/* used to store the font's bit image */
int numbits;		/* the number of bits (shorts) used */
Font f;			/* the font */
Fontchar fc[MAXCHARS];	/* the font's character information */
char *progname;		/* the name of this program */
char *name;		/* the name of the font */
int hflag;		/* if present, don't write out include file directives */
int pid;		/* my process id, used to make things unique */


main(argc, argv)
int argc;
char **argv;
{
	FILE *fin = stdin;	/* default to standard input */
	FILE *fout = stdout;	/* default to standard output */
	int numfiles = 0;	/* number of files in command line */
	int inflag = 'j';	/* default to reading jf file */
	int outflag = 'c';	/* default to writing C file */
	int numflags = 0;	/* number of flags specified */

	progname = argv[0];
	pid = getpid();
	name = "defont";
	while(--argc) {
		argv++;
		if(argv[0][0] == '-') {
			switch(argv[0][1]) {
			case 'h':
				hflag++;
				break;
			case 'c': /* C source file */
			case 'j': /* jf (jerq) file format */
				switch(numflags++) {
				case 0: inflag = argv[0][1]; break;
				case 1: outflag = argv[0][1]; break;
				default: usage();
				}
				break;
			case 'n': /* give the font a name */
				if(argv[0][2])
					name = argv[0] + 2;
				else if(--argc) {
					name = argv[1];
					argv++;
				} else
					usage();
				break;
			case '\0': /* use stdin or stdout */
				switch(numfiles++) {
				case 0: /* input */
					fin = stdin;
					break;
				case 1: /* output */
					fout = stdout;
					break;
				default: /* too many */
					usage();
				}
				break;
			default:
				usage();
			}
		}
		else 
			switch(numfiles++) {
			case 0: /* input file */
				if((fin = fopen(argv[0], "r")) == 0)
					error("Error, can't open input");
				break;
			case 1: /* output file */
				if((fout = fopen(argv[0], "w")) == 0)
					error("Error, can't open output");
				break;
			default: /* too many */
				usage();
			}
	}
	switch(inflag) {
	case 'c': /* read a C file */
		readc(fin);
		break;
	case 'j': /* read a jf file */
		readj(fin);
		break;
	}
	switch(outflag) {
	case 'c': /* write a C file */
		writec(fout);
		break;
	case 'j': /* write a jf file */
		writej(fout);
		break;
	}
	exit(0);
}

readc(fin)
FILE *fin;
{
	char line[256];
	int d[8];
	int i, j;

	while((j = getline(fin, line)) >= 0)
		if(strchr(line, '{'))
			break;
	if(j == -1)
		error("Error on input, no '{' in file");
	while((j = getline(fin, line)) >= 0)
		if(sscanf(line,
		   "0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", &d[0],
		   &d[1], &d[2], &d[3], &d[4], &d[5], &d[6], &d[7]) == 8)
			for(i=0; i<8; i++)
			{
				bits[numbits++] = d[i];
				if(numbits >= MAXBITS)
					error(
					   "Error on input, bit image too big");
			}
		else if(strchr(line, '}'))
			break;
	if(j == -1)
		error("Error on input, no '}' in file");
	while((j = getline(fin,line)) >= 0)
		if(strchr(line, '='))
			break;
	if(j == -1)
		error("Error on input, no '=' for Bitmap");
	while((j = getline(fin,line)) >= 0)
		if(strchr(line, '='))
			break;
	if(j == -1)
		error("Error on input, no '=' for Font");
	while((j = getline(fin, line)) >= 0)
		if(sscanf(line, " %d,%d,%d", &d[0], &d[1], &d[2]) == 3)
		{
			f.n = d[0] + 1;
			f.height = d[1];
			f.ascent = d[2];
			break;
		}
	if(j == -1)
		error(
		   "Error on input, missing declaration of n, height, ascent");
	if(f.n >= MAXCHARS)
		error("Error on input, to many characters in font");
	i = 0;
	while((j = getline(fin, line)) >= 0)
                if(sscanf(line, " {%d,%d,%d,%d,%d", &d[0], &d[1], &d[2], &d[3],
			&d[4]) == 5)
		{
			fc[i].x = d[0];
			fc[i].top = d[1];
			fc[i].bottom = d[2];
			fc[i].left = d[3];
			fc[i].width = d[4];
			if(++i > f.n)
				break;
		}
	if(j == -1) {
		fprintf(stderr, "i = %d, should be %d\n", i, f.n);
		error("Error on input, missing Fontchar(s)");
	}
}

getline(f, s)
FILE *f;
char *s;
{
	char *t;

	t = fgets(s, 256, f);
	if(t == (char *)0)
		return(-1);
	return(strlen(s));
}

error(s)
char *s;
{
	fprintf(stderr, "%s: %s\n", progname, s);
	exit(1);
}


writej(fout)
FILE *fout;
{
	short n;
	long l;
	int width;

	l = 0;
	n = f.n;
	width = ((fc[n].x+31)&~0x1F)/8;
#ifdef vax
	doswab(n);
#endif
	if(write(fileno(fout), &f.n, 4) != 4)
		error("Error on output, can't write font header");
	if(write(fileno(fout), &l, 4) != 4)
		error("Error on output, can't write font header");
	if(write(fileno(fout), fc, (n+1) * 8) != (n+1)*8)
		error("Error on output, can't write character information");
	if(write(fileno(fout), bits, f.height * width) != f.height * width)
		error("Error on output, can't write bitmap image");
}

#ifdef vax
doswab(n)
short n;
{
	int i, h, w;
	char tmpbuf[2];

	h = f.height;
	w = ((fc[n].x+31)&~0x1F)/8;
	swab(&f.n, tmpbuf, 2);
	memcpy(&f.n, tmpbuf, 2);
	for(i=0; i<=n; i++)
	{
		swab(&fc[i].x, tmpbuf, 2);
		memcpy(&fc[i].x, tmpbuf, 2);
	}
	for(i=0; i<h*w/2; i++)
	{
		swab(&bits[i], tmpbuf, 2);
		memcpy(&bits[i], tmpbuf, 2);
	}
}
#endif



readj(fin)
FILE *fin;
{
	char tmpbuf[2];
	int xmax, i;

	if( read(fileno(fin),&f,sizeof(f)) <= 0 )
		error("Error on input, can't read font header");
#ifdef vax
	swab(&f.n, tmpbuf, 2);
	memcpy(&f.n, tmpbuf, 2);
#endif

	if(f.n >= MAXCHARS)
		error("Error on input, to many characters in font");
	for( i=0; i<f.n+1; i++ )
		if( read(fileno(fin),&fc[i],FONTCHARSIZE) <= 0 )
			error("Error on input, can't read character info entry");
#ifdef vax
		else {
			swab(&fc[i].x, tmpbuf, 2);
			memcpy(&fc[i].x, tmpbuf, 2);
		};
#endif

	xmax = fc[f.n].x + WORDSIZE - (fc[f.n].x % WORDSIZE);
	if((f.height*xmax)/16 >= MAXBITS)
		error("Error on input, bitmap image too big");
	/* round to next word boundary */
	for( numbits=0; numbits<(f.height*xmax)/16; numbits++ )
	/* read bitmap as a large array of shorts (always 16 bits on WE-32000 and 68000) */
	 {
		if( read(fileno(fin),&bits[numbits],2) <= 0 )
			error("Can't read raster images");
#ifdef vax
		swab(&bits[numbits], tmpbuf, 2);
		memcpy(&bits[numbits], tmpbuf, 2);
#endif
	}
}

writec(fout)
FILE *fout;
{
	int i, xmax;

	xmax = (fc[f.n].x + 31)&~0x1F;
	if(!hflag)
		fprintf(fout, "\n#include <dmd.h>\n#include <font.h>\n\n");
	fprintf(fout, "static short bits%d[] = {\n", pid);
	for(i=0; i<f.height * xmax / 16; i++)
		fprintf(fout, "0x%X,%c",(unsigned short)bits[i],
		   ( (i+1)%8 ? '\t' : '\n' ));
	fprintf(fout, "};\n");
	fprintf(fout, " static Bitmap strike%d = {\n\
		(Word *) bits%d,\n\
		%d / WORDSIZE,\n\
		0, 0, %d, %d,\n\
		0,\n\
	};\n\n", pid, pid, xmax, xmax-1, f.height);

	fprintf(fout, " struct {\n\
		short n;\n\
		char height;\n\
		char ascent;\n\
		long unused;\n\
		Bitmap *bits;\n\
		Fontchar info[%d];\n\
	} %s = {\n\
			%d, %d, %d, %ld, &strike%d,\n\
			{\n",
	    f.n+1,name,f.n-1,f.height,f.ascent,f.unused,pid);
	for(i=0; i<=f.n; i++)
		fprintf(fout, "\t\t\t  { %d, %d, %d, %d, %d },\n", fc[i].x,
		    fc[i].top, fc[i].bottom, fc[i].left, fc[i].width);
	fprintf(fout, "\t}\n};\n");
	if(ferror(fout))
		error("Error on output");
}


usage()
{
	fprintf(stderr, "Usage: %s [-cj] [infile] [-jc] [outfile] [-n name]\n",
	    progname);
	exit(1);
}

