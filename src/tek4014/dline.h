/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


/* static char _filename_sccsid[]="@(#)dline.h	1.1.1.1	(11/11/87)"; */

/*
 * Definitions for globals involved with lines
 */
#ifdef LINE_C

short Jdmajor;		/* Delta for major direction */
short Jdminor;		/* Delta for minor direction */
short Jxmajor;	/* flag: is x the major direction? */
short Jslopeneg;	/* flag: is slope of line negative? */
Point Jdsetline();
Word DotMask;

#else

extern short Jdmajor;		/* Delta for major direction */
extern short Jdminor;		/* Delta for minor direction */
extern short Jxmajor;	/* flag: is x the major direction? */
extern short Jslopeneg;	/* flag: is slope of line negative? */
extern Point Jdsetline();
extern Word DotMask;

#endif
