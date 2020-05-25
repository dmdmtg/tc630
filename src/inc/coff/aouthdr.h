/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* #ident	"@(#)sgs-inc:common/aouthdr.h	1.4" */

#if __STDC__
#include <stdint.h>
#else
typedef long int32_t;
#endif

typedef	struct aouthdr {
	short	magic;		/* see magic.h				*/
	short	vstamp;		/* version stamp			*/
	int32_t	tsize;		/* text size in bytes, padded to FW
				   bdry					*/
	int32_t	dsize;		/* initialized data "  "		*/
	int32_t	bsize;		/* uninitialized data "   "		*/
#if U3B
	int32_t	dum1;
	int32_t	dum2;		/* pad to entry point	*/
#endif
	int32_t	entry;		/* entry pt.				*/
	int32_t	text_start;	/* base of text used for this file	*/
	int32_t	data_start;	/* base of data used for this file	*/
} AOUTHDR;
