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
#include "dmdps.h"
#include "defs.h"

extern char filename[];
extern char pipewcmnd[];
extern char prname[];

int cflag;

parse_args(argc, argv)
int	argc;
char	*argv[];
{
	int i;
	
#ifndef PAR
#ifndef DMD630
	extern pbits, pspeed;
#endif
#endif

	cflag=0;
	for (i = 1 ; i < argc ;  i++ )
		switch(argv[i][1]) {
#ifdef DMD630
		case 'C':
			cflag=1;
			break;
#endif
		case 'c': strcpy(pipewcmnd,argv[++i]);
		  	break;
		case 'F':
		case 'f': strcpy(filename,argv[++i]);
		  	break;
		case 'T':
		case 'P':
		case 'p': strcpy(prname,argv[++i]);
			break;
#ifndef PAR
#ifndef DMD630
		case '1': pspeed=1200;
			break;
		case '9': pspeed=9600;
			break;
		case '4': pspeed=4800;
			break;
		case '8': pbits=8;
			break;
		case '7': pbits=7;
			break;
#endif
#endif
		}
}
