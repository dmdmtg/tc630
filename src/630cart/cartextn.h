/*       Copyright (c) 1989 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */

#include <stdio.h>
#include <sys/types.h>
#include <a.out.h>
#include <string.h>
#include <fcntl.h>

/*******************************************************************
**
**	author: Ish Niazi
**
**	date: 6/3/88
**
**	The program defines external reference for the subroutines
**	of cartridge utilities. The globals are defined in MAIN programs.
**
**
*********************************************************************/

#ifndef MC68MAGIC
#define MC68MAGIC	0520
#endif

extern int numreloc;	/* number of relocation entries */
extern int inbytes;	/* size of text and data */

extern int tflag, bflag, cflag, lflag, sflag, rflag, vflag, mflag, dflag, nflag, iflag;
extern long zflag;
extern int width, height;
extern char *argvs;
extern char argvbuf[128];
extern char outfile[16];
extern char ofile[16];
extern char Cfile[16];
extern char menu_name[60];
extern char shared[25];
extern char *progname;
extern char *infile;
extern int sizeof_text, sizeof_data, sizeof_bss;
extern FILE *realstdout;
extern char istring[];

