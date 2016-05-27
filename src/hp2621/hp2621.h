/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


/* static char _filename_sccsid[]="@(#)hp2621.h	1.1.1.2	(11/4/87)"; */


#define	MPXTERM
#include <dmd.h>
#include <setup.h>
#include <font.h>
#ifdef DMD630
#include <5620.h>
#else
#undef        bttn123
#undef        bttn13
/* It must be used inorder for the mouse in any new windows */
/* to function  correctly. */
#define       bttn123()       (((struct udata *)P->data)->mouse.buttons & 07)
#define       bttn13()        (((struct udata *)P->data)->mouse.buttons & 05)
#endif


typedef short BOOLEAN;
#define TRUE -1		/* Must be -1 or toggling won't work */
#define FALSE 0

/* Font properties */
#define	CW	9	/* width of a character */
#ifdef NS
#undef NS
#endif
#define	NS	16	/* newline size; height of a character */
#define	CURSOR	"\1"	/* By convention */

#undef PFKEYSIZE
#define PFKEYSIZE	50
#define OUTOFSPACE "hp2621: no memory in terminal"
#define ERRORWT 300	/* Error wait time */
#ifdef HOMEDOWN
#undef HOMEDOWN
#endif
#define HOMEDOWN "0;1H"
#define ESC	'\033'
#define STARTUP 100	/* Size of startup buffer */
#define BORDER	8	/* Size of screen border */
#define	XMARGIN	3	/* inset from border of layer */
#define	YMARGIN	3
#define	INSET	4
#define	BUFS	32
#define	HISTSIZ	4096	/* number of history characters */
#define EBUFSIZE PFKEYSIZE+32	/* Size of echo buffer */
#define BLINK	25	/* cursor blink rate (given in tics) */

#define	C_RESHAPE 8

#define F_MODE up->rvid ? F_STORE : F_CLR

typedef struct M {		/* Menu for button 2 */
  char *menutext[10];
} M;

extern M m;

struct User {
  short		x, y;		/* character positions */
  char		*backp;		/* Points to "backc" pages back from */
				/* last line displayed in layer */
				/* (last line placed in hist). */
				/* The getnextchar routine effectively*/
				/* redraws the screen when backp is */
				/* not zero. */
  short		backc;		/* Used back "back" and "forward". */
				/* Indicates which page (back from */
				/* the last line) that is currently */
				/* being displayed.  A page is 3/4th */
				/* the number of lines in a layer. */
  BOOLEAN	atend;		/* This flag is used by the menu */
				/* option "backup" to indicate the */
				/* start of "hist" has been */
				/* encountered and that the layer */
				/* can't be backup'ed any further. */
  short		nbacklines;	/* Indicates to getnextchar routine */
				/* how many lines are to be redrawn */
				/* in the layer.  It always starts */
				/* as 2 less than length of layer. */
  short		x_max;		/* Maximum # chars/line */
  short		y_max;		/* Maximum # lines/layers */
  BOOLEAN	blocked;	/* Flag for CTRL S or SCROLL keys */
				/* Also used by pagemode for blocking */
				/* input when the layer is full. */
  BOOLEAN	pagemode;	/* Flag indicating pagemode being */
				/* used. */
  int		peekc;		/* When a default character is found */
				/* in realmain,  characters are input */
				/* until one < " " or > '\177' is */
				/* found or more than 32 characters */
				/* are input.  The last character is */
				/* stored in peekc for retrieval by */
				/* the getnextchar routine. */
  BOOLEAN	orig;
  Menu		menu;
  struct	M m;
  BOOLEAN	rvid;		/* Flag indicating reverse video set. */
  BOOLEAN	cursor;		/* Indicates if cursor is on or off */
  int		blink;		/* Indicates if cursor is blinking */
  BOOLEAN	clear;		/* Indicates layer is to be cleared */
  char		*histp;		/* Location in hist where next input */
				/* character is to be stored. */
  char		hist[HISTSIZ];  /* Storage for input characters not */
				/* necessarily on the screen. */
				/* This is a circular list.  A NULL */
				/* and histp always point to the end */
				/* of the list. */
};

extern char startup[];		/* String to send with new hp windows */
extern char *firsttime;		/* String to send with first window */
extern int s_size;		/* Size of startup string */
extern int f_size;		/* Size of firsttime string */

void handlekbd ();
void backup ();
void clear ();
void realmain ();
void newhp ();
void newline ();
void curse ();
int getnextchar ();
int waitchar ();
void readmenu ();
Point pt ();
void scroll ();
short number ();
