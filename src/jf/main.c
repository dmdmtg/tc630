/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)main.c	1.1.1.7	(3/31/88)";

#define MAIN

/* includes */
#include "jf.h"
#ifdef DMD630
#	include <object.h>
#	include <menu.h>
#else
#	include "tmenuhit.h"
#endif /* DMD630 */

/* defines */
#define Options "COc"
#define True  1
#define False 0
#define SizeInBufLen 11

/* types */
typedef struct shortitem {
    char *text;
    struct {
	unsigned short uval;
	unsigned short grey;
    } ufield;
    struct Tmenu *next;
} shortitem;

/* globals */
Point DrectOrigin;
short oldFontFormat = False;

/* externals */
extern int optind;
extern char *optarg;
extern int optptr;

/* locals */
#ifdef DMD630
	static short cacheText = False;
#endif /* DMD630 */
static int tmenuflags = 0;

/*
 * menu definitions
 */


/* read font menu */
#define MENU_READBLITFONT 10
#define MENU_READCANONFONT 11
static shortitem read_menu[] = {
	"blit font", MENU_READBLITFONT, 0, 0,
	"canon font", MENU_READCANONFONT, 1, 0,
	NULL
};
static Tmenu readmenu = {
    (Titem *)read_menu, 0, 0, 0, TM_TEXT | TM_UFIELD | TM_NEXT
};


/* write font menu */
#define MENU_WRITEBLITFONT 20
#define MENU_WRITECANONFONT 21
#define MENU_WRITEICON 22
static shortitem write_menu[]={
	"blit font", MENU_WRITEBLITFONT, 0, 0,
	"canon font", MENU_WRITECANONFONT, 1, 0,
	"icon", MENU_WRITEICON, 0, 0,
	NULL
};
static Tmenu writemenu = {
    (Titem *)write_menu, 0, 0, 0, TM_TEXT | TM_UFIELD | TM_NEXT
};


/*
 * char manipulation menu
 *
 * The following two menus are used to reflect the different nature
 * of character size considerations, and font size considerations.
 * However, all values of both menus - except for MENU_HOLLOW - are
 * handled by the same routine.  Therefore, the constant values must
 * be ascending.  For this reason, the range 30-49 has been reserved.
 */
#define MENU_CHARWIDTH 30
#define MENU_CHARLEFT 31
#define MENU_CHARTOP 32
#define MENU_CHARBOTTOM 33
static shortitem char_menu[]={
	"char width", MENU_CHARWIDTH, 0, 0,
	"char left", MENU_CHARLEFT, 0, 0,
	"char top", MENU_CHARTOP, 0, 0,
	"char bottom", MENU_CHARBOTTOM, 0, 0,
	NULL
};
static Tmenu charmenu = {
    (Titem *)char_menu, 0, 0, 0, TM_TEXT | TM_UFIELD | TM_NEXT
};


/* font manipulation menu */
#define MENU_MAXWIDTH 34
#define MENU_ALLWIDTHS 35
#define MENU_NCHARS 36
#define MENU_HEIGHT 37
#define MENU_ASCENT 38
#define MENU_HOLLOW 49
static shortitem font_menu[]={
	"max width", MENU_MAXWIDTH, 0, 0,
	"all widths", MENU_ALLWIDTHS, 0, 0,
	"nchars", MENU_NCHARS, 0, 0,
	"height", MENU_HEIGHT, 0, 0,
	"ascent", MENU_ASCENT, 0, 0,
	"hollow font", MENU_HOLLOW, 0, 0,
	NULL
};
static Tmenu fontmenu = {
    (Titem *)font_menu, 0, 0, 0, TM_TEXT | TM_UFIELD | TM_NEXT
};


/* bit manipulation menu */
#define MENU_MOVEHOR 50	/* N.B. menu order follows Texture *arrows */
#define MENU_MOVEVER 51
#define MENU_FLIP 52
#define MENU_TRANSPOSE 53

#define MENU_STORE 54   /* N.B. menu order follows op codes */
#define MENU_OR 55
#define MENU_CLR 56
#define MENU_XOR 57
static shortitem bit_menu[]={
	"move hor", MENU_MOVEHOR, 0, 0,
	"move ver", MENU_MOVEVER, 0, 0,
	"flip", MENU_FLIP, 0, 0,
	"transpose", MENU_TRANSPOSE, 0, 0,
	"copy", MENU_STORE, 0, 0,
	"or", MENU_OR, 0, 0,
	"clear", MENU_CLR, 0, 0,
	"xor", MENU_XOR, 0, 0,
	NULL
};
static Tmenu bitmenu = {
    (Titem *)bit_menu, 0, 0, 0, TM_TEXT | TM_UFIELD | TM_NEXT
};


/* top menu */
#define MENU_CHAR 101
#define MENU_FONT 102
#define MENU_BIT 103

#define MENU_MOVE 104
#define MENU_CLOSE 105

#define MENU_READ 106
#define MENU_WRITE 107

#define MENU_NEW 108
#define MENU_DELETE 109

#define MENU_EXIT 110
static shortitem top_menu [] = {
	"char", MENU_CHAR, 0, &charmenu,
	"font", MENU_FONT, 0, &fontmenu,
	"bit", MENU_BIT, 0, &bitmenu,
	"move", MENU_MOVE, 0, 0,
	"close", MENU_CLOSE, 0, 0,
	"read", MENU_READ, 0, &readmenu,
	"write", MENU_WRITE, 0, &writemenu,
	"new font", MENU_NEW, 0, 0,
	"delete font", MENU_DELETE, 0, 0,
	"exit", MENU_EXIT, 0, 0,
	NULL
};
static Tmenu topmenu = {
    (Titem *)top_menu, 0, 0, 0, TM_TEXT | TM_UFIELD | TM_NEXT
};

/* procedures */
Disp *FindEditInPool ();


Reset()
{
	Point shift; Disp *Scan;

	/* shift is the distance between the location and the new */
	shift = sub(Drect.origin,DrectOrigin);

	/* the new location is now current */
	DrectOrigin = Drect.origin;

	Resize();

	/*
	 * There is apparently a linked list of objects to be displayed.
	 * The following loop goes through the list and changes each
	 * objects understanding of where it exits, and marks it to be
	 * redrawn.
	 */
	Scan = Pool;
	while (Scan != DNULL) {
		if (Scan->Class == FontClass)
			Scan->Disp.TopFont->r = raddp(Scan->Disp.TopFont->r,shift);
		else	Scan->Disp.TopEdit->r = raddp(Scan->Disp.TopEdit->r,shift);
		Scan->Redraw = true;
		Scan = Scan->Next;
	}

	/*
	 * redraw traverses the list, and redraws the objects that
	 * need to be redrawn.
	 */
	Redraw();
}

main(argc, argv)
	int argc;
	char *argv[];
{
	int option;
	int openFileFlag=0;

	/*
	 * the standard getopt() relies on static initialized storage.
	 * Since getopt() is called before cache() (to handle the -c
	 * flag), the cache static initializer snapshot will be wrong.
	 * Therefore, always initialize these globals, instead.
	 */
	optind = 1;
	optptr = 1;

	while ((option = getopt (argc, argv, Options)) != EOF)
	    switch (option) {
	    case 'C':
		/* allow canon mode */
		read_menu[1].ufield.grey = 0;
		write_menu[1].ufield.grey = 0;
		break;
	    case 'O':
		oldFontFormat = True;
		break;
	    case 'c':
#		ifdef DMD630
		    cacheText = True;
#		endif /* DMD630 */
		break;
	    default:
		fprintf (stderr,
		    "%s: illegal option -%c ignored\n", argv[0], option);
		break;
	    }

	cursswitch(&deadmouse);
	request(MOUSE|KBD);
	DrectOrigin = Drect.origin;
	Resize();

#	ifdef DMD630
	    if (cacheText)
		cache (NULL, A_NO_SHOW);
#	endif /* DMD630 */

	while (optind < argc)
	    openFileFlag |= readfile(argv[optind++],0);

	if (openFileFlag) cursswitch(TNULL); else cursswitch(&menu3);

	for (;wait(MOUSE);sleep(2)) {

		if (reshaped()) Reset();
		if (!ptinrect(mouse.xy,Drect)) continue;

		if (button12()) HandleSelection(openFileFlag);
		else if (button3()) HandleTopMenu();
	}
}

HandleSelection(b1on)
int b1on;		/* tacky.  This is immediately set */
{
	b1on=button1()!=0;
	mousetrack();
	if (mtk.disp != DNULL) {
		if (mtk.disp->Class==FontClass) {
			if (b1on)
			    charop (mtk.disp->Disp.TopFont, mtk.c);
			else
			    charcl (mtk.disp->Disp.TopFont, mtk.c);
		}
		else edispset(mtk.disp->Disp.TopEdit, mtk.pxl, b1on);
	}
}

HandleTopMenu()
{
	int hit, b=0;

	cursswitch(TNULL);
	hit = tmenuhit (&topmenu, 3, tmenuflags)->ufield.uval;
	tmenuflags |= TM_EXPAND;
	switch (hit) {
		case MENU_MOVE:
			moveobject(); break;
		case MENU_NEW:
			newfont(); break;
		case MENU_READBLITFONT:
			readfile (CNULL, 0);
			break;
		case MENU_READCANONFONT:
			readfile (CNULL, 1);
			break;
		case MENU_WRITEBLITFONT:
			writefile (0);
			break;
		case MENU_WRITECANONFONT:
			writefile (1);
			break;
		case MENU_WRITEICON:
			writeicon ();
			break;
		case MENU_CLOSE:
			cursswitch(&target);
			if (buttons(DOWN) == 3)
			    mousetrack();
			else {
			    buttons(UP);
			    cursswitch(TNULL);
			    break;
			}
			buttons(UP);
			cursswitch(TNULL);
			if(mtk.disp == DNULL)
			    break;
			if (mtk.disp->Class == FontClass)
			    sqdfont(0);
			else
			    charcl (mtk.disp->Disp.TopEdit->fdp,
				mtk.disp->Disp.TopEdit->c);
			break;
		case MENU_DELETE:
			cursswitch(&target);
			if (buttons(DOWN) == 3) mousetrack();
			else {buttons(UP); cursswitch(TNULL); break;}
			buttons(UP);
			cursswitch(TNULL);
			sqdfont(1);
			break;
		case MENU_CHARWIDTH:
		case MENU_CHARLEFT:
		case MENU_CHARTOP:
		case MENU_CHARBOTTOM:
		case MENU_MAXWIDTH:
		case MENU_ALLWIDTHS:
		case MENU_NCHARS:
		case MENU_HEIGHT:
		case MENU_ASCENT:
			sizefunc(hit);
			break;
		case MENU_MOVEHOR:
		case MENU_MOVEVER:
		case MENU_FLIP:
		case MENU_TRANSPOSE:
			shiftfunc(hit - MENU_MOVEHOR);
			break;
		case MENU_STORE:
		case MENU_OR:
		case MENU_CLR:
		case MENU_XOR:
			bitfunc(hit - MENU_STORE);
			break;
		case MENU_HOLLOW:
			cursswitch(&target);
			if (buttons(DOWN) == 3) mousetrack();
			else {buttons(UP); cursswitch(TNULL); break;}
			buttons(UP);
			cursswitch(TNULL);
			hollow ();
			break;
		case MENU_EXIT:
			if (lexit3())
			    exit();
			break;
	}
	cursswitch(TNULL);
}


sizefunc(scode)
	int scode;
{
	char *itoa();
	Font *fontrange();
	Disp *FindFontInPool();
	Rectangle DispRect();

	int ikbd;
	char str[SizeInBufLen];
	Point p;
	Fontdisp *fdp=FDNULL;
	Editdisp *edp;
	Font *fp, *newfp;
	Fontchar *ich;
	Disp *disp;
	shortitem *relevantMenu = char_menu;
	/*
	 * when accessing the relevant menu, convert hit code
	 * to item index.
	 */
	int menuAdjust = MENU_CHARWIDTH;
	int newvalue;
	Point catp;


	if (scode < 0)
	    return 0;
	else if (scode == MENU_CHARWIDTH)
	    return chwidth();

	cursswitch(&target);
	if (buttons(DOWN) == 3)
	    mousetrack(); 
	else {
	    buttons(UP);
	    cursswitch(TNULL);
	    return 0;
	}
	buttons(UP);
	cursswitch(TNULL);
	if (mtk.disp == DNULL)
	    return 0;
	if (scode == MENU_CHARLEFT) {
		if (mtk.disp->Class != EditClass)
		    return 0;
		edp = mtk.disp->Disp.TopEdit;
		fdp = edp->fdp;
		fp=fdp->fp;
		ikbd = SIGNED((fp->info)[edp->c].left);
	} else if (scode == MENU_CHARTOP) {
		if (mtk.disp->Class != EditClass)
		    return 0;
		edp = mtk.disp->Disp.TopEdit;
		fdp = edp->fdp;
		fp=fdp->fp;
		ikbd = SIGNED((fp->info)[edp->c].top);
	} else if (scode == MENU_CHARBOTTOM) {
		if (mtk.disp->Class != EditClass)
		    return 0;
		edp = mtk.disp->Disp.TopEdit;
		fdp = edp->fdp;
		fp=fdp->fp;
		ikbd = SIGNED((fp->info)[edp->c].bottom);
	} else {
		if (mtk.disp->Class != FontClass)
		    return 0;
		fdp = mtk.disp->Disp.TopFont;
		fp=fdp->fp;
		relevantMenu = font_menu;
		menuAdjust = MENU_MAXWIDTH;
		switch (scode) {
		case MENU_MAXWIDTH:
		    ikbd = fdp->mwidth;
		    break;
		case MENU_ALLWIDTHS:
		    ikbd = fp->info[fp->n].width;
		    break;
		case MENU_NCHARS:
		    ikbd = fp->n;
		    break;
		case MENU_HEIGHT:
		    ikbd = fp->height;
		    break;
		case MENU_ASCENT:
		    ikbd = fp->ascent;
		    break;
		}
	}

	disp = FindFontInPool(fdp,Pool);

	cursswitch (&deadmouse);
	drstore (rkbd);
	p=drstring (" (",
		drstring(relevantMenu[scode-menuAdjust].text, pkbd));
	p=drstring ("): ",
		drstring(itoa(ikbd), p));
	if (kbdstring(str, SizeInBufLen, p) <= 0) {
	    drclr(rkbd);
	    return 0;
	}
	ikbd = atoi(str);
	drclr(rkbd);

	newvalue = ikbd;
	switch (scode) {
	case MENU_CHARLEFT:
		newvalue = (fp->info)[edp->c].left = max(-127,min(127,ikbd));
		catp = drstring(relevantMenu[scode-menuAdjust].text, pkbd);
		break;
	case MENU_CHARTOP:
		newvalue = (fp->info)[edp->c].top = max(-127,min(127,ikbd));
		catp = drstring(relevantMenu[scode-menuAdjust].text, pkbd);
		break;
	case MENU_CHARBOTTOM:
		newvalue = (fp->info)[edp->c].bottom = max(-127,min(127,ikbd));
		catp = drstring(relevantMenu[scode-menuAdjust].text, pkbd);
		break;
	case MENU_MAXWIDTH:
		newvalue = fp->info[fp->n].width = max(0,min(255,ikbd));
		catp = drstring(relevantMenu[scode-menuAdjust].text, pkbd);
		break;
	case MENU_ALLWIDTHS:
		for (ich = fp->info; ich < fp->info+fp->n; ich++)
			ich->width = ikbd;
		catp = drstring(relevantMenu[scode-menuAdjust].text, pkbd);
		break;
	case MENU_NCHARS:
		for (edp = fdp->edp; edp != EDNULL; edp = edp->edp) {
			Pool = RemoveFromPool(FindEditInPool(edp,Pool),Pool);
			edp->fdp = FDNULL;
		}
		fdp->edp = EDNULL;
		if ((newfp = fontrange(fp,ikbd)) == FNULL)
			return 0;
		fdp->fp = newfp;
		catp = drstring(relevantMenu[scode-menuAdjust].text, pkbd);
		break;
	case MENU_HEIGHT:
		if (!fontheight(fp,ikbd))
		    return 0;
		catp = drstring(relevantMenu[scode-menuAdjust].text, pkbd);
		break;
	case MENU_ASCENT:
		newvalue = fp->ascent = max(0,min(fp->height,ikbd));
		catp = drstring(relevantMenu[scode-menuAdjust].text, pkbd);
		break;
	}
	drstring (itoa (newvalue), drstring (" is ", catp));

	EraseFont(disp);
	Resize();
	Redraw();
	return 1;
}

chwidth()
{
	Fontdisp *fdp; Font *fp; int b, c, w; Fontchar *ich;
	Editdisp *edp;

	cursswitch(&widthmark);
	for (;;) {
		b=buttons(DOWN); buttons(UP);
		if (b == 3) break;
		mousetrack();
		if (mtk.disp == DNULL) continue;
		if (mtk.disp->Class!=EditClass) continue;
		edp = mtk.disp->Disp.TopEdit;
		c=edp->c; fdp=edp->fdp; fp=fdp->fp;
		ich=fp->info+c;
		if (b == 1) {
			w = 0;
		} else {
			w = mtk.pxl.x + 1;
			if (ich->left & 0x80)
				w += ich->left - 0x100;
		}
		if (w < 0 || ich->width == w) continue;
		ich->width=w;
		editdisp(edp);
	}
	return 1;
}

