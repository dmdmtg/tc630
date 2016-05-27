/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


/* static char _filename_sccsid[]="@(#)twid.h	1.1.1.2	(11/11/87)"; */

/* @(#)twid.h	2.1 */
#define	UP	0
#define	DOWN	1
#define	NTXT	20	/* 4 predefined + 16 user-definable */
#define	NBRSH	NTXT

/* ECC HACK */

extern char *cmdlist[], *brushlist[], *txtlist[], *copylist[], *stylelist[], *unixlist[], *codelist[];
extern char *butlist[];
/* Commands */
#define	INK	0
#define	POINT	1
#define	LINE	2
#define	CURVE	3
#define DISC	4
/* Copy commands */
#define COPY	0
#define ROTATE	1
/* Unix commands */
#define	READ	0
#define	WRITE	1
#define	EXIT	2
/* Brushes */
#define	NEW	0
#define	BPOINT	1
#define	BSMALL	2
#define	BMED	3
#define	BBIG	4
/* Texture16s */
#define	TBLACK	1
#define	TGREY	2
#define	TCHECKS	3
#define	TSTIPPLE 4
/* Copy modes */
#define F_MOVE	4
/* Modes */
#define	INKING	0
#define	POINTING 1
#define	LINING	2	/* rubber bands */
#define	CURVING	3
#define DISCING	4

extern Code codes[4][2];
extern Code onecode, twocode;
extern Bitmap *brush, *brushes[NBRSH];
extern Texture16 *cursor[];
Bitmap *Balloc();
Rectangle rotate();
