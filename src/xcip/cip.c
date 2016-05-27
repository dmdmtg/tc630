#include "cip.h"
#include "version.h"
#ifndef X11

#include <setup.h>

#ifdef DMD630
#include <object.h>
#endif

#ifdef DMD5620
#define jinit
#define dellayer
#define newlayer
#define mpxnewwind
#include <pandora.h>
#endif

#endif /* X11 */

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

#ifdef X11
extern NMenu alignMenu;
extern NMenu spacingMenu;
extern NMenu justMenu;
extern NMenu psMenu;
#else  /* X11 */
extern Tmenu alignMenu;
extern Tmenu spacingMenu;
extern Tmenu justMenu;
extern Tmenu psMenu;
#endif /* X11 */

char *Pgm_name;
char *Jpic_pgm;

#define Xbut1 ((Xbut+(Xmax-Xbut)/6)+4)
#define Xbut2 (((Xmax+Xbut)>>1)-4)
#define Xbut3 ((Xmax-(Xmax-Xbut)/6)-8)
#define Ybut123 (Ybut+((ButSize*3)>>2))
#define _GETENV		9

#define BUTDEPTH	4	/* Bit depth for 3-D button. */

Point jchar();
extern Point PointCurr;


#ifdef DMDTERM
  short Xmin;		/* Left edge of frame. */
  short Ymin;		/* Top edge of frame.  */
#endif

#ifdef X11
  Rectangle clip_rect;	/* clipping rectangle for drawing */
#endif /* X11 */

char *	MessagePnt;
char 	MessageBuf[1024] = "";

int StartUp;

Point drawOffset;	/* Offset into drawing frame. */
Point scrollOffset;	/* Offset into drawing frame from scrolling. */

int videoState = WHITEfield;
Rectangle brushes[NUMBR+2];
int currentBrush = {-1};

int copyFlag = 0;
int thingSelected = 0;
int editDepth = 0;
int buttonState = 0;
int anyChgs = 0;
short fontBells = 1;
fontBlk *fonts = {(fontBlk *)NULL};
fontBlk *defFont;
int textOutName = INITtextOutName;

int left_hand;

int gridState = GRIDoff;

#ifdef FINEALIGN
/*
 * If the currGridsize variable is < 0, then the grid value is set
 * to follow the alignment value.
 */
short currGridsize = 0;
#endif
short currAlignment = 8;
short currPS =   10;
short currFont = ROMAN;
short currJust = CENTER;
short currLineType = SOLID;
short currBoxType = SOLID;
short currLineArrows = 0;
short currSplineArrows = 0;
short currSpacing = 0;

struct thing * firstThing =  TNULL ;

char *but1text[numButtonStates] = {
	"select",	"",		"select",	"select",
	"", 		"",		"abort",	"select",
	"select",	"no",		"center",	"OK",
	"select",	"abort"
};
char *but2text[numButtonStates] = {
	"move all",	"",		"draw",		"edit",
	"draw",		"",		"draw box",	"copy",	
	"move",		"no",		"move box",	"OK",
	"draw",		"abort"
};
char *but3text[numButtonStates] = {
	"main menu",	"menu",		"",		"menu",	
	"end spline",	"",		"",		"menu",
	"menu",		"yes",		"",		"OK",
	"glbl menu",	"do"
};

extern insertFont ();
extern long spaceLeft();
#ifdef X11
extern Cursor hourglass;
extern Cursor textCursor;
#else /* X11 */
extern Texture16 hourglass;
extern Texture16 textCursor;
#endif /* X11 */
short noSpace;		/* Indicates out of initial amount of memory */
char *envfont;
char *strcat();

#ifdef X11

#define HEIGHT  10.5
#define WIDTH   8.5
#define MM_INCH 25.4
#define MINBORDER       20

jerqsizehints()
{
        double pix_inch;
        int width, height;

#if CHOOSESIZEBYINCHES
        pix_inch = (XDisplayHeight(dpy, 0) * MM_INCH)/XDisplayHeightMM(dpy, 0);
        width = pix_inch * WIDTH;
        height = pix_inch * HEIGHT;
#else
	width = DefaultWidth;
	height = DefaultHeight;
#endif
        if (width > (XDisplayWidth(dpy, 0) - MINBORDER))
                width = XDisplayWidth(dpy, 0) - MINBORDER;
        if (height > XDisplayHeight(dpy, 0) - MINBORDER)
                height = XDisplayHeight(dpy, 0) - MINBORDER;
        setsizehints (width, height, 1);
}

typedef int (*Ifunc)();
Ifunc SaveHandler;

int handler(disp, err)
Display *disp;
XErrorEvent *err;
{
	fprintf(stderr,"In error handler\n");
	(*SaveHandler)(disp, err);
	return(0);
}

#endif X11


main(argc,argv)
    int    argc;
    char * argv[];
{	
    struct thing * currentThing = TNULL;

    Pgm_name = &argv[0][strlen(argv[0])];
    while(--Pgm_name > &argv[0][0]) {
	if (*Pgm_name == '/') {
	    Pgm_name++;
	    break;
	}
    }

#ifdef DMD630
    /* Save xcip in cache always. */
    cache( Pgm_name, A_NO_SHOW );
#endif

    if (Pgm_name[strlen(Pgm_name)-2] == '.')
	/* cut off ".m" */
	Pgm_name[strlen(Pgm_name)-2] = '\0';

    if (Pgm_name[0] == 'x')
	Jpic_pgm = "xjpic";
    else
	Jpic_pgm = "jpic";
	

#ifdef X11
    if ( argc > 1 && argv[1][0]=='-' && argv[1][1]=='V' ) {
	printf( "%s version %s\n", Pgm_name, PGM_VERSION );
	exit(0);
    }

    request( KBD | MOUSE );
    initdisplay(argc, argv);  /* initialize Kapilow's X11 emulator package */
    initGraphics();           /* initialize cursors, textures & bitmaps. */

#else /* X11 */

#ifdef DMD5620
    jinit();
#endif

    request( KBD | MOUSE | RCV | SEND );

    /* Get the font directory from param list. */
    envfont = argv[1];		

    P->state |= RESHAPED;		/* Indicate frames not drawn yet. */

#endif /* X11 */

    noSpace = 0;
    currentBrush = -1;
    buttonState = INITbuttons;
    thingSelected = 0;
    copyFlag = 0;
    StartUp = 1;


    /* The values to initialize prevhit with depends on what
     * which menu index xcip thinks is the default value for
     * the selection made from that menu.  The prevhit numbers have
     * to be consistent with the menu indexes (from cip.h or
     * menus.c) into the menu definitions (in menus.c) that
     * correspond to the initial values of these variables
     * (defined here, in cip.c):
     *    for alignMenu,   see currAlignment
     *    for psMenu,      see currPS
     *    for justMenu,    see currJust
     *    for spacingMenu, see currSpacing
     */
#ifdef FINEALIGN
    alignMenu.prevhit = 7;
    alignMenu.prevtop = 0;
#else
    alignMenu.prevhit = 1;
    alignMenu.prevtop = 0;
#endif /* FINEALIGN */

    spacingMenu.prevhit = 4;
    spacingMenu.prevtop = 0;
    justMenu.prevhit = 1;
    justMenu.prevtop = 0;
    psMenu.prevhit = 8;
    psMenu.prevtop = 0;

    initFontList ();

    for (;;) {
        currentThing = doMouseButtons( currentThing, Pt(0,0) );
#ifdef X11
	/* for some mysterous reason, must do nap here instead of
	   wait(mouse) in doMouseButtons() for correct behavior. */
	/*  nap(2);  */
#endif
    }
}


#ifdef DMDTERM

Rectangle origRect = {8,8,200,100};	/* Holds original layer size,
					   initialize to upper left.  */
originalLayer()
{	
#ifdef DMD630
	reshape( origRect );
#endif

#ifdef DMD5620
	register struct Layer * l;
	register struct Proc *  p = P;

	dellayer (p->layer);		/* Delete the current layer */
	p->state &= ~MOVED;
	p->state |= RESHAPED;
	l = newlayer(origRect);		/* and create a new one. */
	if (l == 0) {		/* If not enough room, squish the */
				/* layer to tell user no room. */
		origRect.corner = add (origRect.origin, Pt (100, 50));
		l = newlayer (origRect);
		if (l == 0) {
			exit ();	/* Too bad!! */
		}
	}
	/* Set up new process table. */
	p->layer = l;
	p->rect = inset (origRect, 2);
	setdata (p);
	mpxnewwind (p, C_RESHAPE);
#endif
}


resizeLayer()
{
  Rectangle r;
  register struct Layer *l;
  register struct Proc *p =P;
  long	temp;

  origRect = p->layer->rect;
  
  /* Position opened window based on position of close window. */
  if( origRect.origin.x < 10 ) {
	Xmin = LEFTMOST;
  } else if( origRect.corner.x > (RIGHTX - 10) ) {
	Xmin = RIGHTMOST;
  } else {
	/* Take an average postion */
  	temp = (origRect.origin.x + origRect.corner.x) >> 1;
  	Xmin = ( (temp * (RIGHTMOST-LEFTMOST) ) / RIGHTX ) + LEFTMOST;
  }

  if( origRect.origin.y < 10 ) {
	Ymin = TOPMOST;
  } else if( origRect.corner.y > (BOTTOMY - 10) ) {
	Ymin = BOTTOMMOST;
  } else {
	/* Take an average postion */
  	temp = (origRect.origin.y + origRect.corner.y) >> 1;
  	Ymin = ( (temp * (BOTTOMMOST-TOPMOST) ) / BOTTOMY ) + TOPMOST;
  }

  r.origin.x = Xmin;
  r.origin.y = Ymin;
  r.corner.x = Xmax;
  r.corner.y = Ymax;
  r = inset(r,-BORDER);	/* Make the window bigger */

  drawOffset.x = Xmin;
  drawOffset.y = YPIC;

  brushInit();	/* Init outline of brush boxes. */
  
#ifdef DMD630
  reshape( r );
#endif

#ifdef DMD5620
  dellayer (p->layer);	/* Delete the current layer */
  p->state &= ~MOVED;
  p->state |= RESHAPED;
  l = newlayer (r);		/* and create a new one. */
  if (l == 0) {		/* If not enough room, squish the */
			/* the layer to tell user no room. */
    r.origin = origRect.origin;
    r.corner = add (origRect.origin, Pt (100,50));
    l = newlayer (r);
    if (l == 0) {
      exit ();	/* No more space at all */
    }
  }
  /* Set up new process table. */
  p->layer = l;
  p->rect = inset (r, 2);
  setdata (p);
  mpxnewwind (p, C_RESHAPE);
#endif
  if (CORRECTSIZE) {
    redrawLayer();
  }
}

#endif /* DMDTERM */


redrawLayer()
{
  int t;
  char *tempStr;

  cursinhibit();

#ifdef X11
  brushInit();	/* Init outline of brush boxes. */
#endif 

  drawFrame(); 	/* display drawing area and brushes */

#ifdef X11
  drawOffset.x = Xmin;
  drawOffset.y = YPIC;
  clip_rect = Rect(brushes[PIC].origin.x-2*LW,
		   brushes[PIC].origin.y-44,
		   brushes[PIC].corner.x-2*LW,
		   brushes[PIC].corner.y-43);
#endif /* X11 */

  doRedraw();

  if (videoState == BLACKfield) {
#ifdef X11
    rectf(&display, Drect, F_XOR);
#else /* X11 */
    rectf(&display, Jrect, F_XOR);
#endif /* X11 */
  }

  if (editDepth) {
    drawEDbutton (editDepth);
  }

  if (currentBrush > -1) {
    t = currentBrush;
    currentBrush = -1;
    changeBrush(t);
  }

  cursallow();
  P->state &= ~RESHAPED;
  if (StartUp) {
    putVersion();
    StartUp = 0;
  } else {
    PointCurr.x = MSGXMIN;
    PointCurr.y = MSGYMIN;

    /* Redisplay the message area. */
    MessagePnt = MessageBuf;
    tempStr = alloc(strlen(MessageBuf)+1);
    strcpy(tempStr,MessageBuf);
    moreMessage(tempStr);
    free(tempStr);
  }
}


brushInit()
{
  register int x; 
  register int i;

  x=Xmin;
  for (i=0; i<NUMBR; i++) {
    brushes[i].origin.x = x;
    x += DXBR;
    brushes[i].corner.x = x-LW;
    brushes[i].origin.y = Ymin;
    brushes[i].corner.y = Ymin+YBR;
  }
  brushes[PIC].origin.x = Xmin+LW;
  brushes[PIC].origin.y = YPIC+LW;
  brushes[PIC].corner.x = Xmax-LW;
  brushes[PIC].corner.y = YBOT-LW;
  brushes[ED].origin.x  = XeditD;
  brushes[ED].origin.y  = Ybut;
  brushes[ED].corner.x  = Xbut-LW;
  brushes[ED].corner.y  = Ymax;
}


drawFrame()
{
  register int i;

  for (i=0; i<NUMBR; i++) {
    drawBrush(i);
  }

  for( i=1; i<=LW; i++ )
    box( Rect(brushes[PIC].origin.x-i,brushes[PIC].origin.y-i,
	      brushes[PIC].corner.x+i-1,brushes[PIC].corner.y+i) );

  /* Init message frame */
  for (i=1; i <=LW; i++)
      box( Rect(Xmin+i-1,Ybut+i-1,XeditD-LW-i,Ymax-i) );

  Buttons();
}	

drawBrush(i) 
register int i;
{
  register int	r;  
  int	hb = BUTDEPTH >> 1;  /* Divide by 2. */
  Point m; 
  Point p[6];

  drawButtonPicture( brushes[i] );

  r = (YBR*3)>>3;
  m = div( add( brushes[i].origin, sub(brushes[i].corner,Pt(hb,hb)) ), 2);
  switch (i) {
    case CIRCLE: {
      circle(&display, m,r,F_XOR);
      break;
    }
    case BOX: {
      box(Rect(m.x-r,m.y-r,m.x+r,m.y+r)); 
      break;
    }
    case ELLIPSE: {
      Ellipse(m,(r<<1),DXBR-(8*LW));
      break;
    }
    case LINE: {
      segment(&display, Pt(m.x-r,m.y),Pt(m.x+r,m.y),F_XOR);
      break;
    }
    case ARC: {
      arc(&display, Pt(m.x,m.y+r),Pt(m.x+r,m.y),Pt(m.x-r,m.y),F_XOR);
      break;
    }
    case SPLINE: {
      p[1] = sub(m,Pt(r,r));
      p[2] = add(m,Pt(r>>1,-r));
      p[3] = sub(m,Pt(r>>1,-r));
      p[4] = add(m,Pt(r,r));
      spline(Pt(0,0),p,5);
      break;
    }
    case TEXT: {
      centeredText(sub(m,Pt(0,10)),"Text");
      break;
    }
  }
}


    Rectangle *
Select(t, m, offset)
    struct thing *t;
    Point m;
    Point offset;
{
    register int i;

    for( i=0; ((i<=ED+1)&&(!ptinrect(m,brushes[i]))); i++ )
        ;

    if( i == PIC ) {

	/* Selected inside picture drawing area. */
        return(&brushes[PIC]);    /* temporary */

    } else if( i == ED ) {

	/* Selected EDIT button. */
        if( editDepth > 0 ) {
            drawEDbutton(editDepth);	/* Undraw edit button */
            if( (--editDepth)!=0 ) {
	        drawEDbutton(editDepth);
            }
        }

        changeBrush(-1);	
        if (thingSelected) {
            drawSelectionLines (t, offset);
            thingSelected = 0;
        }

        changeButtons(INITbuttons);
        return( (Rectangle *) NULL);


    } else if( i < PIC ) {

	/* Selected one of the brush types at top. */
	if( currentBrush != i ) {
	    /* Change to selected brush if not already selected,
	       otherwise, de-select it (by falling to following code). */
            changeBrush(i);
            return(&brushes[i]);
	}
    }

    /* Selected outside everything - make no selection. */
    changeBrush(-1);	
    changeButtons(INITbuttons);
    return( (Rectangle *) NULL);
}

    
Buttons()
{	
  register int i;

#ifdef DMDTERM
  /* Determine if mouse if left of right handed. */
# ifdef DMD630
   left_hand = setupval( S_PREF, 6 );
# endif

# ifdef DMD5620
   left_hand = VALMOUSE;
# endif
#endif

  for (i=1; i <= LW; i++)
      box( Rect(Xbut+i-1,Ybut+i-1,Xmax-i,Ymax-i) );

  centeredText(Pt(Xbut2,Ytext), "Mouse Buttons Usage");
  writeButtonLabels(buttonState);
}
  
labeledButton(p,s) 
  Point p; 
  register char *s;
{
  register int w;
  register int bh;

  bh = butHt;

#ifdef X11
  w = (strwidth(&defont,s)+8)>>1;
#else /* X11 */
  w = (jstrwidth(s)+8)>>1;
#endif /* X11 */
  if (w<=4) {
    w=bh;
  }
  centeredText(add(p,Pt(0,(bh>>1)-10)),s);
  box(Rpt(sub(p,Pt(w,bh)),add(p,Pt(w,bh))));
}

  Point
jString(s)
  register char *s;
{
  PointCurr=string(&defont,s,&display,PointCurr,F_XOR);
  return PointCurr;
}

  Point 
jchar(c) 
  register char c;
{
  char s[2];

  s[0]=c; s[1] = '\0';
  return(jString(s));
}
  
drawSelectionLines(t,p) 
  register struct thing *t; 
  Point p; 
{
  Point p1;

  if (t!=TNULL) {
    if (t->type==SPLINE) {
      drawZigZag(p,t->otherValues.spline.plist,
        t->otherValues.spline.used);
    }
    else if (t->type==ARC) {
      p1 = add(t->origin,p);
      xsegment(p1,add(p,t->otherValues.arc.start));
      xsegment(p1,add(p,t->otherValues.arc.end));
    }	
  }
}


#ifdef DMDTERM
  Bitmap gridBm = { (Word *)&grid, 1, 0, 0, 16, 16, 0 };
#endif 

#ifdef FINEALIGN
#ifdef DMDTERM
  Bitmap fineGridBm = { (Word *)&fineGrid, 1, 0, 0, 16, 16, 0 };
#endif /* DMDTERM */
#endif /* FINEALIGN */

drawGrid()
{	
#ifdef X11
#ifdef FINEALIGN
	register int	xpos;
	register int	ypos;
	int 		bmapwid;
	int		tmpGridsize;
	Rectangle	insetRect;
	Rectangle 	inLayer, in_fineGridAnyBm;
	Bitmap *	fineGridAnyBmP;
	unsigned short x_fineGrid_bits[16];

	/*
	 * insetRect is a rectangle that defines the coordinates
	 * of the drawing area in &display (was P->layer.)
	 */ 
	insetRect = inset(Rect(Xmin,YPIC,Xmax,YBOT), LW );

	/* cursinhibit(); */

	tmpGridsize = currGridsize < 0 ? -currGridsize : currGridsize;

	switch( tmpGridsize )
	{
	case 1:
	case 2:
	case 3:
		break;

	/* case 1: */
		/* For a tmpGridsize of 1, every single pixel on
		 * the display gets turned on, so replacing the
		 * normal algorithm with a call to rectf makes for
		 * a very fast operation.
		 */
	/*	rectf( &display, insetRect, F_XOR );
	 *	break;
	 */

	/* case 2: */
	case 4:
	case 8:
	case 16:
		/* For grid sizes which are a power of two,
		 * a texture can be generated and painted on the
		 * screen.
		 */
		/*
		 * Pixels are painted inside the texture to line up with
		 * the point Pt(Xmin,YPIC).
		 * Set the bits of x_fineGrid_bits that must be on.
		 */
		/* fineGrid = T_white; */
		
		for( ypos = 0 ; ypos < 16; ypos++ )
		{
			x_fineGrid_bits[ypos] = 0;
		}
		for( xpos = (Xmin & 0xf) % tmpGridsize;
		     xpos < 16; xpos += tmpGridsize )
		{
			for( ypos = (YPIC & 0xf) % tmpGridsize;
			     ypos < 16; ypos += tmpGridsize )
				/*	point(&fineGridBm,
					Pt(xpos,ypos), F_STORE); */
			{
				/* set bit xpos in row ypos. */
				x_fineGrid_bits[ypos] |= 0x8000 >> xpos;
			}
		}
		/* all bits set, initialize Texture from bits. */
		fineGrid = ToTexture((short *)x_fineGrid_bits );

		/* Then the texture is painted on the insetRect
		 * inside the layer.
		 */
		/* texture16( P->layer, insetRect, &grid, F_XOR ); */
		texture( &display, insetRect, &fineGrid, F_XOR );
		break;

	/* case 3: */
	case 5:
	case 6:
	case 7:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	/*
	 * For grid sizes which are not a power of two,
	 * a bitmap is generated which is 16*gridsize pixels
	 * wide. Pixels for the appropiate grid size are painted
	 * onto the bitmap to align with Pt(Xmin,YPIC).
	 *
	 * The this bitmap is bitblt'ed all over the screen,
	 * one section at a time.
	 */
	{

		/* Allocate Bitmap. */
		bmapwid = 16*tmpGridsize;
		fineGridAnyBmP = balloc(Rect(0, 0, bmapwid, bmapwid));

		if( fineGridAnyBmP == (Bitmap *)0 )
		{
			newMessage("Not enough memory for fast grid.");
			goto slow_grid;
		}

		/* Erase Bitmap. */
		rectf( fineGridAnyBmP, fineGridAnyBmP->rect, F_CLR );

		/* Fill Bitmap with correct pattern. */
		initpoints( fineGridAnyBmP, F_STORE );

		for( xpos = (Xmin & 0xf) % tmpGridsize;
		     xpos < bmapwid; xpos += tmpGridsize )
		{
			for( ypos = (YPIC & 0xf) % tmpGridsize;
			     ypos < bmapwid; ypos += tmpGridsize )
			{
				/* point( fineGridAnyBmP,
				   Pt(xpos,ypos), F_STORE); */
				points( Pt(xpos,ypos) );
			}
		}
		endpoints();

		/* Copy Bitmap to screen. */
		for( ypos = YPIC&~0xf; ypos < YBOT; ypos+=bmapwid )
		{
			for( xpos = Xmin&~0xf; xpos < Xmax; xpos+=bmapwid )
			{
				/* Clip the rectangle at (xpos,ypos)
				 * to within the drawing area.
				 */
				inLayer = Rect( xpos, ypos,
					xpos + bmapwid, ypos + bmapwid);
				in_fineGridAnyBm = inLayer;
				if( rectclip(&inLayer,insetRect) != 1 )
					continue;
				/* Find the coordinates in fineGridAnyBmP. */
				in_fineGridAnyBm.origin.x =
				  inLayer.origin.x - in_fineGridAnyBm.origin.x;
				in_fineGridAnyBm.origin.y =
				  inLayer.origin.y - in_fineGridAnyBm.origin.y;
				in_fineGridAnyBm.corner.x = bmapwid +
				  inLayer.corner.x - in_fineGridAnyBm.corner.x;
				in_fineGridAnyBm.corner.y = bmapwid +
				  inLayer.corner.y - in_fineGridAnyBm.corner.y;
				/* bitblt(fineGridAnyBmP, in_fineGridAnyBm,
					P->layer, inLayer.origin, F_XOR );
				 */
				bitblt(fineGridAnyBmP, in_fineGridAnyBm,
					&display, inLayer.origin, F_XOR );
			}
			wait(CPU);
		}
		bfree(fineGridAnyBmP);
	}
	break;

	/*
	 * This algorithm is slower than the bitmap algorithm and
	 * is only used when the other one can not be used due to
	 * a shortage of memory.
	 */
	slow_grid:
		{
		register int y, gs, x, zy, zx;

			gs = tmpGridsize;
			zx = insetRect.origin.x;
			for( x = Xmin; x < zx; x += gs )
				;
			zx = insetRect.corner.x;
			for(         ; x < zx; x += gs )
			{
				zy = insetRect.origin.y;
				for( y = YPIC; y < zy; y += gs )
					;
				zy = insetRect.corner.y;
				for(         ; y < zy; y += gs )
				{
					/*
					point( P->layer, Pt(x, y), F_XOR );
					*/
					point( &display, Pt(x, y), F_XOR );
				}
				wait(CPU);
			}
		}
		break;

	default:
		break;
	}
	/* cursallow(); */
	/* X11 */
#else /* FINEALIGN */
	/* X11 */
	if( currAlignment > 2 ) {
		if( currAlignment == 8 ) {
			texture( &display, inset(Rect(Xmin,YPIC,Xmax,YBOT), LW),
				 &grid8, F_XOR);
		} else {
			texture( &display, inset(Rect(Xmin,YPIC,Xmax,YBOT), LW),
			 	 &grid, F_XOR );
		}
	}
	return;

#endif /* FILEALIGN */
	/* X11 */
#else
	/* not X11 */
#ifdef FINEALIGN
	register int	xpos;
	register int	ypos;
	int 		bmapwid;
	int		tmpGridsize;
	Rectangle	insetRect;
	Rectangle 	inLayer, in_fineGridAnyBm;
	Bitmap *	fineGridAnyBmP;
#ifdef DMD5620
	register int	i;
	Texture16	grid16;
#endif

	/*
	 * insetRect is a rectangle that defines the coordinates
	 * of the drawing area in P->layer.
	 */ 
	insetRect = inset(Rect(Xmin,YPIC,Xmax,YBOT), LW );

	cursinhibit();

	tmpGridsize = currGridsize < 0 ? -currGridsize : currGridsize;

	switch( tmpGridsize )
	{
	case 1:
	case 2:
	case 3:
		break;

	/* case 1: */
		/* For a tmpGridsize of 1, every single pixel on
		 * the display gets turned on, so replacing the
		 * normal algorithm with a call to rectf makes for
		 * a very fast operation.
		 */
	/*	rectf( P->layer, insetRect, F_XOR );
		break;
	 */

	/* case 2: */
	case 4:
	case 8:
	case 16:
		/* For grid sizes which are a power of two,
		 * a texture can be generated and painted on the
		 * screen.
		 */
		/*
		 * Pixels are painted inside the texture to line up with
		 * the point Pt(Xmin,YPIC).
		 */
		fineGrid = T_white;
		for( xpos = (Xmin & 0xf) % tmpGridsize;
		     xpos < 16; xpos += tmpGridsize )
			for( ypos = (YPIC & 0xf) % tmpGridsize;
			     ypos < 16; ypos += tmpGridsize )
				point(&fineGridBm, Pt(xpos,ypos), F_STORE);

		/* Then the texture is painted on the insetRect
		 * inside the layer.
		 */
#ifdef DMD630
		texture16( P->layer, insetRect, &fineGrid, F_XOR );
#endif
#ifdef DMD5620
		for( i=0; i<16; i++ ) {
			grid16.bits[i] = (short) (fineGrid.bits[i] >> 16);
		}
		texture16( P->layer, insetRect, &grid16, F_XOR );
#endif
		break;

	/* case 3: */
	case 5:
	case 6:
	case 7:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	/*
	 * For grid sizes which are not a power of two,
	 * a bitmap is generated which is 16*gridsize pixels
	 * wide. Pixels for the appropiate grid size are painted
	 * onto the bitmap to align with Pt(Xmin,YPIC).
	 *
	 * The this bitmap is bitblt'ed all over the screen,
	 * one section at a time.
	 */
	{

		/* Allocate Bitmap. */
		bmapwid = 16*tmpGridsize;
		fineGridAnyBmP = balloc(Rect(0, 0, bmapwid, bmapwid));

		if( fineGridAnyBmP == (Bitmap *)0 )
		{
			newMessage("Not enough memory for fast grid.");
			goto slow_grid;
		}

		/* Erase Bitmap. */
		rectf( fineGridAnyBmP, fineGridAnyBmP->rect, F_CLR );

		/* Fill Bitmap with correct pattern. */
		for( xpos = (Xmin & 0xf) % tmpGridsize;
		     xpos < bmapwid; xpos += tmpGridsize )
			for( ypos = (YPIC & 0xf) % tmpGridsize;
			     ypos < bmapwid; ypos += tmpGridsize )
				point( fineGridAnyBmP, Pt(xpos,ypos), F_STORE);

		/* Copy Bitmap to screen. */
		for( ypos = YPIC&~0xf; ypos < YBOT; ypos+=bmapwid )
		{
			for( xpos = Xmin&~0xf; xpos < Xmax; xpos+=bmapwid )
			{
				/* Clip the rectangle at (xpos,ypos)
				 * to within the drawing area.
				 */
				inLayer = Rect( xpos, ypos,
					xpos + bmapwid, ypos + bmapwid);
				in_fineGridAnyBm = inLayer;
				if( rectclip(&inLayer,insetRect) != 1 )
					continue;
				/* Find the coordinates in fineGridAnyBmP. */
				in_fineGridAnyBm.origin.x =
				  inLayer.origin.x - in_fineGridAnyBm.origin.x;
				in_fineGridAnyBm.origin.y =
				  inLayer.origin.y - in_fineGridAnyBm.origin.y;
				in_fineGridAnyBm.corner.x = bmapwid +
				  inLayer.corner.x - in_fineGridAnyBm.corner.x;
				in_fineGridAnyBm.corner.y = bmapwid +
				  inLayer.corner.y - in_fineGridAnyBm.corner.y;
				bitblt(fineGridAnyBmP, in_fineGridAnyBm,
					P->layer, inLayer.origin, F_XOR );
			}
			wait(CPU);
		}
		bfree(fineGridAnyBmP);
	}
	break;

	/*
	 * This algorithm is slower than the bitmap algorithm and
	 * is only used when the other one can not be used due to
	 * a shortage of memory.
	 */
	slow_grid:
		{
		register int y, gs, x, zy, zx;

			gs = tmpGridsize;
			zx = insetRect.origin.x;
			for( x = Xmin; x < zx; x += gs )
				;
			zx = insetRect.corner.x;
			for(         ; x < zx; x+ = gs )
			{
				zy = insetRect.origin.y;
				for( y = YPIC; y < zy; y += gs )
					;
				zy = insetRect.corner.y;
				for(         ; y < zy; y += gs )
				{
					point( P->layer, Pt(x, y), F_XOR );
				}
				wait(CPU);
			}
		}
		break;

	default:
		break;
	}
	cursallow();
	/* not X11 */
#else /* FINEALIGN */
	/* not X11 */
	int		xpos;
	int		ypos;
	int		xpos8;
	int		ypos8;
#ifdef DMD5620
	register int	i;
	Texture16	grid16;
#endif

	if( currAlignment > 2 ) {
		grid = T_white;
		xpos = (Xmin) & 15;	/* mod 16 */
		ypos = (YPIC) & 15;

		cursinhibit();

		point( &gridBm, Pt(xpos,ypos), F_STORE );

		if( currAlignment == 8 ) {
			xpos8 = (Xmin+8) & 15;	/* mod 16 */
			ypos8 = (YPIC+8) & 15;
			point( &gridBm, Pt(xpos,ypos8), F_STORE );
			point( &gridBm, Pt(xpos8,ypos), F_STORE );
			point( &gridBm, Pt(xpos8,ypos8), F_STORE );
		}

#ifdef DMD630
		texture16( P->layer, inset(Rect(Xmin,YPIC,Xmax,YBOT), LW ), 
			 &grid, F_XOR );
#endif
#ifdef DMD5620
		for( i=0; i<16; i++ ) {
			grid16.bits[i] = (short) (grid.bits[i] >> 16);
		}
		texture16( P->layer, inset(Rect(Xmin,YPIC,Xmax,YBOT), LW ), 
			 &grid16, F_XOR );
#endif
		cursallow();
	}
	/* not X11 */
#endif /* FILEALIGN */
#endif /* X11 */
}


drawEDbutton(depth) 
  int depth;
{	
  char		s[5]; 
  Point		p;
  register int	dy;
  Rectangle	tmp;

  drawButtonPicture( brushes[ED] );
  tmp = Rpt( add( brushes[ED].origin, Pt(LW+1,LW+1) ),
	     sub( brushes[ED].corner, Pt(LW+BUTDEPTH-1,LW+BUTDEPTH-1)) );
  flash(&tmp, Pt(0,0));
  p.x = (brushes[ED].corner.x + brushes[ED].origin.x)>>1;
  p.y = brushes[ED].origin.y + 5;
  centeredText(p,"edit depth");
  p.y += 18;
  sprintf(s,"%d",depth);
  centeredText(p,s);
  p.y += 20;
  centeredText(p,"(button)");
}


drawButtonPicture(r)
    Rectangle r;
{
    /* Remove triangular corners. */
    drawIcon( Pt(r.origin.x+8, r.corner.y-BUTDEPTH+8), &lowerTriangle );
    drawIcon( Pt(r.corner.x-BUTDEPTH+8, r.origin.y+8), &upperTriangle );

    rectf( &display, r, F_XOR );
    rectf( &display, Rpt( add( r.origin, Pt(LW,LW) ), 
			  sub( r.corner, Pt(LW+BUTDEPTH,LW+BUTDEPTH))), 
	   F_XOR );

    /* Draw diagonal line for corner. */
    segment( &display, sub(r.corner,Pt(BUTDEPTH,BUTDEPTH)), r.corner, F_XOR );
    
    /* Draw two lines to show edges. */
    segment( &display, 
	     Pt(r.origin.x+1        , r.corner.y-BUTDEPTH ),
	     Pt(r.corner.x-BUTDEPTH , r.corner.y-BUTDEPTH ),
	     F_XOR );
    segment( &display, 
	     Pt(r.corner.x-BUTDEPTH , r.origin.y+1        ),
	     Pt(r.corner.x-BUTDEPTH , r.corner.y-BUTDEPTH ),
	     F_XOR );

}


changeBrush(new) 
  register int new;
{
  Rectangle tmp;

  if (currentBrush>-1) {
    tmp = Rpt( add( brushes[currentBrush].origin, Pt(LW+1,LW+1) ),
	       sub( brushes[currentBrush].corner, 
			    Pt(LW+BUTDEPTH-1,LW+BUTDEPTH-1) ) ) , 
    flash(&tmp, Pt(0,0));
  }
  if (new>-1) {
    tmp = Rpt( add( brushes[new].origin, Pt(LW+1,LW+1) ),
	       sub( brushes[new].corner, 
			    Pt(LW+BUTDEPTH-1,LW+BUTDEPTH-1) ) ) , 
    flash(&tmp, Pt(0,0));
  }
  currentBrush = new;
}



/* Writes in "Mouse Buttons" sub-window the button operation description. */

changeButtons(new) 
  register int new;
{	
  if (buttonState != new) { 
    writeButtonLabels(buttonState);
    writeButtonLabels(new);
    buttonState = new;
  }
}
    

writeButtonLabels(i) 
  register int i;
{
	if( left_hand ) {
		labeledButton(Pt(Xbut3,Ybut123),but1text[i]);
   		labeledButton(Pt(Xbut2,Ybut123),but2text[i]);
   		labeledButton(Pt(Xbut1,Ybut123),but3text[i]);
	} else {
		labeledButton(Pt(Xbut1,Ybut123),but1text[i]);
   		labeledButton(Pt(Xbut2,Ybut123),but2text[i]);
   		labeledButton(Pt(Xbut3,Ybut123),but3text[i]);
	}
}


initFontList ()
{
  if ((fonts=(fontBlk *)getSpace(sizeof(fontBlk))) == (fontBlk *) NULL) {
    newMessage ("No memory in terminal");
    sleep (100);
    exit ();
  }
  fonts->f = &defont;
  fonts->ps = 10;
  fonts->num = DEFONT;
  fonts->useCount = 0;
  fonts->next = fonts;
  fonts->last = fonts;
}

putVersion()
{
	/* note: xcip -V finds these strings from the object file */
	/* the extra blank at the end delimits it from following strings */
	newMessage( Pgm_name );
	moreMessage( " version " );
	moreMessage( PGM_VERSION );
	moreMessage( " " );
	drawCopyright( Pt(Xtext+180,Ytext-7) );
}


sameSizeRect( rectA, rectB )
	Rectangle	rectA;
	Rectangle	rectB;
{
	return eqpt( sub(rectA.corner,rectA.origin) ,
		     sub(rectB.corner,rectB.origin) );
}


/* Go to next line in message area, scroll message lines up if at bottom. */
   static void
nextLine()
{
    char *p,*p1;
    extern char * strchr();

    PointCurr.x = MSGXMIN;
    PointCurr.y += NS;		/* Newline Size. Height of a character.	 */
    if( PointCurr.y > (MSGYMAX-NS) ) {
	/* Scroll up message lines. */
	bitblt( &display, Rect(MSGXMIN, MSGYMIN+NS, MSGXMAX, MSGYMAX),
		&display, Pt(MSGXMIN, MSGYMIN), F_STORE );
	clearRegion( Rect( MSGXMIN, MSGYMAX-NS, MSGXMAX, MSGYMAX ) );
	PointCurr.y -= NS;

	*MessagePnt = '\0';
	p = strchr(MessageBuf, '\n');
	if (p == 0) 
	    p = &MessageBuf[strlen(MessageBuf)];
	if ((int)(p - MessageBuf) > (MSGXMAX-CW))
	    p = &MessageBuf[(MSGXMAX-CW)];
	p1 = alloc(strlen(p)+1);
	strcpy(p1, p);
	strcpy(MessageBuf, p1);
	free(p1);
	MessagePnt = &MessageBuf[strlen(MessageBuf)];
    }
}


/* This routine clears the message area and sets a pointer to 
   the beginning of it. */

    void
initMessage ()
{
    PointCurr.x = MSGXMIN;
    PointCurr.y = MSGYMIN;
    clearRegion( Rect( MSGXMIN, MSGYMIN, MSGXMAX, MSGYMAX ) );
    MessageBuf[0] = '\0';
    MessagePnt = MessageBuf;
}


/* This routine displays a string at 'PointCurr'. */

    void
moreMessage (str)
    char *    str;
{
    while( *str != '\0' ) {
        if( (*str == '\n') || (*str == '\r') ) {
            nextLine();
	    *MessagePnt++ = '\n';
        } else if( *str == '' ) {
            ringbell();
        } else if( *str == '	' ) {
	    /* Ignore tabs. */
        } else {
            if( PointCurr.x > (MSGXMAX-CW) ) 
                nextLine();
            jchar( *str );
	    *MessagePnt++ = *str;
        }
	str++;
    }
    *MessagePnt = '\0';
}


/* This routine displays a message at the bottom of the cip layer */

    void
newMessage (str)
    char *str;
{
    initMessage ();
    moreMessage (str);
}


/* This routine displays given string in message box and then awaits
 * for user to edit it - terminated by return or mouse buttons.  The 
 * new string is returned in the first parameter.  The function returns
 * which button was used and 4 for any other reason.
 */

getMessage (str)
  char *str;
{
  register char c;
  register int i;
  Point startPoint;
  int	r = 4;
  char *p;

  /* wait for release of all buttons */
  for(;button123();nap(2))
    ;

  startPoint = PointCurr;
  p = MessagePnt;
  moreMessage(str);
  i = strlen(str);

  cursswitch( &textCursor );
  clearKeyboard();

  do {
    wait( KBD | MOUSE );
    if (P->state & RESHAPED)
	redrawLayer();

    if( button1() ) {
	r = 1;
	break;
    }

    if( button2() ) {
	r = 2;
	break;
    }

    if( button3() ) {
	r = 3;
	break;
    }
	
    if (own() & KBD ) {
	c = kbdchar();

    	if( (c=='\033') || (c == '\r') )
		break;

    	if (i>0) {
	  /* erase present string */
	  PointCurr = startPoint;
	  MessagePnt = p;
	  moreMessage(str);
	}
    	  
#ifdef DMD5620
    	if (c >= 0x82 && c <= 0x89) {
    		int j,k;
    		j = c - 0x82;
    		k = 0;
    		while ((c = BRAM->pfkeys[j][k].byte) && (++k <= PFKEYSIZE)) {
    			if (i >= MAXNAMESIZE) {
    				ringbell ();
    				str[i] = '\0';
    				break;
    			}
    			else {
    				str[i++] = c;
    			}
    		}
    	} else {
#endif
    		switch (c) {
    		case '@':
    		case 'U'-'@': 
    			{
    				i=0;
    				str[0] = '\0';
    				break;
    			}
    		case 'W'-'@': 
    			{
    				i = backspaceOneWord(str,i);
    				break;
    			}
    		case '\b': 
    			{
    				str[(i>0)? --i : 0] = '\0';
    				break;
    			}
    		default: 
    			{
    				if (i >= MAXNAMESIZE) {
    					ringbell ();
    				}
    				else {
    					str[i++] = c;
    					str[i] = '\0';
    				}
    				break;
    			}
    		}
#ifdef DMD5620
    	}
#endif
        PointCurr = startPoint;
	MessagePnt = p;
	moreMessage(str);
    }
  } while ( 1 );

  /* Wait for all buttons to be released. */
  for(;button123();nap(2))
    ;
  cursSwitch ();
  return r;
}


	void
printSpace()
{
	char	S[40];

#ifdef DMDTERM
# ifdef DMD630
	sprintf( S, "space: bytes remaining: %ld\n", spaceLeft() );
# endif
# ifdef DMD5620
	sprintf( S, "space: bytes remaining: %d\n", spaceLeft() );
# endif
	newMessage( S );
#endif /* DMDTERM */
}



#define CEOF 0xff

    void
printPWD()
{
    register FILE *	fp;
    register int	c;
    register int	i;
    static char		s[MAXNAMESIZE+1] = {'\0'};

    moreMessage("pwd: ");

    /* Check if pwd has already been done. */
    if( s[0] == '\0' ) {
	/* Go to host to do pwd */
	fp = popen("pwd","r");
	if( fp == (FILE *) NULL ) {
            moreMessage("failed\n");
            return;
	}

	c = getc( fp );
	i = 0;
#ifdef DMD5620
	while( (c >= '\040') && (c <= '\176') && (i < MAXNAMESIZE) )
#else
	while( (c != CHAR_EOF) && (i < MAXNAMESIZE) )
#endif
	{
            s[i++] = (char) c;
            c = getc( fp );
	}
	s[i]   = '\0';
	pclose(fp);
    }

    moreMessage(s);
    moreMessage("\n");
}


	void
printChgStatus()
{
    /* Temporarily inhibit until implemented:
	if( anyChgs )
	    moreMessage("*** diagram has been changed since last store ***");
	else
	    moreMessage("no changes since last store");
    */
}
