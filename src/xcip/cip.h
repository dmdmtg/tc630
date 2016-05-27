/*              C I P   H E A D E R

	Terminals supported:  
	    DMD 5620		"DMD5620" and "DMDTERM" defined
	    DMD 630/730		"DMD630"  and "DMDTERM" defined
	    X-window		"X11" defined

Note: thanks to Dave Kapilow for the X-window emulator package and
      David Wexelblat for doing most of the port to X.
*/



#ifdef DMD5620
#  define DMDTERM
#  define MPXTERM
#  include <jerq.h>
#  include <layer.h>
#  include <jerqio.h>
#  include <font.h>
#  define CHAR_EOF		0xff
#  define stipple
#  define clearRegion(r)	stipple(r)
#endif


#ifdef DMD630
#  define DMDTERM
#  define MPXTERM
#  include <dmd.h>
#  include <layer.h>
#  include <dmdio.h>
#  include <font.h>
#  include <5620.h>
#  include <string.h>
#  define texture16		texture
#  define Texture		Texture16
#  define CHAR_EOF		(-1)
#  define clearRegion(r)	rectf( P->layer, r, F_CLR )
#endif


#ifdef DMDTERM
#  define fontheight(f)		((f)->height)
#  define fontwidth(f,c)	((f)->info[(c)].width)
#  define Cursor		Texture16
#endif


#ifdef X11
#  include <stdio.h>
#  include <jerq.h>
#  define CORRECTSIZE 1
#  define Texture16		Texture
#  define CHAR_EOF		EOF
#  define clearRegion(r)	rectf( &display, r, F_CLR )
#endif


#define C_RESHAPE 8

#ifdef DMDTERM
#  define CORRECTSIZE sameSizeRect( P->layer->rect,\
	inset(Rect(Xmin,Ymin,Xmax,Ymax),-BORDER) )
#endif 

#define BOOL	int
#define TRUE	1
#define FALSE	0

#ifdef DMD630 
#  define max(A, B)  ((A) > (B) ? (A) : (B))
#  define min(A, B)  ((A) < (B) ? (A) : (B))
#endif

#ifndef abs
#  define abs(A)  ((A)<0 ? -(A) : (A))
#endif

#define distance(p,q) 	norm(q.x-p.x , q.y-p.y , 0 )

#define isletter(C) (((C)>='a' && (C)<='z') || ((C)>='A' && (C)<='Z'))
#define isdigit(C) ((C)>='0' && (C)<='9')
#define MOUSE_XY (sub(mouse.xy, Pt(1,1)))

#ifdef X11
# define Xmin	BORDER		/* left edge of frame */
# define Ymin	BORDER		/* top edge of frame */
#else
  extern short	Xmin;		/* left edge of frame. */
  extern short	Ymin;		/* top edge of frame.  */
#endif /* X11 */

extern Point drawOffset;	/* Offset into drawing frame. */
extern Point scrollOffset;	/* Offset into drawing frame from scrolling. */

#define LEFTMOST	12	/* Leftmost origin of window. */
#define TOPMOST		12
#define BOTTOMMOST	25	/* Bottommost origin of window. */
#define BOTTOMY		1024	/* Maximum Y on screen */

#ifdef DMD630
#  define RIGHTMOST	238	/* Rightmost origin of window. */
#  define RIGHTX	1024	/* Maximum X on screen */
#endif

#ifdef DMD5620
#  define RIGHTMOST	12	/* ???? */
#  define RIGHTX	800	/* Maximum X on screen */
#endif

#define BORDER 6
#define BrushSize 60			/* Height of brush frame */
#define ButSize   60			/* Height of button frame */
#define LW     2			/* line width for frame boxes */
#define MW     5			/* Margin width between frames*/
#define DefaultXPicSize 770		/* Default width of Pic frame */
#define DefaultYPicSize 840		/* Default height of Pic frame */

#ifdef X11
# define Xmax   (Drect.corner.x - BORDER)/* right edge of frame */
# define XPicSize (Drect.corner.x - 2 * (LW + BORDER)) /* Width of Pic frame */
# define Ymax   (Drect.corner.y - BORDER)/* left edge of frame */
# define YBR    (Ymin+LW+MW+BrushSize+LW)/* y distance of brush frame */
# define YPIC   (YBR+2*MW)		/* top of drawing frame */
# define Ybut   (Ymax - ButSize - 2 * LW)/* Top of button frame */
# define YBOT   (Ybut - MW)		/* bottom of drawing frame */
# define YPicSize (YBOT - YPIC)		/* Height of Pic frame */
# define YCEN   ((YBR+Ymin)>>1)		/* center line of brush area */
# define Xbut   (((Xmax-Xmin)<<1)/3)
# define XeditD ((Xmax-Xmin)/2)

/* reverse the calculations above to find total width and height */
# define DefaultWidth (DefaultXPicSize + 2 * (LW + BORDER))
# define DefaultHeight (DefaultYPicSize + YPIC + ButSize + (2 * LW) + MW + BORDER)

#else /* X11 */
# define XPicSize  DefaultXPicSize	/* Width of Pic frame */
# define YPicSize  DefaultYPicSize	/* Height of Pic frame */
# define Xmax   (Xmin+XFM)		/* right edge of frame */
# define YBR    (LW+MW+BrushSize+LW)	/* y distance of brush frame */
# define YPIC   (Ymin+YBR+MW)		/* top of drawing frame */
# define YBOT   (YPIC+LW+YPicSize+LW)	/* bottom of drawing frame */
# define Ybut   (YBOT+MW)		/* Top of button frame */
# define Ymax   (Ybut+LW+ButSize+LW)	/* bottom of frame */
# define YCEN   (Ymin+(YBR/2))		/* center line of brush area */
# define Xbut   (((XFM*2)/3)-LEFTMOST+Xmin)
# define XeditD ((XFM/2)-LEFTMOST+Xmin)
#endif /* X11 */

#define XFM    (LW+XPicSize+LW)		/* x distance of frame */
#define Xtext  (Xmin+LW)
#define Ytext  (Ybut+ButSize/6)
#define butHt  min(((ButSize-(LW<<2))/3), ((Xmax-Xbut-(3*LW))/18))

#define INSET	4  /* Number of bits in border of selected window. */

#define MSGXMIN (Xmin+INSET+2)		/* Message area - X-axis minimum. */
#define MSGYMIN (YBOT+LW+6)		/* Message area - Y-axis minimum. */
#define MSGXMAX (XeditD-LW-2)		/* Message area - X-axis maximum. */
#define MSGYMAX (Ymax-INSET-1)		/* Message area - Y-axis maximum. */


#define TNULL (struct thing *)0
#define MNULL (struct macro *)0

#define CIRCLE		0
#define BOX		1
#define ELLIPSE 	2
#define LINE		3
#define ARC		4
#define SPLINE		5
#define TEXT		6
#define MACRO		7

#define NUMBR		7
#define DXBR	(XFM/NUMBR)

#define MAXTEXT		2000
#define MAXNAMESIZE	256

#define PIC		NUMBR
#define ED		PIC+1
#define BRUSH		PIC+1

#define GROWCIRCLE	BRUSH+1
#define MOVE		BRUSH+2
#define GROWEWID	BRUSH+3
#define GROWEHT		BRUSH+4
#define REVSPLINE	BRUSH+5
#define REVLINE		BRUSH+6

#define RADdefault	(XFM/24)
#define WIDdefault	(XFM/8)
#define HTdefault	(XFM/12)

#define nearEDGE 	3

#define SOLID	0
#define DASHED	1
#define DOTTED	2

#define startARROW	1
#define endARROW	2
#define doubleARROW	3

#define ROMAN		1
#define ITALIC		2
#define BOLD		3
#define HELVETICA	4
#define HI		5
#define HB		6
#define PALATINO	7
#define PI		8
#define PB		9
#define EXPANDED	10
#define EI		11
#define EB		12
#define CONSTANTWIDTH	13
#define DEFONT		14

#define LEFTJUST	0
#define CENTER		1
#define RIGHTJUST	2

#define POINTSIZE	10

#define INITtextOutName 100
extern  int	textOutName;

#define GRIDoff 0
#define GRIDon	1

#define WHITEfield 1
#define BLACKfield 0

#define INITbuttons 0
#define MENUbuttons 1
#define DRAWbuttons 2
#define EDITbuttons 3
#define SPLINEbuttons 4
#define BLANKbuttons 5
#define MACRObuttons 6
#define COPYbuttons 7
#define MOVEbuttons 8
#define QUITbuttons 9
#define READbuttons 10
#define EXITbuttons 11
#define GLOBbuttons 12
#define DObuttons   13
#define numButtonStates 14

#define HELPSCALE
#ifdef HELPSCALE
/*
 * The order and value of these defines must be kept in sync
 * with the menu entries in the Scale Menu in menus.c .
 */
#define SCALE_NONE		0
#define SCALE_FACTOR_ONLY	1
#define SCALE_LABEL_ONLY	2
#define SCALE_BOTH		3
#define SCALE_SET_FACTOR	4
#define SCALE_SET_LABEL		5

#define LABEL_NONE	0
#define LABEL_IN	1
#define LABEL_FT_IN	2
#define LABEL_FT	3
#define LABEL_YD	4
#define LABEL_MI	5
#define LABEL_MM	6
#define LABEL_METERS	7
#define LABEL_KM	8

#endif /* HELPSCALE */

#define fontBlk struct FONTBlk
struct FONTBlk {
	short		ps; 	/* Point size */
	short		num;	/* Font style, e.g. ROMAN */
	short		useCount;
	Font *		f;
	fontBlk *	next;
	fontBlk *	last;
};

typedef struct {
	Point start, end;
} pointPair;

typedef struct {
	short ht, wid;
} intPair;

typedef struct {
	short	used; 
	short	size;
	Point *	plist;
} pointStream;

typedef struct {
	short		just;		/* left, center or right */
	short		spacing;	/* 0 = normal spacing */
	char *		s;
	fontBlk *	f;
	short		outName;
} textString;

extern short	maxTextId;


struct macro {
	char *		name;      /* If generated macro: 'm' + id. */
	short		id;        /* If generated macro: its number. */
	short 		useCount;
	Rectangle	bb;
	struct thing *	parts;
	struct macro *	next;
};

struct thing {
	short		type;
	Point		origin;	
	Rectangle 	bb;
	union {
		short		brush;
		short		radius;
		Point		corner;
		Point		end;
		pointPair	arc;
		intPair		ellipse;
		textString	text;
		pointStream	spline;
		struct macro *	list;
	} otherValues;
	short		arrow;
	short		border;
	struct thing *	next; 
	struct thing *	last;
};

extern fontBlk *fonts;

extern Rectangle *Select();
extern Rectangle moveBox();

extern Rectangle 	macroBB();
extern struct macro *	findMacro();
extern int		uniqMacroId();
extern short		newMacroId();
extern BOOL		cmpMacroParts();
extern struct macro *   recordMacro();

#ifndef DMD5620
extern Rectangle canon();
#endif

extern struct thing *	newCircle();
extern struct thing *	newBox();
extern struct thing *	newEllipse();
extern struct thing *	newLine();
extern struct thing *	newArc();
extern struct thing *	newText();
extern struct thing *	newSpline();
extern struct thing *	newMacro();
extern BOOL		cmpThings();
extern struct thing *	selectThing();
extern struct thing *	copyThing();
extern struct thing *	deleteThing();
extern struct thing *	deleteAllThings();
extern struct thing *	insert();
extern struct thing *	dummy_insert();
extern struct thing *	Remove();
extern struct thing *	doMouseButtons();
extern struct thing *	place();
extern struct thing *	displayCommandMenu();
extern struct thing *	displayThingMenu();
extern struct thing *	doGet();
extern struct thing *	doClear();
extern struct thing *	defineMacro();
extern struct thing *	makeRelative();
extern struct thing *	reflect();

extern Point track();
extern Point track2();
extern Point trackMacro();
extern Point computeArcOrigin();
extern Point jchar();

extern void	initMessage();
extern void	moreMessage();
extern void	newMessage();

extern Point	align();
extern Point	alignUp();
extern Point	alignDown();
extern int	alignInt();

extern char *getString();
extern char *getSpace();

extern FILE *popen();

extern fontBlk *findFont();

extern xtipple ();

extern char * 	addBackslashes();

#define FINEALIGN
#ifdef FINEALIGN
extern short	currGridsize;
#endif
#ifdef HELPSCALE
extern short	currScaleType;
extern short	currScaleFactor;
extern short	currLabelType;
#endif
extern short	currAlignment;
extern short	currPS;
extern short	currFont;
extern short	currJust;
extern short	currLineType;
extern short	currBoxType;
extern short	currLineArrows;
extern short	currSplineArrows;
extern short	currSpacing;

extern struct thing * firstThing;

#ifdef X11
# define CW		ave_fontwid(&defont)
# define NS		fontheight(&defont)
  extern void		initGraphics();
  extern Rectangle	clip_rect;
#endif


extern Cursor	crossHairs;
extern Cursor	hourglass;
extern Cursor	rusure;
extern Cursor	textCursor;

extern Texture	grid;
extern Texture	grid8;
#ifdef FINEALIGN
  extern Texture	fineGrid;
#endif

#ifdef X11
    extern Texture	copyright;
    extern Texture	lowerTriangle;
    extern Texture	upperTriangle;

    extern Bitmap	markerbm;
    extern Bitmap	markermaskbm;
    extern Bitmap	bgsave;
#else
    extern Texture16	copyright;
    extern Texture16	lowerTriangle;
    extern Texture16	upperTriangle;
#endif
