/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)spaceman.c	1.1.1.2 88/02/10 17:14:04";

/* includes */
#include "jf.h"

/* procedures */


Disp *NewFontPool (FontDisp, Next)
    Fontdisp *FontDisp;
    Disp *Next;
{
	Disp *result;

	result = (Disp *)alloc(sizeof(Disp));

	if (result==DNULL)
	    return(Next);

	result->Class = FontClass;
	result->Disp.TopFont = FontDisp;
	result->Redraw = false;
	result->Next = Next;

	return(result);
}

Disp *NewEditPool (EditDisp, Next)
    Editdisp *EditDisp;
    Disp *Next;
{
	Disp *result;

	result = (Disp *)alloc(sizeof(Disp));

	if (result==DNULL)
	    return(Next);

	result->Class = EditClass;
	result->Disp.TopEdit = EditDisp;
	result->Redraw = false;
	result->Next = Next;

	return(result);
}

Rectangle DispRect(Item)
Disp *Item;
{
	Rectangle r;
	if (Item->Class == FontClass) {
		r = Item->Disp.TopFont->r;
		r.origin = sub(r.origin,Pt(WBORD,WBORD));
	} else r = Item->Disp.TopEdit->r;
	return(r);
}

Disp *ExtractFromPool(Item,Pool)
Disp *Item, *Pool;
{
	if (Pool==DNULL) return(DNULL);
	if (Item==Pool) return(Item->Next);
	Pool->Next = ExtractFromPool(Item,Pool->Next);
	return(Pool);
}

Disp *RemoveFromPool(Item,Pool)
Disp *Item, *Pool;
{
	Disp *result;
	MarkRedraw(Item);
	drclr(DispRect(Item));
	result = ExtractFromPool(Item,Pool);
	free(Item);
	return(result);
}

Disp *MoveToTopOfPool(Item,Pool)
Disp *Item, *Pool;
{
	Disp *Next;
	Next = ExtractFromPool(Item,Pool);
	Item->Next = Next;
	return(Item);
}

Disp *FindFontInPool(fdp,Pool)
Fontdisp *fdp; Disp *Pool;
{
	if (Pool==DNULL) return(DNULL);
	if (Pool->Class == EditClass) return(FindFontInPool(fdp,Pool->Next));
	if (Pool->Disp.TopFont == fdp) return(Pool);
	return(FindFontInPool(fdp,Pool->Next));
}

Disp *FindEditInPool(edp,Pool)
Editdisp *edp; Disp *Pool;
{
	if (Pool==DNULL) return(DNULL);
	if (Pool->Class == FontClass) return(FindEditInPool(edp,Pool->Next));
	if (Pool->Disp.TopEdit == edp) return(Pool);
	return(FindEditInPool(edp,Pool->Next));
}

InvertPoolChars(fdp,List)
Fontdisp *fdp;
Disp *List;
{
	Editdisp *edp;

	if (List == DNULL) return;
	if (List->Class == EditClass) {
		edp = List->Disp.TopEdit;
		if (edp->fdp == fdp) fdispflp(fdp,edp->c);
	}
	InvertPoolChars(fdp,List->Next);
}

RedrawPool(List)
Disp *List;
{
	if (List == DNULL) return;
	RedrawPool(List->Next);
	if (List->Redraw) {
		if (List->Class == FontClass) {
			fontdisp(List->Disp.TopFont);
			InvertPoolChars(List->Disp.TopFont,Pool);
		} else editdisp(List->Disp.TopEdit);
		List->Redraw = false;
	}
}

#ifdef outline
#undef outline
#endif
extern outline ();
Redraw()
{
	RedrawPool(Pool);
	outline(inset(Drect,-3),F_STORE);
	outline(inset(Drect,-2),F_STORE);
	outline(inset(Drect,-1),F_STORE);
}

int LocalTop(Item)
Disp *Item;
{
	Disp *Scan;
	Scan = Pool;
	while (Scan != Item) {
		if (Scan == DNULL) return(1);
		if (rectXrect(DispRect(Item),DispRect(Scan))) return(0);
		Scan = Scan->Next;
	}
	return(1);
}

MarkRedraw(Item)
Disp *Item;
{
	Disp *Scan;
	Item -> Redraw = true;
	Scan = Pool;
	while (Scan != DNULL) {
		if (Item != Scan) {
			if (rectXrect(DispRect(Item),DispRect(Scan))) {
				if (Scan->Redraw == false) MarkRedraw(Scan);
			}
		}
		Scan = Scan->Next;
	}
}

EraseFont(Item)
Disp *Item;
{
	Editdisp *edp; Disp *EditItem;
	drclr(DispRect(Item));
	MarkRedraw(Item);
	edp = Item->Disp.TopFont->edp;
	while (edp != EDNULL) {
		EditItem = FindEditInPool(edp,Pool);
		drclr(DispRect(EditItem));
		MarkRedraw(EditItem);
		edp = edp ->edp;
	}
}

Resize()
{
	Rectangle r;
	int i, j;

	/*
	 * This must define a subwindow for keyboard input: the
	 * points of the rectangle are the same as for the window as a
	 * whole - except for the origins y coor.  pkbd then is probably
	 * the equivalent of the origin of the "Drect" for this pseudo-
	 * window.
	 */
	rkbd = Drect;
	rkbd.origin.y = Drect.corner.y - defont.height - 2*WBORD;
	pkbd=add(rkbd.origin,Pt(WBORD,WBORD));

	r = Drect;
	r.corner.y = rkbd.origin.y;

	for (i=0; i<MFDISP; i++)
	    if (fdisp[i].fp)
		fontsetrect(&fdisp[i],r);

	/*
	 * before editsetrect()ing all the edit cells after a reshape
	 * (presumably), reset the size???
	 * This looks like a quick and dirty - and essentially bogus -
	 * fix.
	 */
	for (j=0; j<MEDISP; j++)
	    edisp[j].size=EDSIZE;

	for (i=0; i<MEDISP; i++)
	    if (edisp[i].fdp)
		editsetrect(&edisp[i]);

	return;
}

mousetrack()		/* see what the mouse is up to */
{
	int i;
	Point edisphit();
	Disp *scan;

	mtk.disp=DNULL;

	/*
	 * this presumably scans through the list of display objects
	 * (or graphics, as I have also been calling them), and returns
	 * the one that the mouse is positioned on, if any.
	 */
	for (scan=Pool; scan!=DNULL; scan=scan->Next) {
		if (scan->Class==FontClass) {
			/* is the mouse positioned over this font? */
			mtk.c = fdisphit(scan->Disp.TopFont, mouse.xy);

			if (mtk.c >= 0) {
			    mtk.disp=scan;
			    return;
			}
		} else {
			mtk.pxl = edisphit(scan->Disp.TopEdit, mouse.xy);

			if (mtk.pxl.x >= 0) {
			    mtk.disp=scan;
			    return;
			}
		}
	}

	return;
}

Rectangle nullrect = {0,0,0,0};

Fontdisp *
newfdp()	/* allocate new font display slot */
{
	register int i;

	for (i=0; i<MFDISP; i++) {
		if (fdisp[i].fp == FNULL) {
			fdisp[i].r = nullrect;
			return &fdisp[i];
		}
	}
	return FDNULL;
}

Editdisp *
newedp()	/* allocate new edit display slot */
{
	register int i;

	for (i=0; i < MEDISP; i++) {
		if (edisp[i].fdp == FDNULL) {
			edisp[i].r = nullrect;
			return &edisp[i];
		}
	}
	return EDNULL;
}
