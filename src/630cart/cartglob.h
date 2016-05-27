/*       Copyright (c) 1989 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */

/*
**	Author:	 Ish Niazi
**	Date:	 06/3/88
**
**	Global variables defined in MAIN programs of the cartridge
**	utilities
**
*/

#ifndef MC68MAGIC
#define MC68MAGIC	0520
#endif

int numreloc;	/* number of relocation entries */
int inbytes;	/* size of text and data */

int tflag, bflag, cflag, lflag, sflag, rflag, vflag, mflag, dflag, nflag, iflag;
int width, height;
long zflag;
char *argvs;
char argvbuf[128];
char outfile[16];
char ofile[16];
char Cfile[16];
char menu_name[60];
char shared[25];
char *progname;
char *infile;
int sizeof_text, sizeof_data, sizeof_bss;
FILE *realstdout;
char istring[100];

