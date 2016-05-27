/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */

/* @(#)jf.h	1.1.1.4 88/02/10 17:13:58 */

/*	jf.h	*/

/* includes */
#include <dmd.h>
#include <dmdio.h>
#include <font.h>
#include "sfont.h"
#ifdef DMD630
#	include <5620.h>
#endif /* DMD630 */

/* defines */
#undef Texture
#undef Texture16
#define Texture Texture16
#define bool int
#define true 1
#define false 0

#define UP	0	/* button state */
#define DOWN	1	/* button state */

#define MFDISP	16	/* maximum number of open fonts */
#define MEDISP	32	/* maximum number of open characters */

#define WBORD	2	/* size of border around character (fontdisp) */
#define WMARG	1	/* size of margin inside border (fontdisp) */

#define EDSIZE	8	/* size of edit pixel */

#define CNULL	(char *) 0
#define TNULL	(Texture *) 0
#define BNULL	(Bitmap *) 0
#define FNULL	(Font *) 0
#define FDNULL	(Fontdisp *) 0
#define EDNULL	(Editdisp *) 0
#define DNULL	(Disp *) 0

#define drstore(r)	rectf(&display,r,F_STORE)
#define drflip(r)	rectf(&display,r,F_XOR)
#define drclr(r)	rectf(&display,r,F_CLR)
#define drstring(s,p)	string(&defont,s,&display,p,F_XOR)

#define SIGNED(ch)	((ch) & 0x80 ? (ch) | 0xffffff00 : (ch))

/* types */
typedef struct Fontdisp Fontdisp;
typedef struct Editdisp Editdisp;

struct Fontdisp {
	Rectangle r;	/* rectangle containing display */
	Font *fp;	/* pointer to displayed font */
	Editdisp *edp;	/* pointer to first edit field (if any) */
	int mwidth;	/* maximum character width */
	int cbx, cby;	/* size of small box around each character */
	int ncpl;	/* number of characters per display line */
	char *filnam;	/* source and/or destination file */
};

struct Editdisp {
	Rectangle r;	/* rectangle containing display */
	Fontdisp *fdp;	/* pointer to originating font */
	Editdisp *edp;	/* pointer to next edit field (if any) */
	int size;	/* expansion factor */
	int c;		/* character code */
	Bitmap *mask;	/* character's mask */
};

# define DispClass int
# define FontClass 0
# define EditClass 1

typedef union FontOrEdit {
	struct Fontdisp *TopFont; 
	struct Editdisp *TopEdit;
} FontOrEdit;

typedef struct Disp {
	DispClass Class;
	union FontOrEdit Disp;
	bool Redraw;
	struct Disp *Next;
} Disp;

typedef struct Mousetrack {
	Disp *disp;	/* display object under mouse */
	int c;		/* character on which mouse is sitting */
	Point pxl;	/* mouse's pixel */
} Mousetrack;

typedef struct Temp
{
	unsigned short name;
	short ptsize;
	short ptroffx;
	unsigned short *nonascii;
}Temp;


extern Texture menu3, deadmouse, target, skull, movearound;
extern Texture *arrows[], widthmark;

#ifndef MAIN
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN Disp *Pool;
EXTERN Disp *NewFontPool();
EXTERN Disp *NewEditPool();
EXTERN Disp *RemoveFromPool();
EXTERN Disp *MoveToTopOfPool();

EXTERN Fontdisp fdisp[MFDISP];
EXTERN Editdisp edisp[MEDISP];
EXTERN int nedisp;

EXTERN struct Mousetrack mtk;

EXTERN Rectangle rkbd;
EXTERN Point pkbd;
EXTERN FILE *filep;
Temp *pter;
