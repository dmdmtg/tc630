/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */

/* @(#)load.h	1.1.1.1	(5/2/89) */

/* System dependencies */
#define NSECTS  12		/* maximum number of sections in m68a.out */
#ifndef MC68MAGIC		/* usually defined in fcntl.h */
#define MC68MAGIC 0x150		/* magic number for target machine, here MC68000 */
#endif
#define NOTINIT	1l		/* no physical address for COFF file has this number */

/* Terminal escape sequences */
#define LOADTEMPLATE	"\033[3;0v"	/* download esc.seq */
#define ENC_CHK 	"\033[F"	/* encoding inquiry esc.seq */

/* Load types */
#define BINARY_LOAD	0
#define	HEX_LOAD	1



/* Application functions */
void Psend();
void Precv();
void doswab();		/* swap bytes no matter what */
long readaddr();

/* System functions and variables */
char *getenv(), *strcpy(), *malloc();
extern int errno;

