/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */

/* @(#)dir.h	1.1.1.4	(6/21/90) */


/* static char _dir_sccsid[]="@(#)dir.h	1.1.1.4 (6/21/90)"; */

#ifndef	DIRSIZ
#define	DIRSIZ	14
#endif
struct	dir
{
	ino_t	d_ino;
	char	d_name[DIRSIZ];
};
