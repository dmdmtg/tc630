#include "cip.h"

#ifdef DMDTERM
#ifdef DMD5620
# include "line.h"
#endif

# include "layer.h"
#endif 

#define DOT 2
#define DASH 10
#define ARROWwid 10
#define ARROWht 10

#ifndef ONEDOTOH
  Point PointCurr;
#endif

extern char *Pgm_name;
extern Point jString ();
extern Rectangle brushes[];
struct thing addOffset();


/* These operations temporarily reset the window to the drawing sub-window
   to automatically obtain the clipping operation on drawing graphics.   */

#ifdef X11
# define WINDOW		&display
# define CLIPON		set_clip(clip_rect)
# define CLIPOFF	unset_clip()

#else /* X11 */
# define WINDOW		P->layer
  Word *    		saveBase;
  Rectangle 		saveScreen;

#define CLIPON \
  saveBase   = P->layer->base; \
  saveScreen = P->layer->rect;  \
  P->layer->base = addr(P->layer,brushes[PIC].origin); \
  P->layer->rect = brushes[PIC]

#define CLIPOFF \
  P->layer->base = saveBase; \
  P->layer->rect = saveScreen

#endif /* X11 */


/* This routine clips an arc within rectangle brushes[PIC] */

xarc(p0, p1, p2) 
Point p0, p1, p2;
{
  CLIPON;
  arc(WINDOW, p0, p1, p2, F_XOR);
  CLIPOFF;
}


/* This routine clips a spline within rectangle brushes[PIC] */

xspline(offset,p, n) 
Point offset;
register Point *p;
int n;
{
  CLIPON;
  spline(offset,p, n);
  CLIPOFF;
}

/* This routine clips a line within rectangle brushes[PIC] */

xsegment (p, q)
Point p, q;
{
  CLIPON;
  segment(WINDOW , p, q, F_XOR);
  CLIPOFF;
}

draw(t,offset) 
  register struct thing *t; 
  Point offset; 
{
  register struct thing *s; 
  Rectangle rc; 
  Point p1,p2; 
  register int u;

  if (t != (struct thing *) NULL) {
    /* cursinhibit(); */
    switch(t->type) {
      case CIRCLE: {
	CLIPON;
	circle(WINDOW,add(offset,t->origin),
	       t->otherValues.radius,F_XOR);
	CLIPOFF;
	break;
      }
      case BOX: {
	CLIPON;
	rc = raddp(t->bb,offset);
	if (t->border == DOTTED) {
	  dashedBox(rc,DOT);
	}
	else {
	  if (t->border == DASHED) {
	    dashedBox(rc,DASH);
	  }
	  else {
	    box(rc);
	  }
	}
	CLIPOFF;
	break;
      }
      case ELLIPSE: {
	CLIPON;
	Ellipse(add(offset,t->origin),t->otherValues.ellipse.ht,
	    t->otherValues.ellipse.wid);
	CLIPOFF;
	break;
      }
      case LINE:  {
	CLIPON;
	p1 = add(t->origin,offset);
	p2 = add(t->otherValues.end,offset);
	if (t->border == DOTTED) {
	  dashedLine(p1,p2,DOT);
	}
	else {
	  if (t->border == DASHED) {
	    dashedLine(p1,p2,DASH);
	  }
	  else {
	    segment(WINDOW,p1,p2,F_XOR);
	  }
        }
	if ((t->arrow==startARROW)||(t->arrow==doubleARROW)) {
	  arrow(p2,p1);
	}
	if ((t->arrow==endARROW)||(t->arrow==doubleARROW)) {
	  arrow(p1,p2);
	}
	CLIPOFF;
	break;
      }
      case ARC: {
	CLIPON;
	arc (WINDOW , add(offset,t->origin),
			 add(offset,t->otherValues.arc.start),
	                 add(offset,t->otherValues.arc.end),F_XOR);
	CLIPOFF;
	break;
      }
      case TEXT: {
	CLIPON;
	drawText(add(offset,t->origin),
		 t->otherValues.text.s,
		 t->otherValues.text.just,
		 t->otherValues.text.spacing,
		 t->otherValues.text.f->f );
	CLIPOFF;
	break;
      }
      case SPLINE: {
	CLIPON;
	u = t->otherValues.spline.used;
	spline(offset,t->otherValues.spline.plist,u);
	if ((t->arrow==startARROW)||(t->arrow==doubleARROW)) {
	  arrow(add(offset,t->otherValues.spline.plist[2]),
	        add(offset,t->otherValues.spline.plist[1]));
	}
	if ((t->arrow==endARROW)||(t->arrow==doubleARROW)) {
	  arrow(add(offset,t->otherValues.spline.plist[u-2]),
	        add(offset,t->otherValues.spline.plist[u-1]));
	}
	CLIPOFF;
	break;
      }
      case MACRO: {
	if ((s=t->otherValues.list->parts) != (struct thing *)NULL) {
	  do {
	    draw(s,add(offset,t->origin));
	    s = s->next;
	  } while (s != t->otherValues.list->parts);
	}
	break;
      }
    }
    /* cursallow(); */
  }
}

xbox(r) 
Rectangle r; 
{
  xsegment(r.origin,Pt(r.corner.x,r.origin.y));
  xsegment(Pt(r.corner.x,r.origin.y),r.corner);
  xsegment(r.corner,Pt(r.origin.x,r.corner.y));
  xsegment(Pt(r.origin.x,r.corner.y),r.origin);
}

box(r) 
Rectangle r; 
{
  segment(WINDOW , r.origin,Pt(r.corner.x,r.origin.y),F_XOR);
  segment(WINDOW , Pt(r.corner.x,r.origin.y),r.corner,F_XOR);
  segment(WINDOW , r.corner,Pt(r.origin.x,r.corner.y),F_XOR);
  segment(WINDOW , Pt(r.origin.x,r.corner.y),r.origin,F_XOR);
}

dashedBox(r,dashsize) 
Rectangle r; 
register int dashsize;
{
  dashedLine(r.origin,Pt(r.corner.x,r.origin.y),dashsize);
  dashedLine(Pt(r.corner.x,r.origin.y),r.corner,dashsize);
  dashedLine(r.corner,Pt(r.origin.x,r.corner.y),dashsize);
  dashedLine(Pt(r.origin.x,r.corner.y),r.origin,dashsize);
}

dashedLine(s,end,dashsize) 
  Point s, end; 
  int dashsize;
{
  register int e, dx, dy, i, toDraw, yinc, xinc, swit;

#ifdef X11
  /* For better efficiency, using initpoints, points, and endpoints. */
  initpoints(&display, F_XOR);
#endif /* X11 */
  dx = abs(end.x - s.x);
  dy = abs(end.y - s.y);
  xinc = ((end.x-s.x)>0)? 1 : -1;
  yinc = ((end.y-s.y)>0)? 1 : -1;
  swit = (dy>dx);
  toDraw = 1;
  e = (swit)? (2*dx - dy) : (2*dy - dx);
  for (i=0; i < ((swit) ? dy : dx); i++) {
    if (i>0 && i%dashsize==0) {
      toDraw = (toDraw==1)?0:1;
    }
    if (toDraw) {
#ifdef X11
      points(s,F_XOR);
#else /* X11 */
      point(WINDOW,s,F_XOR);
#endif /* X11 */
    }
    if (e>0) {
      if (swit) {
	s.x += xinc;
      }
      else {
	s.y += yinc;
      }
      e += (swit)? (2*dx - 2*dy) : (2*dy - 2*dx);
    }
    else {
      e += (swit)? 2*dx : 2*dy;
    }
    if (swit) {
      s.y += yinc;
    }
    else {
      s.x += xinc;
    }
  }
#ifdef X11 
  endpoints();
#endif /* X11 */
}

int 
degrees(d)  
register int d;
{
  while (d>360) {
    d -= 360;
  }
  while (d<0) {
    d += 360;
  }
  return(d);
}

arrow(a, b)		/* draw arrow (without line) */
Point a,b; 
{
  register int alpha, rot, hyp;
  register int dx, dy;

  rot = atan2( ARROWwid / 2, ARROWht);
  hyp = norm(ARROWwid,ARROWht,0);
  alpha = atan2(b.y-a.y, b.x-a.x);
  dx = muldiv(hyp,cos(degrees(alpha + 180 + rot)),1024);
  dy = muldiv(hyp,sin(degrees(alpha + 180 + rot)),1024);
  /* line(x1+dx, y1+dy, x1, y1); */
#ifdef DMDTERM
  if ((b.x==a.x) && (b.y < a.y)) {
    dy = -dy;
  }
#endif
  cursinhibit(); 
  segment(WINDOW,add(b,Pt(-dx,dy)),b,F_XOR); 
  cursallow();
  dx = muldiv(hyp,cos(degrees(alpha + 180 - rot)),1024);
  dy = muldiv(hyp,sin(degrees(alpha + 180 - rot)),1024);
  /* line(x1+dx, y1+dy, x1, y1); */
#ifdef DMDTERM
  if ((b.x==a.x) && (b.y < a.y)) {
    dy = -dy;
  }
#endif
  cursinhibit(); 
  segment(WINDOW,add(b,Pt(dx,-dy)),b,F_XOR); 
  cursallow();
}

centeredText(p,s) 
  Point p; 
  register char *s; 
{
  string( &defont, s, &display, Pt( p.x - (strwidth(&defont,s)>>1), p.y ), 
	  F_XOR );
}

flash(b,offset) 
  register Rectangle *b; 
  Point offset;
{
  if (b != (Rectangle *) NULL) {
    cursinhibit();
    rectf( &display, raddp(*b,offset), F_XOR );
    cursallow();
  }
}


flashThing(t,offset) 
  register struct thing *t; 
  Point offset;
{	
  Rectangle r;
  if (t != (struct thing *) NULL) {
    cursinhibit();
    switch (t->type) {
      case CIRCLE:
      case BOX:
      case ELLIPSE:
      case LINE:
      case ARC:
      case SPLINE: {
	draw(t,offset);
	break;
      }
      case MACRO:
      case TEXT: {
	/* Must add Pt(1,1) to corner to fill box fully - why ??? */
	r.origin = t->bb.origin;
	r.corner = add( t->bb.corner , Pt(1,1) );
	flash( &r , offset );
	break;
      }
    }
    cursallow();
  }
}



Ellipse(p,h,w) 
  Point p; 
  register int h, w;
{
  ellipse(WINDOW , p, w>>1, h>>1, F_XOR);
}

drawZigZag(offset,p,n) 
	Point		offset;  
	register Point *p;  
	int		n; 
{
	register int	i;
	Point		j;
	Point		k;

	if (p != (Point *) NULL) {
		cursinhibit();
		if (n>0) {
			j = add(offset, p[1]);
			for (i=2; i<=n; i++) {
				k = add (offset, p[i]);
				xsegment(j, k);
				j = k;
			}
		}
		cursallow();
	}
}

#define SCALE (long) 1000
#define STEPS 10

spline(offset,p, n) 
  Point offset; 
  register Point *p;
  int n;
{
  register long w, t1, t2, t3; 
  register int i, j; 
  Point q;
  Point pcurrent;		/* Current point */

  if (p != (Point *) NULL) {
    p[0] = p[1];
    p[n] = p[n-1];
    cursinhibit();
    pcurrent = add(offset,p[0]);
    for (i = 0; i < n-1; i++) {
      for (j = 0; j < STEPS; j++) {
        w = SCALE * j / STEPS;
        t1 = w * w / (2 * SCALE);
        w = w - SCALE/2;
        t2 = 3*SCALE/4 - w * w / SCALE;
        w = w - SCALE/2;
        t3 = w * w / (2*SCALE);
        q.x = (t1*p[i+2].x + t2*p[i+1].x + t3*p[i].x + SCALE/2) / SCALE;
        q.y = (t1*p[i+2].y + t2*p[i+1].y + t3*p[i].y + SCALE/2) / SCALE;
        segment(&display, pcurrent, add(offset,q), F_XOR);
        pcurrent = add(offset, q );
      }
    }
    cursallow();
  }
}


#ifdef DMDTERM
Bitmap	tempBitmap = { 0, 1, 0, 0, 16, 16, 0 };

#ifdef DMD5620
Word	tempIcon[16];
#endif
#endif 


drawIcon(axy,icon)
	Point			axy;
	register Texture16 *	icon;
{

#ifdef X11
        Bitmap *tempBitmap, *balloc();

        tempBitmap = balloc(Rect(0, 0, 16, 16));
        texture(tempBitmap, tempBitmap->rect, icon, F_STORE);
        bitblt(tempBitmap, tempBitmap->rect, &display, sub(axy, Pt(8,8)), F_OR);
        bfree(tempBitmap);

#else 

#ifdef DMD630
	tempBitmap.base = icon->bits;
#endif

#ifdef DMD5620
	register int	i;
	register Word	word;

	for( i=0; i<15; i++) {
		word = icon->bits[i];
		tempIcon[i] = word << 16;
	}
	tempBitmap.base = tempIcon;
#endif
	bitblt(&tempBitmap, tempBitmap.rect, &display,
		sub(axy, Pt(8,8)), F_XOR);
#endif /* X11 */
}


drawCopyright( pt )
	Point pt;
{
	pt = string(&defont,"Copyright",&display,pt,F_XOR);
	drawIcon( add(pt,Pt(11,7)), &copyright );
	string(&defont,"1995 AT&T",&display,add(pt,Pt(24,0)),F_XOR);
	string(&defont,"All Rights Reserved",&display,add(pt,Pt(-74,14)),F_XOR);
}


/**** CIP logo ****/

/* Note: used only in DMD terminals. */

#ifdef DMDTERM

  typedef struct {
	Point	start;
	Point	end;
  } Logo_Line;

  /* Insert line data convert from logo.pic file. */
  Logo_Line	logo[] = {
# include "logo.data"
  -1,-1,-1,-1
  };

drawToolIcon()
{
	register Logo_Line *	line;

	jmoveto( Pt(40,25) );
	jstring(Pgm_name);

	for( line = &logo[0]; line->start.x != -1; line++ ) {

#ifdef DMD630
		jsegment( Pt( 100 + line->start.x , 900 - line->start.y ), 
		          Pt( 100 + line->end.x   , 900 - line->end.y   ), 
				  F_STORE );
#endif
#ifdef DMD5620
		jsegment( Pt( 25 + line->start.x , 900 - line->start.y ), 
		          Pt( 25 + line->end.x   , 900 - line->end.y   ), 
				  F_STORE );
#endif

	}
}

#endif /* DMDTERM */
