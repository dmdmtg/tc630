/*	Copyright (c) 1987 AT&T	*/
/*	All Rights Reserved	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* static char _host_sccsid[]="@(#)host.h	1.1.1.6 (6/21/90)";

/* defines */
#define LoadComplete	0
#define AlreadyLoaded	1
#define LoadFailed	2
#define NoDMD		3
#define NoDefont	5

/* types */
typedef struct {
	int x,y;
} Point;

typedef struct {
	Point origin,corner;
} Rectangle;

typedef struct {
	int height;
	int ascent;
	int *width;
} Font;	/* on the host, fonts are just widths */
