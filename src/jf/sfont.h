/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */

/* @(#)sfont.h	1.1.1.2 88/02/10 17:14:03 */

/*	sfont.h		*/

typedef struct Sfontchar
{
	unsigned short	offset;		/* ...to character's *base (>0)	*/
	unsigned char	cornerx;	/* ...of character's Bitmap	*/
	unsigned char	cornery;
	unsigned char	deltax;		/* ...to top left		*/
	unsigned char	deltay;
	unsigned char	width;		/* nominal horizontal spacing	*/
	unsigned char	unused;
} Sfontchar;

typedef struct Sfont
{
	short m, n;		/* font contains char's m,m+1,...,n	*/
	unsigned short nwords;	/* size of bits or xbits data area	*/
	unsigned short name;	/* troff name				*/
	short ptsize;		/* point size				*/
	short ptroffx;		/* offset to troff reference point	*/
	short ptroffy;		/* offset to troff reference point	*/
	short height;		/* nominal vertical spacing		*/
	long use;		/* use count				*/
	long spare[8];		/* paranoia				*/
	struct Sfont *prev;	/* preceding font in linked list	*/
	struct Sfont *next;	/* succeding font in linked list	*/
	unsigned short *nonascii; /* table of non-ascii names \(xx	*/
	short *bits;		/* where the small characters are	*/
	short **xbits;		/* where the large characters are	*/
	Sfontchar info[1];	/* n-m+1 character descriptors		*/
} Sfont;

#define NSFHEAD	(sizeof(Sfont)-2*sizeof(Sfont *)-sizeof(unsigned short *) \
		-sizeof(short *)-sizeof(short **)-sizeof(Sfontchar))

/*
 *	charbits.base  =  info[c-m].offset  + bits;
 *	charbits.width = (info[c-m].cornerx + 15) >> 4;
 *	charbits.rect  = { 0, 0, info[c-m].cornerx, info[c-m].cornery };
 *
 *	bitblt(&charbits,charbits.rect,
 *		destbp,add(destp,Pt(info[c-m].deltax,info[c-m].deltay)),f_op);
 *	[ For troff placement, use destp-ptroff; ptroff.y is the ascent. ]
 *	destp.x += info[c-m].width;
 */
