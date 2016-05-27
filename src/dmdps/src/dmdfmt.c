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

#define	Usage	"Usage: %s [ -T dots | ci8510b | 450 | hex | printronix | QMS | i300 | i300 | thinkjet | laserjet | fx-80 ] [ file ]\n"

#define Printers "dots - dot format for line printer\nhex - hex format (for icons)\n450 - ploting format for diablo 450 type\nprintronix - printronix printer format\nqms - QMS lasergraphics format\nfx-80 - epson FX-80 format\nci8510b - C.Itoh 8510b format\nlaserjet - HP 2686A (LaserJet)\nhpink - HP 2225C (ThinkJet)\ni300, i240 - imagen impress format (default)\n" 

#ifndef min
#define min(a,b)	((a<=b) ? a : b)
#endif

#define WHITE 1
#define BLACK 0

extern	int	optind;
extern	char	*optarg;
extern	struct	passwd *getpwuid();
extern	char	*getlogin(), *ctime();
extern	long	time();
extern	void	exit();
extern	int	getrast();

char *argv0;
struct bithead bh;

int	hflag	= 0;
int	Hflag	= 0;
int	dflag	= 0;
int	xflag	= 0;
int	lflag	= 0;
int	iflag	= 0;
int	pflag	= 0;
int	qflag	= 0;
int	eflag	= 0;
int	cflag	= 0;
int	color	= WHITE;

main(argc, argv)
int	argc;
char	**argv;
{
	FILE	*in, *out;
	int	c;
	struct	passwd	*userentry;
	char	*username, *tspool;
	long	bintim;
	
	argv0=argv[0];

	while ((c = getopt (argc, argv, "T:x")) != EOF)
		switch (c) {
		case 'T':
			switch(optarg[0]) {
			case 'c': /* C.Itoh 8510B */
				cflag++;
				break;
			case 'f': /* Epson FX-80  */
				eflag++;
				break;
			case 'p': /* printronix   */
				pflag++;
				break;
			case 'q': /* qms */
				qflag++;
				break;
			case 'd': /* dots */
				lflag++;
				break;
			case '4': /* 450 */
				dflag++;
				break;
			case 'h': 
				switch(optarg[1]) {
				case 'e': /* hex */
					xflag++;
					break;
				case 'p': /* Hp Thinkjet */
					hflag++;
					break;
				default:
					fprintf(stderr,Printers);
					exit(1);
				}
				break;
			case 'l': /* Hp laserjet */
				Hflag++;
				break;
			case 'i': /* Imagen Impress */
				iflag++;
				break;
			default:
				fprintf(stderr, Printers);
				exit(1);
			}
			break;
		case 'x':
			color = ( color == WHITE ? BLACK : WHITE );
			break;
		default:
			fprintf(stderr, Usage, argv0);
			exit(1);
		}
	
	if ((eflag + qflag + cflag + pflag + iflag + dflag + xflag + Hflag + hflag + lflag) > 1 ) {
		fprintf(stderr, Usage, argv0);
		fprintf(stderr, "only one printer may be specified at a time.\n");
		exit(1);
	}

	if ( ! (eflag | lflag | dflag | cflag | xflag | qflag | pflag | Hflag | hflag) )
		iflag++;

	if (iflag) {
		/* imagen only! */
		if ((username=getlogin()) == NULL)
			if ((userentry=getpwuid(getuid())) == NULL)
				username="guess who!";
			else
				username=userentry->pw_name;
		time(&bintim);
		tspool=ctime(&bintim);
		tspool[strlen(tspool)-1]=0;
	}

	/* default input and output files */
	in =stdin; out=stdout;

	if (optind < argc) {
		/* remaining argument is input file */
		if ((in = fopen(argv[optind], "r")) == NULL) {
			fprintf(stderr, "%s: unable to open %s for input\n",
				argv0, argv[optind]);
			exit(1);
			}
		optind++;
	}
	if (optind < argc) {
		fprintf(stderr, Usage, argv0);
		exit(1);
	}

	if (iflag) {
		/* imagen only */
		fputs("@document(name \"",out);
		fputs(username,out);
		fputs("\", spooldate \"",out);
		fputs(tspool,out);
		fputs("\", language imPRESS, jobheader on, messagedetail on)",out);
	}

	if (glyphpage(in,out) == EOF)
		fprintf(stderr, "%s: unexpected end of file\n", argv0);

	if (iflag)
		putc(imPEOF,out);

	fclose(in);
	fclose(out);
	exit(0);
}

glyphpage(in,out)
FILE *in, *out;
{
	register int i,j,k;
	int nrasters, rastwid, rastbyt, nghoriz, ngvert, krast, iprev;
	char *printronix();

	/* automatically initialized to zeros */
	static	unsigned char	rasters[32][MAXWID];

	if (readhdr(in,&bh)== EOF)
		return(EOF);
	nrasters=bh.nrasters;
	rastwid=bh.rastwid;

	ngvert  =(nrasters+31)/32;
	nghoriz =(rastwid + 1)/ 2;
	rastbyt = rastwid * 2;

	fprintf(stderr,"%s: input: type %d, %d bits wide; %d bits long\n",argv0,bh.type,rastbyt*8, nrasters);

	if ( nrasters <=0 || rastbyt <=0 ) {
		fprintf(stderr, "%s: bad size specification.\n",argv0);
		exit(2);
	}

	if (iflag) {
		/* imagen only */
		putc(SETABSH, out);
		putc(            0, out);
		putc(           32, out);
		putc(SETABSV, out);
		putc(            0, out);
		putc(           32, out);
#ifdef ISCALE
		putc(SETMAGN , out);
		putc(            1, out);
#endif
		putc(BITMAP   , out);
		putc(         0x0F, out);
		putc(      nghoriz, out);
		putc(      ngvert , out);

	} else if (qflag)
		fprintf(out,"^PY^IP0303^IJ00010^IT00010^P%4.4d",rastbyt*8);

	else if (cflag) /* set resolution and lf lengthes.  */
		if (rastbyt*8 <= 640 && rastbyt*8 > 0)
			fprintf(out,"\033N\033T16");
		else if (rastbyt*8 <= 768)
			fprintf(out,"\033E\033T16");
		else 
			fprintf(out,"\033Q\033T16");

	else if (hflag)
		fprintf(out, "\033*t100R\033*r0A");
	else if (Hflag)
		fprintf(out, "\033*r640S");

	for (; nrasters>0; nrasters -= krast) {
		krast=min(nrasters,32);
		for (iprev=31,i=0; i<32; iprev=i++)
			if (i < krast)
				if (getrast(in,rasters[i],rastwid) == EOF)
					return (EOF);
				else
					/* nothing */ ;
			else /* pad */
				for (j=0; j<rastbyt; j++)
					rasters[i][j] = 0;

		if (iflag)
			/* imagen format */
			for (j=0; j<rastbyt; j+=4)
				for (i=0; i<32; i++)
					fwrite(&rasters[i][j],4,1,out);
		else if (eflag || cflag ) {
			register int c,l;
			char	sbits[5];

			/* for each group of 8 rows */
			for (i=0; i<krast; i+=8) {
				if (eflag) /* esc L size1 size2 */
					fprintf( out, "\033L%c%c", 
					    (rastbyt*8) % 256, (rastbyt*8) / 256);
				else { /* cflag * esc S xbits */
					fprintf( out,"\033S%4.4d",rastbyt*8);
				}	
				/* for each byte across */
				for (j=0; j<rastbyt; j++) 
					/* for each bit across the 8 bytes */
					for (k=7; k>=0; k--) {
						/* for each bit down */
						for (c=0, l=0; l<8; l++) 
							if ((rasters[i+l][j]>>k) & 1)
								if (eflag)
								   c |= (1 << (7-l));
								else /* cflag */
								   c |= (1 << l);
						putc(c, out);
					}
				if (eflag)
					fprintf( out, "\015\033J%c", 23);
				else /* cflag */
					fprintf( out, "\r\n");
			}
		} else
			for (i=0; i<krast; i++) {
				if (dflag)
					/* first <lf> then <cr> then grafmode for d450 */
					fputs("\012\015\0333", out);
				if (lflag | xflag)
					/* simple new-line for line pronter and hex */
					putc('\n', out);
				if (qflag) {
					/* One scan line to the QMS */
					qmsraster(&rasters[i][0],rastbyt,out);
					continue;
				}
				if (pflag) {
					/* ^E for printronix graf mode ,
						then SKIP the for loop */
					/* printronix can take a max of
					   132 bytes+nl+grafmode */
					fprintf(out,"\005%.131s\n",
						printronix(&rasters[i][0],rastbyt));
					continue;
				}
				if (Hflag | hflag) {
					fprintf(out, "\033*b%dW", rastbyt);
					fwrite(rasters[i], 1, rastbyt, out);
					continue;
				}

				for (j=0; j<rastbyt; j++)  {
					if (xflag)
						/* print 8 bits in hex */
						fprintf(out, "%2.2X", rasters[i][j]);
					if (dflag | lflag)
						/* print 8 bits for a line printer or diablo*/
						for (k=7; k>=0; k--)
							if ((rasters[i][j]>>k) & 1)
								if (dflag)
									fputs(". ", out);
								else
									putc('.', out);
							else
								putc(' ', out);
				}
			}
	}

	if (iflag)
		putc(ENDPAGE, out);
	else if (qflag)
		fprintf(out, "^G^,^-\n");
	else if (eflag)
		fprintf(out, "\033@");
	else if (cflag)
		fprintf(out, "\033<\033E\033A\033\"");
	else putc('\n', out);

	return(0);
}

char power[8] = { 040, 020, 010, 04, 02, 01, 0 };

#define	PMAX	131
char *printronix(rastline,numrasts)
unsigned char *rastline;
int numrasts;
{

	static char buf[PMAX+1];
	int rastbitp	= 0;
	int bufbitp	= 0;
	int bufptr	= 0;
	int i;

	for (i=0;i<PMAX;i++)
		buf[i] = 0100; /* set the seventh bit */

	for (i = 0; i < numrasts; i++) {
		for (rastbitp=7;rastbitp>=0;rastbitp--) {
			if ((rastline[i] >> rastbitp) & 1)
				buf[bufptr] |= power[5-bufbitp];
			++bufbitp;
			if (bufbitp == 6) {
				bufptr++;
				bufbitp=0;
			if (bufptr >= PMAX)
				break;
			}
		if (bufptr >= PMAX)
			break;
		}
	}
	if (bufbitp)
		bufptr++; /* only bump if we didn't end on a boundary */

	buf[bufptr] = 0;
	return(&buf[0]);
}

qmsraster(rastline,numrastb,outfile)
unsigned char *rastline;
int numrastb;
FILE *outfile;
{

	register int bit, count;
	unsigned char dots;

	/* Initialize the "dots" value before entering the loop */
	count = 0;
	dots = *rastline;

	/* loop through for all "dots" */
	for (; numrastb >= 0; rastline++, numrastb--) 
		if (dots == *rastline)
			count++;
		else  {
			if (count == 1) 
				/* simple data - no repeats */
				fprintf(outfile, "%2.2X", dots);
			else 
				/* repeated pattern */
				fprintf(outfile, "^C%3.3d%2.2X", count, dots);
			count = 1;
			dots = *rastline;
		}
}

