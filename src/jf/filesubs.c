/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)filesubs.c	1.1.1.4 88/02/24 13:44:07";

/* includes */
#include "jf.h"

/* defines */
#define BlitSel 0
#define CanonSel 1

#define NEWSIZE	16	/* size of empty font */
#define NEWNCH	16	/* number of characters in empty font */

#ifdef outline
#	undef outline
#endif

/* externals */
extern short oldFontFormat;

/* procedures */
Disp *FindEditInPool ();


outline(r,op)
Rectangle  r;
int op;
{
   if (r.origin.x == r.corner.x)
     if (r.origin.y == r.corner.y) {}
     else segment(&display,r.origin,Pt(r.origin.x,r.corner.y-1),op);
   else
     if (r.origin.y == r.corner.y)
     segment(&display,Pt(r.corner.x-1,r.origin.y),r.origin,op);
     else {
       segment(&display,r.origin,Pt(r.origin.x,r.corner.y-1),op);
       segment(&display,Pt(r.origin.x,r.corner.y-1),sub(r.corner,Pt(1,1)),op);
       segment(&display,sub(r.corner,Pt(1,1)),Pt(r.corner.x-1,r.origin.y),op);
       segment(&display,Pt(r.corner.x-1,r.origin.y),r.origin,op);
     }
}

bord(r)
Rectangle r;
{
  outline(r,F_XOR);
  outline(inset(r,2),F_XOR);
}


/*
 * given a rectangle r (screen coord) and a point p (screen coord),
 * tracks while p moves by Dp and returns r moved by Dp
 *
 * The "pressed" argument was added to support two different kinds
 * of moves.  The first kind of move is represented by "newfont".
 * In that case, there is no selection of the graphic on which the
 * operation is to be performed: as soon as you release the button,
 * the graphic is created right there.  Placing the graphic is performed
 * by depressing a button a second time.
 *
 * In the case of move however, there are three steps to be performed,
 * not just two: the operation, the graphic, and the new position must
 * all be selected.  In this case, combine the last two operations
 * into the same depression (down and back up).   Otherwise, the user
 * must make three seperate depression.
 */
Rectangle
MoveFrame (r, pressed)
	Rectangle r;
	int pressed;		/* bit mask */
{
	Point initp, oldp, p;
	Texture16 *oldcursor;


	wait(MOUSE);

	/* snowflake prompt */
	oldcursor = cursswitch(&movearound);

	p = mouse.xy;
	initp = p;
	oldp = p;

	/* draw the initial border */
	bord(r);

	/*
	 * track the mouse as long as given button is depressed.
	 * Note that button123() returns a bit mask, so pressed
	 * must be in that form, too (i.e. 0x01, 0x02, 0x04).
	 * Thus, this waits only as long as the specified button
	 * (or none) is depressed.
	 */
	while (button123() == pressed) {
		wait(MOUSE);
		p = mouse.xy;
		if (!eqpt(p,oldp)) {
			bord(raddp(r,sub(oldp,initp)));
			bord(raddp(r,sub(p,initp)));
			oldp = p;
		}
	}
	r = raddp(r,sub(p,initp));

	/* this erases the selection rectangle (by XOR) */
	bord(r);

	while (button123()) {
	    wait(CPU);
	    wait(MOUSE);
	}
	cursswitch(oldcursor);
	return(r);
}

EXTERN Point DrectOrigin;

/*
 * This is apparently the head of a list of objects.
 */
Disp *Pool = DNULL;

newfont()	/* create empty font */
{
	Fontdisp *fdp, *newfdp();

	register Font *fp;
	register Fontchar *i;
	Point center;

	/* allocate memory for the new font display graphic structure */
	if ((fdp=newfdp()) == FDNULL)
	    return 0;

	/* allocate memory for the new font */
	fp=(Font *)alloc (sizeof(Font) + NEWNCH*sizeof(Fontchar));
	if (fp == FNULL)
	    return 0;

	/* allocate memory for the new font's bitmap */
	fp->bits=balloc (Rect(0, 0, 32, NEWSIZE));
	if (fp->bits == (Bitmap *)0) {
	    free ((char *)fp);
	    return 0;
	}

	/* clear the bitmap's allocated memory */
	rectf (fp->bits, fp->bits->rect, F_CLR);

	/* initialize font structure */
	fp->n=NEWNCH;			/* currently 16 */
	fp->height=NEWSIZE;		/* currently 16 */
	fp->ascent=NEWSIZE;

	/*
	 * initialize each info array element
	 *
	 * Do NEWNCH+1 elements to insure that the placeholder for
	 * the last info.x is also handled.
	 */
	for (i=fp->info; i <= &fp->info[fp->n]; i++) {
		i->x = 1;		/* ???? */
		i->top = 0;
		i->bottom = 0;
		i->left = 0;
		i->width = 0;
	}

	/*
	 * (the previous clause incremented *past* the last, dummy
	 * info array element)  Go back and insert (for some, unknown
	 * reason) the default width.
	 */
	(--i)->width = NEWSIZE;

	/*
	 * I do not understand why the earler initialization of the
	 * info array elements put their x at one.  At any rate,
	 * this puts the first array element where it should be: 0
	 */
	fp->info[0].x = 0;

	/* now initialize the font display graphic structure */
	fdp->fp=fp;
	fdp->filnam=CNULL;

	/* determine the rectangle that encloses the font display graphic */
	fontsetrect(fdp,Drect);

	/*
	 * the rectangle r must be in screen coordinates.
	 * The subtraction yields the corner point in window
	 * coordinates, and the division yields a point in the
	 * middle of the rectangle.
	 */
        center = div(sub(fdp->r.corner, fdp->r.origin), 2);

	while (!ptinrect (mouse.xy, Drect))
		wait (MOUSE);

	/* have the user move the font display graphic into place */
	fdp->r = MoveFrame (raddp (fdp->r, sub (mouse.xy, center)), 0x00);

	Pool = NewFontPool (fdp, Pool);

	/* display the font display graphic */
	fontdisp (fdp);

	return 1;
}


#define NSTR	128	/* maximum length of font file name */

readfile(astr,devSel)	/* load font from file */
char *astr;
short devSel;
{
	static char *fpath;
	char str[NSTR], *strcat2();
	Point center;
	int nchars;
	Point p;
	Font *fp, *gtfont();
	Fontdisp *fdp, *newfdp();

	if (devSel == BlitSel)
	  fpath = "/usr/dmd/font/";
	else
	  fpath = "/usr/can/font/";

	/*
	 * allocate new font display slot
	 * There's something I don't understand here.  newfdp()
	 * seems only to clear fdp.r.  Between here
	 * and the fontsetrect(), it appears that only the fp
	 * and filenam fields get set.  But then fontsetrect()
	 * calculates fdp.r.corner based on fdp.r.origin.  When
	 * is fdp.r.origin set??? ... unless fdp.r is in window
	 * coordinates???
	 */
	if ((fdp = newfdp ()) == FDNULL)
	    return 0;

	/*
	 * if arguments are passed to jf on the jx command line,
	 * readfile() is called to load those fonts.  Otherwise,
	 * if the user requests that fonts be loaded via the mouse,
	 * he must be prompted for their names.
	 */
	if (astr == CNULL ) {
		/* draw the keyboard input sub-window */
		drstore(rkbd);

		/* prompt.  p is loc. of following user input area */
		p=drstring("File: ",pkbd);

		nchars = kbdstring(str, NSTR - strlen (fpath), p);
	} else {
		strncpy (str, astr, NSTR - strlen (fpath));
		nchars=strlen(str);
	}

	if (nchars <= 0) {
	    drclr(rkbd);
	    return 0;
	}

	cursswitch(&deadmouse);
	if (devSel == BlitSel)
	{
		/*
		 * if str is a relative path name, then
		 * strcat2 must prepend fpath, a pointer, to str, an array,
		 * by shifting the contents of str to the right enough to
		 * make sufficient room for the string pointed to by fpath.
		 */
		if ((fp = getfont (str)) == FNULL)
			if ((str[0] == '/')
			 || ((fp = getfont (strcat2 (fpath, str))) == FNULL)) {
				drstore(rkbd);
				drstring("Font load failed.",pkbd);
				cursswitch(TNULL);
				return 0;
			}

		/*
		 * fp->n is the ascii value of the last character image
		 * of the font (SDPG).  But there are actually 127+1
		 * characters.  Why the SDPG also says that n "gives the
		 * number of character images in the font", I'll never
		 * understand.  The effect of this descrepency is that
		 * one too few characters are display.
		 */
		if (!oldFontFormat)
		    fp->n++;
	}
	else
	{
		if ((fp = gtfont (str)) == FNULL)
			if ((str[0] == '/')
	 		 || ((fp = gtfont (strcat2 (fpath, str))) == FNULL)) {
				drstore(rkbd);
				drstring("Font load failed.",pkbd);
				cursswitch(TNULL);
				return 0;
			}
	}
	cursswitch(TNULL);

	/*
	 * ensure that the keyboard entry area is clean.
	 */
	drstore(rkbd);

	/*
	 * tell the user now that the font is loaded, in case he's doing
	 * something in another window, waiting for the font to com up.
	 */
	p=drstring("Loaded ",pkbd); drstring(str,p);

	if ((fdp->filnam = alloc(strlen(str))) != CNULL)
	    strcpy (fdp->filnam, str);

	fdp->fp=fp;

	/* determine the rectangle that encloses the font display graphic */
	fontsetrect (fdp, Drect);

	/*
	 * The subtraction yields the corner point in window
	 * coordinates, and the division yields a point in the
	 * middle of the rectangle.  Apparently, fdp.r is already
	 * in window coordinates, but the subtraction shouldn't
	 * matter.
	 */
        center = div(sub(fdp->r.corner, fdp->r.origin), 2);

	while (!ptinrect (mouse.xy, Drect))
		wait (MOUSE);

	/*
	 * MoveFrame is the mechanism for tracking the mouse until
	 * the user selects the position where he wants to put the
	 * font display graphic down.  Note that mouse.xy is in
	 * screen coordinates, as well as fdp.r.  Since the mouse
	 * is in the center of the rectangle that will enclose the
	 * font display graphic, the subtract of "center" - in
	 * window coordinates - from mouse.xy will yield the screen
	 * coordinates for the (future) *origin* of that rectangle.
	 * And the rectangle itself, as defined by fontsetsect is
	 * converted to screen coordinates by the rectangle addition.
	 */
	fdp->r = MoveFrame (raddp(fdp->r, sub (mouse.xy, center)), 0x00);

	/* enque the new font on the list of display objects */
	Pool = NewFontPool (fdp, Pool);

	fontdisp(fdp);

	return 1;
}

writefile(devSel)
	short devSel;		/* write font to file */
{
	Fontdisp *fdp=FDNULL;
	Editdisp *edp;		/* pointer to open char edit cells? */
	Font *fp;
	Fontchar *i;
	Point p;
	Rectangle r, charect(), rsupport();
	Sfont *sf,*reconvert();
	int c, nchars, fouch(), samename=0;
	char str[NSTR],*pt,*rindex();

	cursswitch(&target);

	if (buttons(DOWN) == 3)
	    mousetrack();	/* set mtk */
	else {
	    buttons(UP);	/* ensures that all buttons are released */
	    cursswitch(TNULL);
	    return 0;
	}

	buttons(UP);		/* ensures that all buttons are released */
	cursswitch(TNULL);

	/* if the mouse isn't positioned over any object, finished */
	if (mtk.disp == DNULL)
	    return 0;

	/* you can't write a character edit cell */
	if (mtk.disp->Class != FontClass)
	    return 0;

	fdp = mtk.disp->Disp.TopFont;
	fp=fdp->fp;

	/*
	 * traverse the list of open character edit cells, presumably
	 * closing them up, prior to the write.
	 */
	for (edp=fdp->edp; edp!=EDNULL; edp=edp->edp) {
		c=edp->c;
		i=fp->info+c;	/* i points to info element for editted char */
		r = rsupport(fp->bits, charect(fp, c), (Bitmap *)0);
		i->top=r.origin.y;
		i->bottom=r.corner.y;
	}

	/*
	 * prompt the user for the name of the font file to write
	 */
	drstore(rkbd);

	if (fdp->filnam != CNULL) {
		p=drstring("File (",pkbd);
		p=drstring(fdp->filnam,p);
		p=drstring(") : ",p);
	} else {
		p=drstring("File: ",pkbd);
	}

	nchars=kbdstring(str,NSTR,p);
	if (nchars <= 0) {
		if (fdp->filnam == CNULL) {
		    drclr(rkbd);
		    return 0;
		}
		samename=1;
		strcpy(str,fdp->filnam);
	}

	cursswitch(&deadmouse);

	/* write the font */
	if (devSel == BlitSel) {
		filep=fopen(str,"w");
		c=-1;
		if (filep != (FILE *)0) {
		    c = myoutfont (fdp->fp, fouch);
		    fclose(filep);
		}
	}
	else {	/* canon font */
	/*
	 *
	filep=fopen(str,"w"); c=-1;
	if (filep != (FILE *)0) {
	 *
	 */
		sf = reconvert(fdp->fp);
		pt = rindex(str,'/');
		if (pt == 0) 
			pt = str - 1;
		sf->name = (*++pt)<<8;
		sf->name |= *++pt;
		c=putfont(sf,str);
	/*	fclose(filep);
	 *
	}
	 *
	 */
	}	/* end canon font */

	/* report the results */
	drstore(rkbd);

	if (c<0) {
		drstring("Font write failed.",pkbd);
		return 0;
	} else {
		p=drstring("Wrote ",pkbd);
		drstring(str,p);
		if (!samename) {
			/*
			 * if a new name was specified, replace
			 * the filnam stored in the font with
			 * the new one.
			 */
			if (fdp->filnam != CNULL)
			    free(fdp->filnam);
			fdp->filnam=alloc(strlen(str));
			if (fdp->filnam != CNULL)
			    strcpy(fdp->filnam,str);
		}
		return 1;
	}
}

fouch(c)		/* write single character to font file */
int c;
{
	return(putc(c,filep));
}

moveobject()	/* move object */
{
	Fontdisp *fdp;
	Editdisp *edp;
	Rectangle DispRect(), r,newr;


	cursswitch(&target);
	if (buttons(DOWN) == 3)
	    mousetrack();
	else {
	    buttons(UP);
	    cursswitch(TNULL);
	    return;
	}
	if (mtk.disp == DNULL)
	    return 0;
	if (mtk.disp->Class == FontClass) {
		fdp = mtk.disp->Disp.TopFont;
		r = fdp->r;
		newr = MoveFrame(r, 0x01);
	} else {
		edp = mtk.disp->Disp.TopEdit;
		r = edp->r;
		newr = MoveFrame(r, 0x01);
	}
	if (eqrect(r,newr))
	    mtk.disp->Redraw = true;
	else
	    MarkRedraw(mtk.disp);
	drclr(DispRect(mtk.disp));
	if (mtk.disp->Class == FontClass)
	    fdp->r = newr;
	else
	    edp->r = newr;
	Pool = MoveToTopOfPool(mtk.disp,Pool);
	Redraw();
}

sqdfont(icode)	/* squeeze or delete font */
int icode;
{
	Fontdisp *fdp=FDNULL;
	Editdisp *edp;

	if (mtk.disp == DNULL)
	    return 0;

	if (mtk.disp->Class != FontClass)
	    return 0;

	fdp = mtk.disp->Disp.TopFont;

	for (edp=fdp->edp; edp != EDNULL; edp=edp->edp) {
		if (edp->mask) {
			bfree(edp->mask);
			edp->mask = BNULL;
		}
		fdispflp(edp->fdp,edp->c);
		Pool = RemoveFromPool(FindEditInPool(edp,Pool),Pool);
		edp->fdp=FDNULL;
	}
	fdp->edp=EDNULL;

	if (icode) {
		ffree(fdp->fp);
		fdp->fp=FNULL;
		if (fdp->filnam != CNULL)
		    free(fdp->filnam);
		Pool = RemoveFromPool(mtk.disp,Pool);
	} else {
		icode = fontprune(fdp->fp);
		Resize();
	}
	Redraw();
	return icode;
}

writeicon()		/* write Texture declaration */
{
	Editdisp *edp;
	Font *fp;
	Fontchar *ich;
	char str[NSTR];
	register Bitmap *tb;
	register i;
	char hexbuf [5];

	/*
	 * allocate a 16x16 bitmap.  Larger characters will
	 * automatically be clipped appropriately
	 */
	if ((tb = balloc(Rect(0,0,16,16))) == 0)
	    return 0;

	/* get user selection of character to be iconized */
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

	/*
	 * if the user didn't select an edit cell, we'll
	 * assume he aborted.
	 */
	if (mtk.disp == DNULL) {
	    bfree(tb);
	    return 0;
	}
	if (mtk.disp->Class != EditClass) {
	    bfree(tb);
	    return 0;
	}

	edp = mtk.disp->Disp.TopEdit;
	fp = edp->fdp->fp;
	ich = fp->info + edp->c;

	/* get the name of the new texture */
	drstore(rkbd);
	if (kbdstring(str,NSTR,drstring("Texture: ",pkbd)) <= 0) {
		bfree(tb);
		return 0;
	}

	cursswitch(&deadmouse);
	filep=fopen(str,"w");
	if (filep != (FILE *)0) {
		/* copy the character into the new bitmap */
		bitblt(fp->bits, Rect(ich->x, 0, (ich+1)->x, fp->height),
		    tb, Pt(0,0), F_STORE);

		/* start building the texture file */
		fprintf(filep,"Texture16 %s = {\n", str);

		/*
		 * The icon bitmap is only 1 short wide.  Print out each
		 * one of the 16 shorts.  If the machine's wordsize is
		 * greater than that, left adjust the low-order 16 bits.
		 */
		for (i=0; i<16; i++) {
			stos (tb->base[i]>>(WORDSIZE-16), hexbuf);
			fprintf(filep, "%c0x%s,", " \t"[(i&7) == 0], hexbuf);
			if (i == 7)
			    fprintf(filep, "\n");
		}
		fprintf(filep, "\n};\n");
		fclose(filep);
		drstore(rkbd);
		drstring(str,drstring("Wrote ",pkbd));
		bfree(tb);
		return 1;
	}
	else {
		drstore(rkbd);
		drstring("Texture write failed.",pkbd);
		bfree(tb);
		return 0;
	}
}


/* short to string */
stos (number, bufptr)
	short number;
	char *bufptr;
{
	register short hexdig;
	register short digitp = 4;

	bufptr[digitp] = '\0';

	while (--digitp >= 0) {
		hexdig = number & 0x000f;
		bufptr[digitp] = hexdig > 9? hexdig + ('A'-10) : hexdig + '0';

		number >>= 4;
	}
}
