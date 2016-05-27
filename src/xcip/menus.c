#include "cip.h"

#ifdef X11
# include "menu.h"

#else /* X11 */

# ifdef DMD630
#  include <menu.h>
# endif

# ifdef DMD5620
#  include "tmenuhit.h"
#  include <pandora.h>
# endif

#endif /* X11 */


extern int		currentBrush;
extern int		copyFlag;
extern int		thingSelected;
extern int		editDepth;
extern int		gridState;
extern int		videoState;
extern int		buttonState;
extern short		fontBells;
extern short		noSpace;

extern struct macro *	macroList;

extern Rectangle	brushes[];


extern struct thing *	Transform();
extern struct thing *	undo();
extern void		undo_clear();
extern Point 		jString();
extern void		getKbdChar();
extern void		printSpace();
extern void		printPWD();
extern void		printChgStatus();


#ifdef DMDTERM

/**** TITEM STRUCTURES ****/

/* Basic Titem structure with only text field. */
typedef struct BasicItem {
	char *		text;	/* string for menu */
} BasicItem;

/* Checkmark Titem structure with text and icon fields. */
typedef struct ChkmarkItem {
	char *		text;	/* string for menu */
	Bitmap *	icon;	/* pointer to the icons bitmap */
} ChkmarkItem;

/* Top menu Titem structure with text and next fields. */
typedef struct TopMenuItem {
	char *		text;	/* string for menu */
	Tmenu *		next;	/* pointer to sub-menu */
} TopMenuItem;

#ifdef HELPSCALE
/* Mid-level menu Titem structure with text, icon, and sub-menu fields. */
typedef struct MidMenuChkmarkItem {
	char *		text;	/* string for menu */
	Tmenu *		next;	/* pointer to sub-menu */
	Bitmap *	icon;	/* pointer to the icons bitmap */
} MidMenuChkmarkItem;
#endif /* HELPSCALE */

#endif /* DMDTERM */

/**** MENU RELATED FUNCTIONS ****/

/* Set a checkmark in specified menu at specified index and clearing
   checkmarks off all other items in menu. */

#ifdef X11

	void
setChkmark( tmenu, index )
	NMenu *		tmenu;
	short		index;
{
	register NMitem *	ci;
	register int		i;

	ci = tmenu->item;

	for( i=0;  ci->text != NULL;  ci++, i++ ) {
		ci->selected = (i==index) ? 1 : 0 ;
	}
}

#else /* X11 */

	void
setChkmark( tmenu, index )
	Tmenu *		tmenu;
	short		index;
{
	register ChkmarkItem *	ci;
	register int		i;

	ci = (ChkmarkItem *) tmenu->item;

	for( i=0;  ci->text != NULL;  ci++, i++ ) {
		ci->icon = (i==index) ? &B_checkmark : 0 ;
	}
}

#ifdef HELPSCALE
	void
setChkmarkMidmenu( tmenu, index )
	Tmenu *		tmenu;
	short		index;
{
	register MidMenuChkmarkItem *	ci;
	register int		i;

	ci = (MidMenuChkmarkItem *) tmenu->item;

	for( i=0;  ci->text != NULL;  ci++, i++ ) {
		ci->icon = (i==index) ? &B_checkmark : 0 ;
	}
}
#endif /* HELPSCALE */

#endif /* X11 */


/* Returns index in the menu of the selected item (starting at 0). */
	int
indexHit( tmenu) 
#ifdef X11
	NMenu *		tmenu;
#else /* X11 */
	Tmenu *		tmenu;
#endif /* X11 */
{
	return tmenu->prevhit + tmenu->prevtop ;
}



/**** Point size menu ****/

int  pointSizes[] = { 2,3,4,5,6,7,8,9,10,11,12,14,16,18,20,22,24 };

#ifdef X11 
NMitem psText[] = {
	"2\240","",0,0,0,0,0,0,
	"3\240","",0,0,0,0,0,0,
	"4\240","",0,0,0,0,0,0,
	"5\240","",0,0,0,0,0,0,
	"6\240","",0,0,0,0,0,0,
	"7\240","",0,0,0,0,0,0,
	"8\240","",0,0,0,0,0,0,
	"9\240","",0,0,0,0,0,0,
	"10\240","",0,0,0,0,0,0,
	"11\240","",0,0,0,0,0,0,
	"12\240","",0,0,0,0,0,0,
	"14\240","",0,0,0,0,0,0,
	"16\240","",0,0,0,0,0,0,
	"18\240","",0,0,0,0,0,0,
	"20\240","",0,0,0,0,0,0,
	"22\240","",0,0,0,0,0,0,
	"24\240","",0,0,0,0,0,0,
	0
};
NMenu psMenu = { psText, 0, 0, 0 };
#else /* X11 */
ChkmarkItem psText[] = {
	"2\240",	0,
	"3\240",	0,
	"4\240",	0,
	"5\240",	0,
	"6\240",	0,
	"7\240",	0,
	"8\240",	0,
	"9\240",	0,
	"10\240",	0,
	"11\240",	0,
	"12\240",	0,
	"14\240",	0,
	"16\240",	0,
	"18\240",	0,
	"20\240",	0,
	"22\240",	0,
	"24\240",	0,
	0
};
Tmenu psMenu = { (Titem *) psText, 0, 0, 0, TM_TEXT | TM_ICON };
#endif /* X11 */

/* Convert from point size to index into menu. */
	int
psIndex( ps )
	int	ps;
{
	register int	i;

	for( i=0; pointSizes[i] < ps; i++ )
		;
	return i;
}

#define indexPS(i)	(pointSizes[i])



/**** Font menu  (Order must agree with constants found in cip.h) ****/

#ifdef X11
NMitem fontText[] = {
	"Roman\240",		"",0,0,0,0,0,0,
	"  Italic\240",		"",0,0,0,0,0,0,
	"  Bold\240",		"",0,0,0,0,0,0,
	"Helvetica\240",	"",0,0,0,0,0,0,
	"  Italic\240",		"",0,0,0,0,0,0,
	"  Bold\240",		"",0,0,0,0,0,0,
	"Palatino\240",		"",0,0,0,0,0,0,
	"  Italic\240",		"",0,0,0,0,0,0,
	"  Bold\240",		"",0,0,0,0,0,0,
	"Expanded\240",		"",0,0,0,0,0,0,
	"  Italic\240",		"",0,0,0,0,0,0,
	"  Bold\240",		"",0,0,0,0,0,0,
	"Constant width ",	"",0,0,0,0,0,0,
	0
};
NMenu fontMenu = { fontText, 0, 0, 0 };
#else /* X11 */
ChkmarkItem fontText[] = {
	"Roman\240",		0,
	"  Italic\240",		0,
	"  Bold\240",		0,
	"Helvetica\240",	0,
	"  Italic\240",		0,
	"  Bold\240",		0,
	"Palatino\240",		0,
	"  Italic\240",		0,
	"  Bold\240",		0,
	"Expanded\240",		0,
	"  Italic\240",		0,
	"  Bold\240",		0,
	"Constant width ",	0,
	0
};
Tmenu fontMenu = { (Titem *) fontText, 0, 0, 0, TM_TEXT | TM_ICON };
#endif /* X11 */

/* Convert from font num to index into menu. */
#define fontIndex(num)	((num)-1)

#define indexFont(i)	((i)+1)


/**** Justification menu (Order must agree with constants in cip.h) ****/

#ifdef X11
NMitem justText[] = {
	"Left\240",	"",0,0,0,0,0,0,
	"Center\240",	"",0,0,0,0,0,0,
	"Right\240",	"",0,0,0,0,0,0,
	0
};
NMenu justMenu = { justText, 0, 0, 0 };
#else /* X11 */
ChkmarkItem justText[] = {
	"Left\240",	0,
	"Center\240",	0,
	"Right\240",	0,
	0
};
Tmenu justMenu = { (Titem *) justText, 0, 0, 0, TM_TEXT | TM_ICON };
#endif /* X11 */

#define justIndex(just)		(just)

#define indexJust(i)		(i)


/**** Space menu ****/

#ifdef X11
NMitem spaceText[] = {
	"-4\240",	"",0,0,0,0,0,0,
	"-3\240",	"",0,0,0,0,0,0,
	"-2\240",	"",0,0,0,0,0,0,
	"-1\240",	"",0,0,0,0,0,0,
	"Normal\240",	"",0,0,0,0,0,0,
	"+1\240",	"",0,0,0,0,0,0,
	"+2\240",	"",0,0,0,0,0,0,
	"+3\240",	"",0,0,0,0,0,0,
	"+4\240",	"",0,0,0,0,0,0,
	"+5\240",	"",0,0,0,0,0,0,
	"+6\240",	"",0,0,0,0,0,0,
	"+7\240",	"",0,0,0,0,0,0,
	"+8\240",	"",0,0,0,0,0,0,
	"+9\240",	"",0,0,0,0,0,0,
	"+10\240",	"",0,0,0,0,0,0,
	"+11\240",	"",0,0,0,0,0,0,
	"+12\240",	"",0,0,0,0,0,0,
	"+13\240",	"",0,0,0,0,0,0,
	"+14\240",	"",0,0,0,0,0,0,
	"+15\240",	"",0,0,0,0,0,0,
	"+16\240",	"",0,0,0,0,0,0,
	"+17\240",	"",0,0,0,0,0,0,
	"+18\240",	"",0,0,0,0,0,0,
	"+19\240",	"",0,0,0,0,0,0,
	"+20\240",	"",0,0,0,0,0,0,
	"+21\240",	"",0,0,0,0,0,0,
	"+22\240",	"",0,0,0,0,0,0,
	"+23\240",	"",0,0,0,0,0,0,
	"+24\240",	"",0,0,0,0,0,0,
	"+25\240",	"",0,0,0,0,0,0,
	"+26\240",	"",0,0,0,0,0,0,
	0
};
NMenu spacingMenu = { spaceText, 0, 0, 0 };
#else /* X11 */
ChkmarkItem spaceText[] = {
	"-4\240",	0,
	"-3\240",	0,
	"-2\240",	0,
	"-1\240",	0,
	"Normal\240",	0,
	"+1\240",	0,
	"+2\240",	0,
	"+3\240",	0,
	"+4\240",	0,
	"+5\240",	0,
	"+6\240",	0,
	"+7\240",	0,
	"+8\240",	0,
	"+9\240",	0,
	"+10\240",	0,
	"+11\240",	0,
	"+12\240",	0,
	"+13\240",	0,
	"+14\240",	0,
	"+15\240",	0,
	"+16\240",	0,
	"+17\240",	0,
	"+18\240",	0,
	"+19\240",	0,
	"+20\240",	0,
	"+21\240",	0,
	"+22\240",	0,
	"+23\240",	0,
	"+24\240",	0,
	"+25\240",	0,
	"+26\240",	0,
	0
};
Tmenu spacingMenu = { (Titem *)spaceText, 0, 0, 0, TM_TEXT | TM_ICON };
#endif /* X11 */

#define spaceMin	-4
#define spaceMax	26

#define spacingIndex(sp)	((sp)+4)

#define indexSpacing(i)		((i)-4)


/**** Density menu ****/

#ifdef X11
NMitem densityText[] = {
	"Solid\240",	"",0,0,0,0,0,0,
	"Dashed\240",	"",0,0,0,0,0,0,
	"Dotted\240",	"",0,0,0,0,0,0,
	0
};
NMenu densityMenu = { densityText, 0, 0, 0 };
#else /* X11 */
ChkmarkItem densityText[] = {
	"Solid\240",	0,
	"Dashed\240",	0,
	"Dotted\240",	0,
	0
};
Tmenu densityMenu = { (Titem *) densityText, 0, 0, 0, TM_TEXT | TM_ICON };
#endif /* X11 */

#define densityIndex(den)	(den)

#define indexDensity(i)		(i)


/**** Arrow menu ****/

#ifdef X11
NMitem arrowText[] = {
	"No arrows\240",	"",0,0,0,0,0,0,
	"Start end\240",	"",0,0,0,0,0,0,
	"Finish end\240",	"",0,0,0,0,0,0,
	"Both ends\240",	"",0,0,0,0,0,0,
	0
};
NMenu arrowMenu = { arrowText, 0, 0, 0 };
#else /* X11 */
ChkmarkItem arrowText[] = {
	"No arrows\240",	0,
	"Start end\240",	0,
	"Finish end\240",	0,
	"Both ends\240",	0,
	0
};
Tmenu arrowMenu = { (Titem *) arrowText, 0, 0, 0, TM_TEXT | TM_ICON };
#endif /* X11 */

#define arrowIndex(a)		(a)

#define indexArrow(i)		(i)


/**** Grid menu ****/

#ifdef FINEALIGN
int gridLevels[] = { 0,-1,4,5,6,7,8,9,10,11,12,13,14,15,16 };
#endif

#ifdef X11
NMitem gridText[] = {
#ifdef FINEALIGN
	"Off\240",	"",0,0,0,0,0,0,
	"Follow Alignment\240",	"",0,0,0,0,0,0,
	"4\240",	"",0,0,0,0,0,0,
	"5\240",	"",0,0,0,0,0,0,
	"6\240",	"",0,0,0,0,0,0,
	"7\240",	"",0,0,0,0,0,0,
	"8\240",	"",0,0,0,0,0,0,
	"9\240",	"",0,0,0,0,0,0,
	"10\240",	"",0,0,0,0,0,0,
	"11\240",	"",0,0,0,0,0,0,
	"12\240",	"",0,0,0,0,0,0,
	"13\240",	"",0,0,0,0,0,0,
	"14\240",	"",0,0,0,0,0,0,
	"15\240",	"",0,0,0,0,0,0,
	"16\240",	"",0,0,0,0,0,0,
	0
#else /* FINEALIGN */
	"Off\240",	"",0,0,0,0,0,0,
	"On\240",	"",0,0,0,0,0,0,
	0
#endif /* FINEALIGN */
};
NMenu gridMenu = { gridText, 0, 0, 0 };
#else /* X11 */
ChkmarkItem gridText[] = {
#ifdef FINEALIGN
	"Off\240",	0,
	"Follow Alignment\240",	0,
	"4\240",	0,
	"5\240",	0,
	"6\240",	0,
	"7\240",	0,
	"8\240",	0,
	"9\240",	0,
	"10\240",	0,
	"11\240",	0,
	"12\240",	0,
	"13\240",	0,
	"14\240",	0,
	"15\240",	0,
	"16\240",	0,
	0
#else /* FINEALIGN */
	"Off\240",	0,
	"On\240",	0,
	0
#endif /* FINEALIGN */
};
Tmenu gridMenu = { (Titem *) gridText, 0, 0, 0, TM_TEXT | TM_ICON };
#endif /* X11 */

#ifdef FINEALIGN
/* Convert from gridSize to index into menu. */
	int
gridIndex( g )
	int	g;
{
	register int	i;

	/* Special case for currGridsize < 0 (follow alignment) */
	if ( g < 0 )
	{
		return 1;
	}

	for( i=0; gridLevels[i] < g; i++ )
		;
	return i;
}

#define indexGrid(i)		(gridLevels[(i)])

#else /* FINEALIGN */

#define gridIndex(a)		(a)

#define indexGrid(i)		(i)

#endif /* FINEALIGN */



/**** Alignment menu ****/

#ifdef FINEALIGN
int alignLevels[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
#else
int alignLevels[] = { 2,8,16 };
#endif

#ifdef X11
NMitem alignText[] = {
#ifdef FINEALIGN
	"1\240",	"",0,0,0,0,0,0,
	"2 (fine)\240",	"",0,0,0,0,0,0,
	"3\240",	"",0,0,0,0,0,0,
	"4\240",	"",0,0,0,0,0,0,
	"5\240",	"",0,0,0,0,0,0,
	"6\240",	"",0,0,0,0,0,0,
	"7\240",	"",0,0,0,0,0,0,
	"8 (medium)\240",	"",0,0,0,0,0,0,
	"9\240",	"",0,0,0,0,0,0,
	"10\240",	"",0,0,0,0,0,0,
	"11\240",	"",0,0,0,0,0,0,
	"12\240",	"",0,0,0,0,0,0,
	"13\240",	"",0,0,0,0,0,0,
	"14\240",	"",0,0,0,0,0,0,
	"15\240",	"",0,0,0,0,0,0,
	"16 (coarse)\240",	"",0,0,0,0,0,0,
	0
#else /* FINEALIGN */
	"fine   (2)\240", 	"",0,0,0,0,0,0,
	"medium (8)\240", 	"",0,0,0,0,0,0,
	"coarse (16)\240", 	"",0,0,0,0,0,0,
	0
#endif /* FINEALIGN */
};
NMenu alignMenu = { alignText, 0, 0, 0 };
#else /* X11 */
ChkmarkItem alignText[] = {
#ifdef FINEALIGN
	"1\240",	0,
	"2 (fine)\240",	0,
	"3\240",	0,
	"4\240",	0,
	"5\240",	0,
	"6\240",	0,
	"7\240",	0,
	"8 (medium)\240",	0,
	"9\240",	0,
	"10\240",	0,
	"11\240",	0,
	"12\240",	0,
	"13\240",	0,
	"14\240",	0,
	"15\240",	0,
	"16 (coarse)\240",	0,
	0
#else /* FINEALIGN */
	"fine   (2)\240", 	0,
	"medium (8)\240", 	0,
	"coarse (16)\240", 	0,
	0
#endif /* FINEALIGN */
};
Tmenu alignMenu = { (Titem *)alignText, 0, 0, 0, TM_TEXT | TM_ICON };
#endif /* X11 */


/* Convert from point size to index into menu. */
	int
alignIndex( a )
	int	a;
{
	register int	i;

	for( i=0; alignLevels[i] < a; i++ )
		;
	return i;
}

#define indexAlign(i)	(alignLevels[i])



#ifdef HELPSCALE
/**** Display Scale Menu ****/

/*
 * The order of the entries in this menu must be kept in sync
 * with the defines in cip.h .
 */

#ifdef X11
NMitem labelText[] = {
	"none\240",		"",0,0,0,0,0,0,
	"inches\240",		"",0,0,0,0,0,0,
	"feet and inches\240",	"",0,0,0,0,0,0,
	"feet\240",		"",0,0,0,0,0,0,
	"yards\240",		"",0,0,0,0,0,0,
	"miles\240",		"",0,0,0,0,0,0,
	"millimeters\240",	"",0,0,0,0,0,0,
	"meters\240",		"",0,0,0,0,0,0,
	"kilometers\240",	"",0,0,0,0,0,0,
	0
};
NMenu labelMenu = { labelText, 0, 0, 0 };
#else /* X11 */
ChkmarkItem labelText[] = {
	"none\240",		0,
	"inches\240",		0,
	"feet and inches\240",	0,
	"feet\240",		0,
	"yards\240",		0,
	"miles\240",		0,
	"millimeters\240",	0,
	"meters\240",		0,
	"kilometers\240",	0,
	0
};
Tmenu labelMenu = { (Titem *)labelText, 0, 0, 0, TM_TEXT | TM_ICON };
#endif /* X11 */


#ifdef X11
NMitem scaleText[] = {
	"Off\240",			"",0,0,0,0,0,0,
	"Scale Factor Only\240",	"",0,0,0,0,0,0,
	"Label Only\240",		"",0,0,0,0,0,0,
	"Both On\240",			"",0,0,0,0,0,0,
	"Set Scale Factor\240",		"",0,0,0,0,0,0,
	"Set Label\240",		"",0,&labelMenu,0,0,0,0,
	0
};
NMenu scaleMenu = { scaleText, 0, 0, 0 };
#else /* X11 */
MidMenuChkmarkItem scaleText[] = {
	"Off\240",			0,0,
	"Scale Factor Only\240",	0,0,
	"Label Only\240",		0,0,
	"Both On\240",			0,0,
	"Set Scale Factor\240",		0,0,
	"Set Label\240",		&labelMenu,0,
	0
};
Tmenu scaleMenu = { (Titem *)scaleText, 0, 0, 0, TM_TEXT|TM_ICON|TM_NEXT };
#endif /* X11 */

/* Convert from scale to index into menu. */
#define scaleIndex(a) (a)
#define indexScale(i) (i)

#define labelIndex(a) (a)
#define indexLabel(i) (i)

#endif /* HELPSCALE */



/**** Line menu ****/

#ifdef X11
NMitem lineText[] = {
	"Density\240",		"",0,&densityMenu,0,0,0,0,
	"Arrow\240",		"",0,0,0,0,0,0,
        "Reflect x\240",	"",0,0,0,0,0,0,
	"Reflect y\240",	"",0,0,0,0,0,0,
	"Copy\240",		"",0,0,0,0,0,0,
	"Delete\240",		"",0,0,0,0,0,0,
	0
};
NMenu lineMenu = { lineText, 0, 0, 0 };
#else /* X11 */
TopMenuItem lineText[] = {
	"Density\240",		&densityMenu,
	"Arrow\240",		0,	
        "Reflect x\240",	0,
	"Reflect y\240",	0,
	"Copy\240",		0,
	"Delete\240",		0,
	0
};
Tmenu lineMenu = { (Titem *)lineText, 0, 0, 0, TM_TEXT|TM_NEXT };
#endif /* X11 */


/**** Global line menu ****/

#ifdef X11
NMitem gLineText[] = {
	"Density\240",		"",0,&densityMenu,0,0,0,0,
	"Arrows\240",		"",0,&arrowMenu,0,0,0,0,
	0
};
NMenu gLineMenu = { gLineText, 0, 0, 0 };
#else /* X11 */
TopMenuItem gLineText[] = {
	"Density\240",		&densityMenu,
	"Arrows\240",		&arrowMenu,
	0
};
Tmenu gLineMenu = { (Titem *)gLineText, 0, 0, 0, TM_TEXT|TM_NEXT };
#endif /* X11 */


/**** Box menu ****/

#ifdef X11
NMitem boxText[] = {
	"Density\240",		"",0,&densityMenu,0,0,0,0,
	"Copy\240",		"",0,0,0,0,0,0,
	"Delete\240",		"",0,0,0,0,0,0,
	0
};
NMenu boxMenu = { boxText, 0, 0, 0 };
#else /* X11 */
TopMenuItem boxText[] = {
	"Density\240",		&densityMenu,
	"Copy\240",		0,	
	"Delete\240",		0,
	0
};
Tmenu boxMenu = { (Titem *)boxText, 0, 0, 0, TM_TEXT|TM_NEXT };
#endif /* X11 */


/**** Circle menu ****/

#ifdef X11
NMitem circleText[] = {
	"Copy\240",	"",0,0,0,0,0,0,
	"Delete\240",	"",0,0,0,0,0,0,
	0
};
NMenu circleMenu = { circleText, 0, 0, 0 };
#else /* X11 */
BasicItem circleText[] = {
	"Copy\240",
	"Delete\240",
	0
};
Tmenu circleMenu = { (Titem *)circleText, 0, 0, 0, TM_TEXT };
#endif /* X11 */


/**** Spline menu ****/

#ifdef X11
NMitem splineText[] = {
	"Arrow\240",		"",0,0,0,0,0,0,
        "Reflect x\240",	"",0,0,0,0,0,0,
	"Reflect y\240",	"",0,0,0,0,0,0,
	"Copy\240",		"",0,0,0,0,0,0,
	"Delete\240",		"",0,0,0,0,0,0,
	0
};
NMenu splineMenu = { splineText, 0, 0, 0 };
#else /* X11 */
BasicItem splineText[] = {
	"Arrow\240",
        "Reflect x\240",
	"Reflect y\240",
	"Copy\240",
	"Delete\240",
	0
};
Tmenu splineMenu = { (Titem *)splineText, 0, 0, 0, TM_TEXT };
#endif /* X11 */


/**** Arc menu ****/

#ifdef X11
NMitem arcText[] = {
        "Reflect x\240",	"",0,0,0,0,0,0,
	"Reflect y\240",	"",0,0,0,0,0,0,
	"Copy\240",		"",0,0,0,0,0,0,
	"Delete\240",		"",0,0,0,0,0,0,
	0
};
NMenu arcMenu = { arcText, 0, 0, 0 };
#else /* X11 */
BasicItem arcText[] = {
        "Reflect x\240",
	"Reflect y\240",
	"Copy\240",
	"Delete\240",
	0
};
Tmenu arcMenu = { (Titem *)arcText, 0, 0, 0, TM_TEXT };
#endif /* X11 */


/**** Macro menu ****/

#ifdef X11
NMitem macroText[] = {
	"Edit\240",		"",0,0,0,0,0,0,
	"Separate\240",		"",0,0,0,0,0,0,
	"Reflect x\240", 	"",0,0,0,0,0,0,
	"Reflect y\240", 	"",0,0,0,0,0,0,
	"Copy\240", 		"",0,0,0,0,0,0,
	"Delete\240", 		"",0,0,0,0,0,0,
	0
};
NMenu macroMenu = { macroText, 0, 0, 0 };
#else /* X11 */
BasicItem macroText[] = {
	"Edit\240",
	"Separate\240",
	"Reflect x\240", 
	"Reflect y\240", 
	"Copy\240", 
	"Delete\240", 
	0
};
Tmenu macroMenu = { (Titem *)macroText, 0, 0, 0, TM_TEXT };
#endif /* X11 */


/**** Text object menu ****/

#ifdef X11
NMitem textText[] = {
	"Point size\240",	"",0,&psMenu,0,0,0,0,
	"Font style\240",	"",0,&fontMenu,0,0,0,0,
	"Justify\240",		"",0,&justMenu,0,0,0,0,
	"Spacing\240",		"",0,&spacingMenu,0,0,0,0,
	"Separate\240",		"",0,0,0,0,0,0,
	"Copy\240",		"",0,0,0,0,0,0,
	"Delete\240",		"",0,0,0,0,0,0,
	0
};
NMenu textMenu = { textText, 0, 0, 0 };
#else /* X11 */
TopMenuItem textText[] = {
	"Point size\240",	&psMenu,
	"Font style\240",	&fontMenu,
	"Justify\240",		&justMenu,
	"Spacing\240",		&spacingMenu,
	"Separate\240",		0,
	"Copy\240",		0,
	"Delete\240",		0,
	0
};
Tmenu textMenu = { (Titem *) textText, 0, 0, 0, TM_TEXT|TM_NEXT };
#endif /* X11 */


/**** Global TEXT menu ****/

#ifdef X11
NMitem gTextData[] = {
	"Point size\240",	"",0,&psMenu,0,0,0,0,
	"Font style\240",	"",0,&fontMenu,0,0,0,0,
	"Justify\240",		"",0,&justMenu,0,0,0,0,
	"Spacing\240",		"",0,&spacingMenu,0,0,0,0,
	0
};
NMenu gTextMenu = { gTextData, 0, 0, 0 };
#else /* X11 */
TopMenuItem gTextData[] = {
	"Point size\240",	&psMenu,
	"Font style\240",	&fontMenu,
	"Justify\240",		&justMenu,
	"Spacing\240",		&spacingMenu,
	0
};
Tmenu gTextMenu = { (Titem *) gTextData, 0, 0, 0, TM_TEXT|TM_NEXT };
#endif /* X11 */


/**** Command menu ****/

#define GET		0
#define PUT		1
#define CLEAR		2
#define REDRAW		3
#define DEFINEMACRO	4
#define UNDO		5
#define GRIDTOGGLE	6
#define ALIGNMENT	7
#define XCIPINFO	8
#define XCIPVERS	9

#ifdef HELPSCALE
# define DISPSCALE	10
# ifndef X11
#  define ORIGWINDOW	11
# else /* X11 */
#  define QUIT		11
# endif /* X11 */
#else  /* HELPSCALE */
# ifndef X11
#  define ORIGWINDOW	10
# else /* X11 */
#  define QUIT		10
# endif /* X11 */
#endif /* HELPSCALE */


#ifdef X11
NMitem commandText[] = {
	"Get file\240",		"",0,0,0,0,0,0,
	"Put file\240",		"",0,0,0,0,0,0,
	"Clear screen\240", 	"",0,0,0,0,0,0,
	"Redraw screen\240",	"",0,0,0,0,0,0,
	"Define macro\240",	"",0,0,0,0,0,0,
	"Undo\240",		"",0,0,0,0,0,0,
	"Grid\240", 		"",0,&gridMenu,0,0,0,0,
	"Alignment\240", 	"",0,&alignMenu,0,0,0,0,
	"Information\240",	"",0,0,0,0,0,0,
	"Version\240",		"",0,0,0,0,0,0,
#ifdef HELPSCALE
	"Display Scale\240",	"",0,&scaleMenu,0,0,0,0,
#endif /* HELPSCALE */
	"Quit\240",		"",0,0,0,0,0,0,
	0
};
NMenu commandMenu = { commandText, 0, 0, 0 };
#else /* X11 */
TopMenuItem commandText[] = {
	"Get file\240",		0,		
	"Put file\240",		0,	
	"Clear screen\240", 	0,
	"Redraw screen\240",	0,
	"Define macro\240",	0,
	"Undo\240",		0,
	"Grid\240", 		&gridMenu,
	"Alignment\240", 	&alignMenu,
	"Information\240",	0,
	"Version\240",		0,
#ifdef HELPSCALE
	"Display Scale\240",	&scaleMenu,
#endif /* HELPSCALE */
	"Close window\240",	0,
	0
};
Tmenu commandMenu = { (Titem *)commandText, 0, 0, 0, TM_TEXT|TM_NEXT };
#endif /* X11 */





	struct thing *
reflect(t, p, axis)
	register struct thing *	t;
	Point		p;	/* Offset */
	char		axis;	/* Either 'x' or 'y' */
{
	struct thing *	h;

	drawSelectionLines (t, p);
	draw (t, p);
	h = (axis == 'x') ?
	    Transform (t, Pt(0, t->bb.corner.y+t->bb.origin.y))
	  : Transform (t, Pt(t->bb.corner.x+t->bb.origin.x, 0));
	draw (t, p);
	drawSelectionLines (t, p);
	return (h);
}


	void
copyOperation()
{
	if( !noSpace ) {
		copyFlag = 1;
		changeButtons( COPYbuttons );
	}
}


	struct thing *
deleteOperation(t,p)
	struct thing *	t;
	Point		p;	/* Offset */
{
	drawSelectionLines(t,p);
	draw(t, p);

	t = deleteThing(t);	

	thingSelected = 0;
	copyFlag = 0;
	changeButtons( INITbuttons );
	return( t );
}


	struct thing *
separateLineText(t,p)
	struct thing *	t;
	Point		p;
{
	register int	lp;
	register int	i;
	register int	q;
	char *		ls;
	char *		s;
	char *		ss;
	struct thing *	nt;
	struct thing *	h;
	Font *		f;
	int		ncr = 0; /* number of carriage returns */
	int		start = 0;
	
	draw(t,p); /* erase present text */
	h = t;
	f = t->otherValues.text.f->f;
	ls = t->otherValues.text.s;
	
	for(i=0;ls[i]!='\0';i++) {
		if(ls[i] == '\r')
			ncr++;
	}
	
	lp = -( ((fontheight(f)+t->otherValues.text.spacing)*ncr) >> 1 );
	
	for(i = 0; i <= ncr; i++) {
		for(q = start; (ls[q] != '\r') && (ls[q] != '\0'); q++)
			;

		if ( (ss=getSpace(q-start+1)) != NULL ) {
			s = ss;
			for(q = start; (ls[q] != '\r') && (ls[q] != '\0'); q++)
				*s++ = ls[q];
			*s='\0';
		}

		nt = newText( Pt(t->origin.x, (t->origin.y + lp) ), ss);
		nt->otherValues.text.f = 
		    findFont(t->otherValues.text.f->ps,
			     t->otherValues.text.f->num);
		nt->otherValues.text.just = t->otherValues.text.just;
		boundingBox(nt);
		draw(nt,p);
		h = insert(nt,h);
		start = q + 1;
		lp = lp + fontheight(f) + t->otherValues.text.spacing;
	}

	thingSelected = 0;
	changeButtons( INITbuttons );

	return( deleteThing(t) );
}


	struct thing *
displayThingMenu(m,ta,p) 
	Point			m;	/* Mouse location */
	Point			p;	/* Offset */
	struct thing **	ta;	/* address of thing to be displayed */
{
	register struct thing *	t;
	register short		item;
	register int		b;
	int			oldED; 
	register struct thing *	pts;
	register struct thing *	h;
	register struct thing *	g;
	struct thing *		f;
	struct macro *		mac;
	Rectangle		r; 
#ifdef X11
	NMitem *		ti;
#else /* X11 */
	Titem *			ti;
#endif /* X11 */

	t = *ta;

	switch(t->type) {

	case LINE: 
		setChkmark( &densityMenu, densityIndex(t->border) );

#ifdef X11
		ti = hmenuhit( &lineMenu, 3 );
		if( ti == (NMitem * )NULL )
#else /* X11 */
		ti = tmenuhit( &lineMenu, 3, TM_EXPAND );
		if( ti == (Titem * )NULL )
#endif /* X11 */
			break;

		switch( indexHit(&lineMenu) ) {

		case 0: /* Density */
			draw(t,p);
			t->border = indexDensity( indexHit(&densityMenu) );
			draw(t,p);
				break;
	
		case 1: /* Arrow - add or remove arrowheads */
			draw(t,p);
			if (distance(m,t->origin) < 
			    distance(m,t->otherValues.end)) {
				if ((t->arrow==startARROW)||
				    (t->arrow==doubleARROW)) {
					t->arrow -= startARROW;
				}
				else {
					t->arrow += startARROW;
				}
			}
			else {
				if ((t->arrow==endARROW)||
				    (t->arrow==doubleARROW)) {
					t->arrow -= endARROW;
				}
				else {
					t->arrow += endARROW;
				}
			}
			draw(t,p);
			break;
	
		case 2: /* Reflect x */
			t = reflect(t, p, 'x');
			break;
	
		case 3: /* Reflect y */
			t = reflect(t, p, 'y');
			break;
	
		case 4: /* Copy */
			copyOperation();
			break;
	
		case 5: /* Delete */
			t = deleteOperation(t,p);
			*ta = TNULL;
		}
		break;

	case BOX:
		setChkmark( &densityMenu, densityIndex(t->border) );

#ifdef X11
		ti = hmenuhit( &boxMenu, 3 );
		if( ti == (NMitem * )NULL )
#else /* X11 */
		ti = tmenuhit( &boxMenu, 3, TM_EXPAND );
		if( ti == (Titem * )NULL )
#endif /* X11 */
			break;

		switch( indexHit(&boxMenu) ) {

		case 0: /* Density */
			draw(t,p);
			t->border = indexDensity( indexHit(&densityMenu) );
			draw(t,p);
			break;
	
		case 1: /* Copy */
			copyOperation();
			break;
	
		case 2: /* Delete */
			t = deleteOperation(t,p);
			*ta = TNULL;
		}
		break;

	case MACRO:
#ifdef X11
		ti = hmenuhit( &macroMenu, 3 );
		if( ti == (NMitem * )NULL )
#else /* X11 */
		ti = tmenuhit( &macroMenu, 3, 0 );
		if( ti == (Titem * )NULL )
#endif /* X11 */
			break;

		switch( item = indexHit(&macroMenu) ) {

		case 0: /* edit macro */
			oldED = editDepth;
			pts = t->otherValues.list->parts;

			/* Draw edit button when editDepth==1, other times */
			/* undraw button. */
			if ((editDepth)!=0) {
				drawEDbutton(editDepth);
			}
			drawEDbutton(++editDepth);

			changeButtons (INITbuttons);
			thingSelected = 0;
			p = sub( p, add( drawOffset, scrollOffset ) );

			while (editDepth>oldED) {
				pts = doMouseButtons(pts, add(p,t->origin) );
#ifdef X11
				/* nap(2); */
#endif /* X11 */
			}

			p = add( p, add( drawOffset, scrollOffset) );
			t->otherValues.list->parts = pts;
			if (pts != TNULL) {
				r = macroBB(pts);	/* get outline */

				/* Origin is offset by new r.origin */
				t->origin = add( t->origin, r.origin );

				/* Offset all parts by new r.origin */
				g = pts;
				do {
					makeRelative(g,r.origin);	
					g = g->next;
				} while (g != pts);

				/* Set list->bb such that its origin is 0 */
				t->otherValues.list->bb = rsubp( r, r.origin);

				boundingBox( t );
			        undo_clear();

			} else {
			    /* Macro is now empty - delete all references. */
			    undo_clear();  /* Make certain macro is not in 
					      undo list. */
			    mac = t->otherValues.list;
			    while(1) {
				if( t->type == MACRO ) {
			            if( t->otherValues.list == mac ) {
				        /* Must be very careful here.  The macro
				           node itself is deleted on the last
				           reference. */
				        if( mac->useCount <= 1 ) {
					    /* Last reference - delete and then
					       leave loop. */
				            t = deleteThing( t );
					    break;
				        } else {
				            t = deleteThing( t );
				        }
			            } else {
				        t = t->next;
				    }
			        } else {
				    t = t->next;
			        }
#ifndef X11
				sw(1);
#endif
			    }

			    *ta = TNULL;
			}
			doRedraw();
			break;

		case 1: /* separate */
			if (firstThing == t) {
				firstThing = t->next;
				if (firstThing == t) {
					firstThing = TNULL;
				}
			}
			h = Remove(t);	/* Cut macro from thing list */
			t->otherValues.list->useCount--;
			m = sub(Pt(0,0),t->origin);

			/* Go thru macro's thing list and make 
			   everything relative */
			if ((g=t->otherValues.list->parts)!=TNULL) {
				if (t->otherValues.list->useCount > 0) {

					do {	/* Copy list */
						g = g->next;

						/* Only arcs are not copied 
						   at their origins. */

						f = copyThing (g, 
						      (g->type == ARC) ?
				 		       g->otherValues.arc.start
						      : g->origin, 
						      Pt(0,0), 0);
						/* 0: object wont be drawn */ 

						f = makeRelative (f, m);
						h = insert (f, h);
					} while (g != t->otherValues.list->parts);
				} else {
					do{	/* Move list */
						g = g->next;
						g = makeRelative(g,m);
						f = g;
						g = Remove (f);
						h = insert (f, h);
					} while (g != TNULL);

					removeMacro (t->otherValues.list);
				}
			}
	
			thingSelected = 0;
			copyFlag = 0;
			changeButtons(INITbuttons);
			free(t);	/* Remove macro thing */
			*ta = TNULL;
			if (firstThing == TNULL) {
				firstThing = h;
			}
			t = h;
			undo_clear();
			break;

		case 2: /* Reflect x */
		case 3: /* Reflect y */
			draw (t, p);
			if ((g = t->otherValues.list->parts) != TNULL) {
				r = t->otherValues.list->bb;
				do {
					h = (item == 2) ?
				  	  Transform(g,Pt(0,r.origin.y+r.corner.y))
					: Transform(g,Pt(r.origin.x+r.corner.x,0));
					g = g->next;
				} while (g != t->otherValues.list->parts);
			}

			if (editDepth) { /* Only draw if not at level 0 */
				draw (t, p);
			} else {	 /* otherwise, redraw entire screen. */
				doRedraw();
			}
			break;
			
	
		case 4: /* Copy */
			copyOperation();
			break;
	
		case 5: /* Delete */
			t = deleteOperation(t,p);
			*ta = TNULL;
		}
		break;

	case TEXT:
		setChkmark( &psMenu,	psIndex(t->otherValues.text.f->ps) );
		setChkmark( &fontMenu,	fontIndex(t->otherValues.text.f->num) );
		setChkmark( &justMenu,	justIndex(t->otherValues.text.just) );
		setChkmark( &spacingMenu, spacingIndex(t->otherValues.text.spacing) );

#ifdef X11
		ti = hmenuhit( &textMenu, 3 );
		if( ti == (NMitem *) NULL )
#else /* X11 */
		ti = tmenuhit( &textMenu, 3, TM_EXPAND );
		if( ti == (Titem *) NULL )
#endif /* X11 */
			break;

		switch( indexHit( &textMenu ) ) {

		case 0: /* point size */
			draw(t,p); /* erase thing */
			t->otherValues.text.f->useCount--;
			t->otherValues.text.f = 
			findFont(indexPS(indexHit(&psMenu)),
				t->otherValues.text.f->num);
			boundingBox(t);
			draw(t,p);
			break;

		case 1: /* font style */
			draw(t,p);
			t->otherValues.text.f->useCount--;
			t->otherValues.text.f = 
			findFont(t->otherValues.text.f->ps,
				indexFont(indexHit(&fontMenu)));
			boundingBox(t);
			draw(t,p);
			break;

		case 2: /* justification */
			draw(t,p);
			t->otherValues.text.just = 
				indexJust(indexHit( &justMenu ));
			boundingBox(t);
			draw(t,p);
			break;

		case 3: /* spacing */
			draw(t,p);
			t->otherValues.text.spacing = 
				indexSpacing(indexHit(&spacingMenu));
			boundingBox(t);
			draw(t,p);
			break;

		case 4: /* separate lines of text */
			t = separateLineText(t,p);
			*ta = TNULL;
			undo_clear();
			break;
	
		case 5: /* Copy */
			copyOperation();
			break;
	
		case 6: /* Delete */
			t = deleteOperation(t,p);
			*ta = TNULL;
			break;
		}
		break;

	case CIRCLE:
	case ELLIPSE: 
#ifdef X11
		ti = hmenuhit( &circleMenu, 3 );
		if( ti == (NMitem * )NULL )
#else /* X11 */
		ti = tmenuhit( &circleMenu, 3, 0 );
		if( ti == (Titem * )NULL )
#endif /* X11 */
			break;

		switch( indexHit(&circleMenu) ) {
	
		case 0: /* Copy */
			copyOperation();
			break;
	
		case 1: /* Delete */
			t = deleteOperation(t,p);
			*ta = TNULL;
		}
		break;

	case ARC:
#ifdef X11
		ti = hmenuhit( &arcMenu, 3 );
		if( ti == (NMitem * )NULL )
#else /* X11 */
		ti = tmenuhit( &arcMenu, 3, TM_EXPAND );
		if( ti == (Titem * )NULL )
#endif /* X11 */
			break;

		switch( indexHit(&arcMenu) ) {

		case 0: /* Reflect x */
			t = reflect(t, p, 'x');
			break;

		case 1: /* Reflect y */
			t = reflect(t, p, 'y');
			break;
	
		case 2: /* Copy */
			copyOperation();
			break;
	
		case 3: /* Delete */
			t = deleteOperation(t,p);
			*ta = TNULL;
		}
		break;

	case SPLINE: 
#ifdef X11
		ti = hmenuhit( &splineMenu, 3 );
		if( ti == (NMitem * )NULL )
#else /* X11 */
		ti = tmenuhit( &splineMenu, 3, 0 );
		if( ti == (Titem * )NULL )
#endif /* X11 */
			break;

		switch( indexHit(&splineMenu) ) {

		case 0: /* arrow */
			b = t->otherValues.spline.used;
			if (distance(m,t->origin)<
			    distance(m,t->otherValues.spline.plist[b])) {
				if ((t->arrow==startARROW)||
				    (t->arrow==doubleARROW)) {
					t->arrow -= startARROW;
				} else {
					t->arrow += startARROW;
				}
				arrow(add(p,t->otherValues.spline.plist[2]),
				      add(p,t->otherValues.spline.plist[1]));
			} else {
				if ((t->arrow==endARROW)||
				    (t->arrow==doubleARROW)) {
					t->arrow -= endARROW;
				} else {
					t->arrow += endARROW;
				}
				arrow(add(p,t->otherValues.spline.plist[b-2]),
				add(p,t->otherValues.spline.plist[b-1]));
			}
			break;
	
		case 1: /* Reflect x */
			t = reflect(t, p, 'x');
			break;
	
		case 2: /* Reflect y */
			t = reflect(t, p, 'y');
			break;
	
		case 3: /* Copy */
			copyOperation();
			break;
	
		case 4: /* Delete */
			t = deleteOperation(t,p);
			*ta = TNULL;
		}
		break;

	}	/* end of switch on t->type */

	return(t);
}



RUsure ()
{
	int savebuttonstate;
	int ret;

	ret = 0;
	cursswitch (&rusure);
	savebuttonstate = buttonState;
	if (CORRECTSIZE) {
		changeButtons( QUITbuttons );
	}
	while (!button123()) {
		if (P->state & RESHAPED)
			redrawLayer();
		nap(2);
	}
	if (button3())
		ret = 1;
	changeButtons( savebuttonstate );
	while (button123())
		nap(2);
	cursSwitch();
	return (ret);
}


	struct thing *
displayCommandMenu(h, offset) 
	struct thing *		h;
	Point 			offset;
{
#ifdef X11
	register NMitem *	ti;
#else /* X11 */
	register Titem *	ti;
#endif /* X11 */
	int			newAlign;
	int			newGrid;

	switch( currentBrush ) {

	case TEXT:
		setChkmark( &psMenu,	  psIndex(currPS) );
		setChkmark( &fontMenu,	  fontIndex(currFont) );
		setChkmark( &justMenu,	  justIndex(currJust) );
		setChkmark( &spacingMenu, spacingIndex(currSpacing) );

#ifdef X11
		ti = hmenuhit( &gTextMenu, 3 );
		if( ti == (NMitem *) NULL )
#else /* X11 */
		ti = tmenuhit( &gTextMenu, 3, TM_EXPAND );
		if( ti == (Titem *) NULL )
#endif /* X11 */
			break;

		switch( indexHit( &gTextMenu ) ) {
		case 0:	/* Point size */
			currPS = indexPS( indexHit( &psMenu ) );
			break;
		case 1:	/* Font style */
			currFont = indexFont( indexHit( &fontMenu ) );
			break;
		case 2:	/* Justify */
			currJust = indexJust( indexHit( &justMenu ) );
			break;
		case 3:	/* Spacing */
			currSpacing = indexSpacing( indexHit( &spacingMenu ) );
			break;
		}
		break;

	case BOX:
		setChkmark( &densityMenu,  densityIndex(currBoxType) );

#ifdef X11
		ti = hmenuhit( &densityMenu, 3 );
		if( ti == (NMitem *) NULL )
#else /* X11 */
		ti = tmenuhit( &densityMenu, 3, 0 );
		if( ti == (Titem *) NULL )
#endif /* X11 */
			break;

		currBoxType = indexDensity( indexHit( &densityMenu ) );

		break;

	case SPLINE:
		setChkmark( &arrowMenu,  arrowIndex(currSplineArrows) );

#ifdef X11
		ti = hmenuhit( &arrowMenu, 3 );
		if( ti == (NMitem *) NULL )
#else /* X11 */
		ti = tmenuhit( &arrowMenu, 3, 0 );
		if( ti == (Titem *) NULL )
#endif /* X11 */
			break;

		currSplineArrows = indexArrow( indexHit( &arrowMenu ) );

		break;

	case LINE:
		setChkmark( &densityMenu,  densityIndex(currLineType) );
		setChkmark( &arrowMenu,  arrowIndex(currLineArrows) );

#ifdef X11
		ti = hmenuhit( &gLineMenu, 3 );
		if( ti == (NMitem *) NULL )
#else /* X11 */
		ti = tmenuhit( &gLineMenu, 3, TM_EXPAND );
		if( ti == (Titem *) NULL )
#endif /* X11 */
			break;

		switch( indexHit( &gLineMenu ) ) {

		case 0:	/* Density */
			currLineType = indexDensity( indexHit( &densityMenu ) );
			break;

		case 1:	/* Arrows */
			currLineArrows = indexArrow( indexHit( &arrowMenu ) );
			break;
		}
		break;

	case CIRCLE:
	case ELLIPSE:
	case ARC:
	case MACRO:
		/* No menu when these brushes are selected */
		for( ; button3(); nap(2) );
		break;

	default:	/* Use main menu when no brush is selected */
		setChkmark( &alignMenu, alignIndex(currAlignment) );
#ifdef FINEALIGN
		setChkmark( &gridMenu,  gridIndex(currGridsize) );
#else /* FINEALIGN */
		setChkmark( &gridMenu,  gridIndex(gridState) );
#endif /* FINEALIGN */
#ifdef HELPSCALE
# ifdef X11
		setChkmark( &scaleMenu, scaleIndex(currScaleType) );
# else
		setChkmarkMidmenu( &scaleMenu, scaleIndex(currScaleType) );
# endif /* X11 */
		setChkmark( &labelMenu, labelIndex(currLabelType) );
#endif /* HELPSCALE */

#ifdef X11
		ti = hmenuhit( &commandMenu, 3 );
		if( ti == (NMitem *) NULL )
#else /* X11 */
		ti = tmenuhit( &commandMenu, 3, TM_EXPAND );
		if( ti == (Titem *) NULL )
#endif /* X11 */
			break;

		switch( indexHit( &commandMenu ) ) {

		case GET:  /* Get file */
			fontBells = 0;
			h = doGet(h);
			ringbell();
			fontBells = 1;
			break;

		case PUT:  /* Put file */
			undo_clear();  /* Must clear out any macros in save area else 
							  they will be written out even if unused! */
			doPut();
			ringbell();
			break;

		case CLEAR: 
			if (RUsure()) {
				h = doClear(h);
			}
			break;

		case DEFINEMACRO: 
			if (!noSpace) {
				h = defineMacro(h,offset);
			}
			break;

		case UNDO:
			h = undo(h);
			doRedraw();
			break;

		case REDRAW:
			doRedraw();
			break;


		case GRIDTOGGLE:
#ifdef FINEALIGN
			newMessage("");
			newGrid = indexGrid( indexHit( &gridMenu ) );

			/* if newGrid == -1 set grid to follow alignment */
			if ( newGrid == -1 )
			{
				newGrid = -1 * currAlignment;

				if ( currAlignment < 4 )
				{
					moreMessage("Alignment too small for a useful grid,\n");
					moreMessage("so grid display has been inhibited.");
				}

			}

			/*
			 * If the displayed grid size has changed,
			 * redraw the grid.
			 */
			if (  newGrid != currGridsize &&
			     -newGrid != currGridsize    )
			{
				/* if grid was on, turn it off */
				if ( gridState == GRIDon )
				{
					drawGrid();
				}

				/* set new size and draw new grid */
				currGridsize = newGrid;
				gridState = (newGrid == 0) ?GRIDoff:GRIDon;
				if ( gridState == GRIDon )
				{
					drawGrid();
				}
			}
#else /* FINEALIGN */
			newMessage("Grid displayed is based on the alignment\nsetting.");
			moreMessage("  No grid is displayed when\nalignment is 2.");

			newGrid = indexGrid( indexHit( &gridMenu ) );
			if( newGrid != gridState ) {
				drawGrid();
				gridState = newGrid;
			}
#endif /* FINEALIGN */
			break;

		case ALIGNMENT: 
			newAlign = indexAlign( indexHit( &alignMenu ) );
#ifdef FINEALIGN
			if ( newAlign > 0 )
			{
				currAlignment = newAlign;
			}

			/*
			 * A negative grid size means that the grid
			 * is following the alignment.
			 */
			if ( currGridsize < 0 )
			{
				newMessage("");
				if ( currAlignment < 4 )
				{
					moreMessage("Alignment too small for a useful grid,\n");
					moreMessage("so grid display has been inhibited.");
				}

				newGrid = -1 * currAlignment;

				/*
				 * If the displayed grid size has changed,
				 * redraw the grid.
				 */
				if (  newGrid != currGridsize )
				{
					/* if grid was on, turn it off */
					if( gridState == GRIDon )
					{
						drawGrid();
					}

					/* set new size, draw new grid */
					currGridsize = newGrid;
					gridState = (newGrid == 0) ?
							GRIDoff:GRIDon;
					if ( gridState == GRIDon )
					{
						drawGrid();
					}
				}
			}
#else
			if( newAlign != currAlignment ) {
				if( gridState == GRIDon ) {
					drawGrid();  /* erases present grid */
					currAlignment = newAlign;
					drawGrid();
				} else {
					currAlignment = newAlign;
				}
			}
#endif /* FINEALIGN */
			break;

		case XCIPINFO:
			cursswitch(&hourglass);
			initMessage();
			printSpace();
			printPWD();
			printChgStatus();
			cursSwitch();
			break;

		case XCIPVERS:
			putVersion();
			break;

#ifdef HELPSCALE
		case DISPSCALE:
		    {
			int	newType;
			int	newLabel;
			int	newFactor;
			int	saveB;
			int	r;
			char	numbuff[100];

			newType = indexScale( indexHit( &scaleMenu ) );

			switch (newType)
			{
			case SCALE_NONE:
			case SCALE_LABEL_ONLY:
				currScaleType = newType;
				break;

			case SCALE_FACTOR_ONLY:
			case SCALE_BOTH:
				currScaleType = newType;
				sprintf(numbuff,"%s %d\n",
					"The current scale factor is",
					currScaleFactor );
				newMessage(numbuff);
				break;

			case SCALE_SET_FACTOR:
				saveB = buttonState;
				newMessage("Change scale factor to: ");
				changeButtons(DObuttons);
				sprintf(numbuff,"%d",currScaleFactor);
				r = getMessage(numbuff);
				changeButtons(saveB);

				if( r == 3 || r == 4 )
				{
					newFactor = atoi(numbuff);
					if( newFactor > 0 )
					{
						switch (currScaleType)
						{
						case SCALE_NONE:
							currScaleType =
							 SCALE_FACTOR_ONLY;
							break;
						case SCALE_LABEL_ONLY:
							currScaleType =
							 SCALE_BOTH;
						}
						currScaleFactor = newFactor;
					}
					else
					{
						newMessage("*** scale must be an integer and > 0 ***");
					}
				}
				else
				{
					newMessage("*** change scale aborted ***");
				}
				break;

			case SCALE_SET_LABEL:
				newLabel = indexLabel(indexHit(&labelMenu));

				if ( currLabelType != LABEL_NONE &&
				     newLabel == LABEL_NONE )
				{
					switch ( currScaleType )
					{
					case SCALE_BOTH:
						currScaleType =
							SCALE_FACTOR_ONLY;
						break;
					case SCALE_LABEL_ONLY:
						currScaleType =
							SCALE_NONE;
						break;
					}
				}

				if ( currLabelType == LABEL_NONE &&
				     newLabel != LABEL_NONE )
				{
					switch ( currScaleType )
					{
					case SCALE_NONE:
						currScaleType =
							SCALE_LABEL_ONLY;
						break;
					case SCALE_FACTOR_ONLY:
						currScaleType =
							SCALE_BOTH;
						break;
					}
				}

				currLabelType = newLabel;
				break;
			}

		    }
		    break;
#endif /* HELPSCALE */

#ifdef X11
		case QUIT: 
			if (RUsure()) {
				exit (1);
			}
			break;

#else
		case ORIGWINDOW:
			originalLayer();
			break;
#endif


		}
		break;

	}
	return(h);
}
      

doRedraw() 
{
	register struct thing *t;

	eraseAll();

	t = firstThing;
	if( t != TNULL ) {
		do {
			if (t->type==MACRO) 
				boundingBox(t);
			draw(t, add(drawOffset, scrollOffset)); 
			t = t->next;
#ifndef X11
			sw(1);  /* give up CPU */
#endif
		} while( t != firstThing );
	}
}


#ifdef DMD5620
	void
setdata (p)	/* Setdata routine copied from layersys/boot.c */
	register struct Proc *p;
{
	register struct udata *u = ((struct udata *)p->data);

	u->Drect = p->rect;
	u->Jdisplayp = p->layer;
}
#endif
