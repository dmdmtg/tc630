/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


/* static char _filename_sccsid[]="@(#)tdmd.h	1.1.1.2	(11/19/87)"; */

/* defines */
#define	OPEN	033
#define	ARC	00
			/* CONT is not defined, as it is the default */
#define	MOVE	01
#define	ERASE	02
#define	CLOSE	03
#define	ON	04
#define	OFF	05
#define	LINEMOD	06
#define DELAY	07
#define FILL	010
#define CONT	011
#define EXIT	012

#define	PACKET	100
#define	HPACKET	150
#define	READY	'Z'
