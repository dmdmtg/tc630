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
#define NULL	0
#ifndef min
#define min(a, b) (a <= b ? a : b)
#endif
#define NULL	0
#define BNULL	(Bitmap *)NULL

#define VALSCREENCOLOR 0

#define UP	0
#define DOWN	1
#define ACROSS	2

typedef int	Word;		/* words are now 32 bits	*/

typedef struct Point {
	int	x;
	int	y;
} Point;

typedef struct Rectangle {
	Point origin;
	Point corner;
} Rectangle;

typedef struct Bitmap {
	Word	*base;		/* pointer to start of data */
	unsigned width;		/* width in 32 bit words of total data area */
	Rectangle rect;		/* rectangle in data area, local coords */
} Bitmap;

typedef struct Texture {
	Word bits[32];
} Texture;

extern Bitmap *balloc();
extern Bitmap *readbitmap();

extern Rectangle Rect();
extern Point Pt();
extern Word *addr();

typedef int	Code;
#define	F_STORE	((Code) 0)	/* target = source */
#define	F_OR	((Code) 1)	/* target |= source */
#define	F_CLR	((Code) 2)	/* target &= ~source */
#define	F_XOR	((Code) 3)	/* target ^= source */

#define	WORDSHIFT	5
#define	WORDSIZE	32
#define	WORDMASK	(WORDSIZE-1)
#define	ONES		0xFFFFFFFF
#define	FIRSTBIT	((unsigned)0x80000000)
#define	LASTBIT		((unsigned)0x1)
#define	XMAX		800
#define	YMAX		1024

