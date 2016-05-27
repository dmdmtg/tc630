/*      Operations on things     */

#ifdef DMDTERM
# ifndef FONT
#   define	FONT "$DMD/font"
# endif	FONT
#endif /* DMDTERM */

#include "cip.h"

#ifdef X11
# include <X11/Xatom.h>
#endif /* X11 */

extern int nextMacroName;

extern char *envfont;
extern int editDepth;
extern struct macro * macroList;

extern void StringSize();
extern short fontBells;
extern Point alignDown();
extern Point alignUp();

#ifdef HELPSCALE
short currScaleType = SCALE_NONE;
short currLabelType = LABEL_NONE;
short currScaleFactor = 1;
#endif

struct thing *	undo_old = TNULL;	/* Copy of original thing. */
struct thing *	undo_new = TNULL;	/* Points to latest thing. */

/* quadrants for arc start and end points (relative to arc origin) */
#define UPPERLEFT   1
#define UPPERRIGHT  2
#define LOWERRIGHT  3
#define LOWERLEFT   4


  struct thing *
newCircle(p) 
  Point p;
{
  register struct thing *b;

  if ((b = (struct thing *) getSpace(sizeof(struct thing))) != TNULL) {
    b->type = CIRCLE;
    b->origin = p;
    b->otherValues.radius = RADdefault;
    boundingBox(b);
    b->arrow = 0;
  }
  return(b);
}

struct thing *
newBox(r) 
Rectangle r;
{
  register struct thing *b;

  if ((b = (struct thing *) getSpace(sizeof(struct thing))) != TNULL) {
    b->type = BOX;
    b->origin = r.origin;
    b->otherValues.corner = r.corner;
    boundingBox(b);
    b->border = currBoxType;
    b->arrow = 0;
  }
  return(b);
}

struct thing *
newEllipse(p) 
Point p;
{
  register struct thing *b;

  if ((b = (struct thing *) getSpace(sizeof(struct thing))) != TNULL) {
    b->type = ELLIPSE;
    b->origin = p;
    b->otherValues.ellipse.ht = HTdefault;
    b->otherValues.ellipse.wid = WIDdefault;
    boundingBox(b);
    b->border = SOLID;
    b->arrow = 0;
  }
  return(b);
}

struct thing *
newLine(o,c) 
Point o, c;
{
  register struct thing *b;

  if ((b = (struct thing *) getSpace(sizeof(struct thing))) != TNULL) {
    b->type = LINE;
    b->origin = o;
    b->otherValues.end = c;
    boundingBox(b);
    b->border = currLineType;
    b->arrow = currLineArrows;
  }
  return(b);
}

struct thing *
newArc(s,e) 
Point s, e;
{
  register struct thing *b; 

  if ((b = (struct thing *) getSpace(sizeof(struct thing))) != TNULL) {
    b->type = ARC;
    b->otherValues.arc.start = s;
    b->otherValues.arc.end = e;
    b->origin = computeArcOrigin(s,e);
    boundingBox(b);
    b->border = SOLID;
    b->arrow = 0;
  }
  return(b);
}

    struct thing *
newText(p,s) 
    Point p; char *s;
{
    register struct thing *b; fontBlk *f;
    
    if ((b = (struct thing *) getSpace(sizeof(struct thing))) != TNULL) {
        b->type = TEXT;
        b->origin = p;
        f = findFont(currPS,currFont);
        b->otherValues.text.f = f;
        b->otherValues.text.just = currJust;
        b->otherValues.text.spacing = currSpacing;
        b->otherValues.text.s = s;
        boundingBox(b);
        b->border = SOLID;
        b->arrow = 0;
        b->otherValues.text.outName = textOutName++;
    }
    return(b);
}

struct thing *
newSpline(u,s,p) 
int u, s; 
Point *p;
{
  register struct thing *b;

  if ((b=(struct thing *) getSpace(sizeof(struct thing))) != TNULL) {
    b->type = SPLINE;
    b->origin = p[1];
    b->otherValues.spline.used = u;
    b->otherValues.spline.size = s;
    b->otherValues.spline.plist = p;
    boundingBox(b);
    b->border = SOLID;
    b->arrow = currSplineArrows;
  }
  return(b);
}

  struct thing *
newMacro(p,l) 
  Point p; 
  struct macro *l; 
{
  register struct thing *b;

  if ((b = (struct thing *) getSpace(sizeof(struct thing))) != TNULL) {
    b->type = MACRO;
    b->origin = p;
    b->otherValues.list = l;
    l->useCount++;
    boundingBox(b);
    b->border = SOLID;
    b->arrow = 0;
  }
  return(b);
}		


/* Compare things.  Returns TRUE if things are the same.  Will recurse
   to check out macro things.
*/
    BOOL
cmpThings( t1, t2 )
    register struct thing *	t1;
    register struct thing *	t2;
{
    register int		i;
    register struct thing *	tm1;
    register struct thing *	tm2;
    int				n;

    if( t1->type != t2->type )
	return FALSE;

    if( !eqpt(t1->origin,t2->origin) )
	return FALSE;

    switch( t1->type ) {

    case CIRCLE:
	if( (t1->otherValues.radius == t2->otherValues.radius) ) {
            return TRUE;
	} else {
	    return FALSE;
	}

    case BOX:
	if( eqpt(t1->otherValues.corner, t2->otherValues.corner) &&
	    (t1->border == t2->border) ) {
            return TRUE;
	} else {
	    return FALSE;
	}

    case ELLIPSE:
	if( (t1->otherValues.ellipse.ht  == t2->otherValues.ellipse.ht) &&
	    (t1->otherValues.ellipse.wid == t2->otherValues.ellipse.wid) ) {
            return TRUE;
	} else {
	    return FALSE;
	}

    case LINE:
	if( eqpt(t1->otherValues.end, t2->otherValues.end) &&
	    (t1->arrow == t2->arrow) &&
	    (t1->border == t2->border) ) {
            return TRUE;
	} else {
	    return FALSE;
	}

    case ARC:
	if( eqpt(t1->otherValues.arc.start, t2->otherValues.arc.start) &&
	    eqpt(t1->otherValues.arc.end,   t2->otherValues.arc.end) ) {
            return TRUE;
	} else {
	    return FALSE;
	}

    case SPLINE:
	if( (t1->otherValues.spline.used == t2->otherValues.spline.used) &&
	    (t1->otherValues.spline.size == t2->otherValues.spline.size) &&
	    (t1->arrow == t2->arrow) ) {

	    n = t1->otherValues.spline.used;
	    for( i=1; i<=n; i++ ) {
		if( !eqpt(t1->otherValues.spline.plist[i],
		         t2->otherValues.spline.plist[i]  ) )
		    return FALSE;
	    }
	    return TRUE;
	} else {
	    return FALSE;
	}

    case TEXT:
	if( (t1->otherValues.text.just    == t2->otherValues.text.just) &&
	    (t1->otherValues.text.spacing == t2->otherValues.text.spacing) &&
	    (t1->otherValues.text.f       == t2->otherValues.text.f) &&
	    (strcmp(t1->otherValues.text.s, t2->otherValues.text.s)==0) ) {
            return TRUE;
	} else {
	    return FALSE;
	}

    case MACRO:
	t1 = t1->otherValues.list->parts;
	t2 = t2->otherValues.list->parts;
	if( (t1 != TNULL) && (t2 != TNULL) ) {
	    tm1 = t1;
	    tm2 = t2;
	    do {
	        if( !cmpThings( tm1, tm2 ) )
	            return FALSE;
	        tm1 = tm1->next;
	        tm2 = tm2->next;
	    } while( (tm1 != t1) && (tm2 != t2) );

	    if( (tm1==t1) && (tm2==t2) ) {
	        return TRUE;
	    }
	}
	return FALSE;
    }
}


/* Adds thing t to list of things.  Returns the thing after t. */

    struct thing *
insert(t,list) 
    register struct thing *t, *list;
{
    if( t != TNULL ) {
        if( list != TNULL ) {
            t->next = list;
            t->last = list->last;
            list->last->next = t;
            list->last = t;
        } else {
            t->last = t;
            t->next = t;
        }
        return(t->next);  /* Note: return next item to put the new thing 
			     at the end of the list.  This will build
			     the list in order. */
    }
    return(list);
}

    struct thing *
dummy_insert(t,list) 
    register struct thing *t, *list;
{
    if( t != TNULL ) {
        if( list != TNULL ) {
            t->next = list;
            t->last = list->last;
            list->last->next = t;
            list->last = t;
        } else {
            t->last = t;
            t->next = t;
        }
        return(t);  
    }
    return(list);
}
    void
insertFont(t) 
    register fontBlk *t;
{
    if (t != (fontBlk *) NULL) {
        t->next = fonts;
        t->last = fonts->last;
        fonts->last->next = t;
        fonts->last=t;
    }
}

    struct thing *
Remove(t) 
    register struct thing *t;
{
    if (t != TNULL) {
        t->last->next = t->next;
        t->next->last = t->last;
    }
    return(( (t==TNULL) || (t==t->next)) ? TNULL : t->next);
}


/* This routine deletes the given macro from the macro list.
   It has to scan the macro list looking for that macros entry since
   there are no back links within the list. */
    void
removeMacro (m)
    struct macro *m;
{
    register struct macro *ml;	/* Pointer for macro list */

    if (m == macroList) {
        macroList = m->next;
    } else {
        for (ml = macroList; ml != MNULL; ml = ml->next) {
            if (ml->next == m) {	/* Macro found delete it. */
  	        ml->next = m->next;
            }
        }
    }
    if (m->name != (char *)NULL) {
        free (m->name);
    }
    free (m);
    return;
}


  void
freeThing(t) 
  register struct thing * t; 
{
  register struct thing * h; 

  if (t!=TNULL) {

    switch (t->type) {
      case MACRO: {
	if (--t->otherValues.list->useCount <= 0) {
	  for (h=t->otherValues.list->parts; h!=TNULL; ) {
	    h = deleteThing (h);
	  }
	  removeMacro (t->otherValues.list);
	}
	break;
      }
      case TEXT: {
	t->otherValues.text.f->useCount--;
	free (t->otherValues.text.s);
	break;
      }
      case SPLINE: {
	free (t->otherValues.spline.plist);
	break;
      }
    }
    free((char *) t);
  }
}


/* Delete things by removing from linked list of objects and freeing
   space it occupied. */
	struct thing *
deleteThing(t)
	struct thing *	t;
{
	struct thing *	list;

	if( t == TNULL ) {
	    return TNULL;
	} else {
	    if( firstThing == t ) {
		firstThing = t->next;
		if( firstThing == t ) {
		    firstThing = TNULL;
		}
	    }
	    list = Remove(t);
    	    freeThing( t );
	    return list;
	}
}


  struct thing *
deleteAllThings(list) 
  struct thing *list;
{
  register struct thing *h;

  for (h=list; h!=TNULL; ){
    h=deleteThing (h);
  }
  return(TNULL);
}


/* Undo an editted object.  Assumes undo_new is pointing to last
   editted object (linked into the list of objects) and undo_old is 
   a copy of last editted object before it was editted (not linked into 
   the list of objects).
*/
    struct thing *
undo( list )
    struct thing *  list;
{
    struct thing *  tmp;

    if( undo_new != TNULL ) {

        /* If undo_new is firstThing, then reset firstThing. */
        if( firstThing == undo_new ) {
            firstThing = undo_new->next;
            if( firstThing == undo_new ) {
                 firstThing = TNULL;
            }
        }

        /* Unlink from object list. */
        list = Remove( undo_new );
    }

    if( undo_old != TNULL ) {

        /* Link into object list. */
        list = insert( undo_old, list );

	/* If firstThing is null, then set it. */
	if( firstThing == TNULL ) {
	    firstThing = list;
	}
    }

    /* Swap old for new. */
    tmp = undo_new;
    undo_new = undo_old;
    undo_old = tmp;

    return list;
}


/* Save thing to be modified in undo_old. */
	void
save_old( t )
	register struct thing *	t;
{
	if( undo_old != TNULL )
		freeThing( undo_old );/* Reclaim last undo_old space. */
	if( t == TNULL )
		undo_old = TNULL;
	else
		undo_old = copyThing( t, t->origin, Pt(0,0), 0 );
}


/* Save new thing in undo_new. */
	void
save_new( t )
	struct thing *	t;
{
	undo_new = t;
}


/* Clear undo list. */
	void
undo_clear()
{
	if( undo_old != TNULL )
		freeThing( undo_old );
	undo_old = TNULL;

	/* Since undo_new is a pointer to a thing in the official list,
	   just reset it to TNULL. */
	undo_new = TNULL;
}


  struct thing *
selectThing(m,list) 
  Point m; 
  register struct thing *list;
{
  register struct thing *t; 
  register int i;
  Rectangle r;

  if (list != TNULL) {
    /* Cycle thru things by starting with next thing. */
    t = list->next;
    do {
      switch( t->type ) {
      case SPLINE:
	/* See if point is inside imanginary boxes created
	   by each segment of the spline. */
	for( i=2; i<t->otherValues.spline.used; i++) {
	  r = canon( t->otherValues.spline.plist[i-1],
	             t->otherValues.spline.plist[i]    );
	  if( ptinrect( m , inset(r,-nearEDGE) ) )
	    return( t );
	}
	break;
      case LINE:
	/* First do a quick check if within bounding box area and then
	   do a refined check to see if close.  */
	if( ptinrect( m, inset(t->bb,-nearEDGE) ) )
		if( (distance(t->origin,t->otherValues.corner) + nearEDGE) >= 
	    	    (distance(t->origin,m) + distance(t->otherValues.corner,m)))
	  		return( t );
	break;
      default:
	/* See if point is inside or near edge of bounding box. */
	if( ptinrect( m, inset(t->bb,-nearEDGE) ) )
	  return( t );
	break;
      }
      t=t->next;
    } while (t != list->next);
  }
  return ( TNULL );
}

boundingBox(t) 
  register struct thing *t;
{
  Point p, q, r; 
  register int i, h; 
  int hi, w;
  int sx, sy, ex, ey, cx, cy, squad, equad;
  Font *f; 

  if (t != TNULL) {
    switch(t->type) {
      case CIRCLE: {
	p.x = t->otherValues.radius;
	p.y = p.x;
	t->bb.origin = sub(t->origin,p);
	t->bb.corner = add(t->origin,p);
	break;	
      }
      case BOX: {
	t->bb.origin = t->origin;
	t->bb.corner = t->otherValues.corner;
	break;
      }
      case ELLIPSE: {
	p.x = (t->otherValues.ellipse.wid)>>1;
	p.y = (t->otherValues.ellipse.ht)>>1;
	t->bb.origin = sub(t->origin,p);
	t->bb.corner = add(t->origin,p);
	break;
      }
      case LINE: {
	p = t->origin;
	q = t->otherValues.end;
	t->bb = canon (p, q);
	break;
      }
      case ARC: {
	/* compute radius of arc's circle */
	p = t->origin;
	i = distance(p,t->otherValues.arc.start);

	/* save coordinates of arc's start, end, and origin points */
	sx = t->otherValues.arc.start.x;
	sy = t->otherValues.arc.start.y;
	ex = t->otherValues.arc.end.x;
	ey = t->otherValues.arc.end.y;
	cx = p.x;
	cy = p.y;

	/* determine quadrant of start point w.r.t. origin */
	if (sx < cx) {
		squad = sy < cy ? UPPERLEFT : LOWERLEFT;
	} else {
		squad = sy < cy ? UPPERRIGHT : LOWERRIGHT;
	}

	/* determine quadrant of end point w.r.t. origin */
	if (ex < cx) {
		equad = ey < cy ? UPPERLEFT : LOWERLEFT;
	} else {
		equad = ey < cy ? UPPERRIGHT : LOWERRIGHT;
	}

	/*
	 * special case:  start and end points are in the same quadrant and
	 * arc is greater than 270 but less than 360 degrees -- bounding box 
	 * goes around entire circle of arc
	 */
	if (squad == equad 
		&& (((squad == UPPERLEFT || squad == UPPERRIGHT) && sx < ex)
		 || ((squad == LOWERLEFT || squad == LOWERRIGHT) && sx > ex))) {
		t->bb.origin = sub(p,Pt(i,i));
		t->bb.corner = add(p,Pt(i,i));
		break;
	}
		
	/*
	 * all other cases (i.e., start and end points are in different
	 * quadrants OR in same quadrant and arc is less than 90 degrees)
	 */

/*
 * The table below indicates the coordinates for the origin and
 * corner of the bounding box for the arc, given the quadrants of
 * the start and end points for all cases except the special case
 * above.  Legend:  minx = min(sx, ex), miny = min(sy, ey),
 * maxx = max(sx,ex), and maxy = max(sy, ey).
 *
 *      \    end point quadrant
 *       \         
 *  start \       UPPER          UPPER          LOWER          LOWER
 *  point  \      LEFT           RIGHT          RIGHT          LEFT
 * quadrant +--------------+--------------+--------------+--------------+
 *          | (minx,miny)  | (cx-i,miny)  | (cx-i,miny)  | (cx-i,miny)  |
 *   UPPER  |              |              |              |              |
 *   LEFT   |  (maxx,maxy) |  (cx+i,cy+i) |  (maxx,cy+i) |  (maxx,maxy) |
 *          +--------------+--------------+--------------+--------------+
 *          | (minx,cy-i)  | (minx,miny)  | (cx-i,cy-i)  | (cx-i,cy-i)  |
 *   UPPER  |              |              |              |              |
 *   RIGHT  |  (maxx,maxy) |  (maxx,maxy) |  (maxx,cy+i) |  (maxx,maxy) |
 *          +--------------+--------------+--------------+--------------+
 *          | (minx,cy-i)  | (minx,miny)  | (minx,miny)  | (cx-i,cy-i)  |
 *   LOWER  |              |              |              |              |
 *   RIGHT  |  (cx+i,maxy) |  (cx+i,maxy) |  (maxx,maxy) |  (cx+i,maxy) |
 *          +--------------+--------------+--------------+--------------+
 *          | (minx,cy-i)  | (minx,miny)  | (minx,miny)  | (minx,miny)  |
 *   LOWER  |              |              |              |              |
 *   LEFT   |  (cx+i,cy+i) |  (cx+i,cy+i) |  (maxx,cy+i) |  (maxx,maxy) |
 *          +--------------+--------------+--------------+--------------+
 */

	if (squad == equad || squad == LOWERLEFT || equad == UPPERLEFT
		|| (squad == LOWERRIGHT && equad == UPPERRIGHT) ) {
		t->bb.origin.x = min(sx, ex);
	} else {
		t->bb.origin.x = cx - i;
	}
	if (squad == equad || squad == UPPERLEFT || equad == UPPERRIGHT
		|| (squad == LOWERLEFT && equad == LOWERRIGHT) ) {
		t->bb.origin.y = min(sy, ey);
	} else {
		t->bb.origin.y = cy - i;
	}
	if (squad == equad || squad == UPPERRIGHT || equad == LOWERRIGHT
		|| (squad == UPPERLEFT && equad == LOWERLEFT) ) {
		t->bb.corner.x = max(sx, ex);
	} else {
		t->bb.corner.x = cx + i;
	}
	if (squad == equad || squad == LOWERRIGHT || equad == LOWERLEFT
		|| (squad == UPPERRIGHT && equad == UPPERLEFT) ) {
		t->bb.corner.y = max(sy, ey);
	} else {
		t->bb.corner.y = cy + i;
	}
	break;
      }
      case TEXT: {
	f = t->otherValues.text.f->f;
	h = fontheight(f);
	p = add( t->origin , Pt(0,h>>1) );
	StringSize(f,t->otherValues.text.spacing,t->otherValues.text.s,&hi,&w);

	/* Calculate minimal non-aligned bounding box for text */
	switch (t->otherValues.text.just) {
	case CENTER:
	  t->bb.origin = sub( p, Pt(w>>1 , hi>>1) );
	  t->bb.corner = add( p, Pt(w>>1 , hi>>1) );
	  break;
	case LEFTJUST:
	  t->bb.origin = sub( p, Pt(0 , hi>>1) );
	  t->bb.corner = add( p, Pt(w , hi>>1) );
	  break;
	case RIGHTJUST:
	  t->bb.origin = sub( p, Pt(w, hi>>1) );
	  t->bb.corner = add( p, Pt(0, hi>>1) );
	  break;
	}
	break;
      }
      case SPLINE: {
	p.x = Xmin; p.y=YPIC; q.x=Xmax; q.y=YBOT;
	for (i=1; i<t->otherValues.spline.used; i++) {
	  r = t->otherValues.spline.plist[i];
	  p.x = max(p.x,r.x);  p.y = max(p.y,r.y);
	  q.x = min(q.x,r.x);  q.y = min(q.y,r.y);
	}
	t->bb.origin = q;
	t->bb.corner = p;
	break;
      }
      case MACRO: {
	t->bb.origin = add(t->origin,t->otherValues.list->bb.origin);
	t->bb.corner = add(t->origin,t->otherValues.list->bb.corner);
	break;
      }
    }
  }
}

Point 
computeArcOrigin(s,e) 
Point s,e;
{
  Point t;

  if (e.x<s.x) {	/*swap s and e */
    t=s; s=e; e=t;
  }
  return( div(add(add(e,Pt(s.y-e.y,e.x-s.x)),s),2) );
}


char fontString[3] = { '\0', '\0', '\0' };

font2string( n ) 
	int n;
{
      fontString[1] = '\0';
      switch( n ) {
      case ROMAN:
	fontString[0] = 'R';
	break;
      case ITALIC:
	fontString[0] = 'I';
	break;
      case BOLD:
	fontString[0] = 'B';
	break;
      case HELVETICA:
	fontString[0] = 'H';
	break;
      case HI:
	fontString[0] = 'H';
	fontString[1] = 'I';
	break;
      case HB:
	fontString[0] = 'H';
	fontString[1] = 'B';
	break;
      case PALATINO:
	fontString[0] = 'P';
	fontString[1] = 'A';
	break;
      case PI:
	fontString[0] = 'P';
	fontString[1] = 'I';
	break;
      case PB:
	fontString[0] = 'P';
	fontString[1] = 'B';
	break;
      case EXPANDED:
	fontString[0] = 'E';
	break;
      case EI:
	fontString[0] = 'E';
	fontString[1] = 'I';
	break;
      case EB:
	fontString[0] = 'E';
	fontString[1] = 'B';
	break;
      case CONSTANTWIDTH:
	fontString[0] = 'C';
	fontString[1] = 'W';
	break;
      }
}

#ifdef X11
char *Roman_fonts[] = {
	"-adobe-times-medium-r-*-*-*-%d-%s-*-*-*-*",
	"*",
	"-adobe-times-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-times-medium-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-times-medium-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-times-medium-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-times-medium-r-*-*-*-140-*-*-*-*-*-*",
	"-adobe-times-medium-r-*-*-*-140-*-*-*-*-*-*",
	"*",
	"-adobe-times-medium-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-times-medium-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-times-medium-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-times-medium-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-times-medium-r-*-*-*-240-*-*-*-*-*-*",
};
char *Roman_italic_fonts[] = {
	"-adobe-times-medium-i-*-*-*-%d-%s-*-*-*-*",
	"*",
	"-adobe-times-medium-i-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-i-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-i-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-i-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-i-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-i-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-i-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-medium-i-*-*-*-100-*-*-*-*-*-*",
	"-adobe-times-medium-i-*-*-*-100-*-*-*-*-*-*",
	"-adobe-times-medium-i-*-*-*-120-*-*-*-*-*-*",
	"-adobe-times-medium-i-*-*-*-120-*-*-*-*-*-*",
	"-adobe-times-medium-i-*-*-*-140-*-*-*-*-*-*",
	"-adobe-times-medium-i-*-*-*-140-*-*-*-*-*-*",
	"*",
	"-adobe-times-medium-i-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-times-medium-i-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-times-medium-i-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-times-medium-i-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-times-medium-i-*-*-*-240-*-*-*-*-*-*",
};
char *Roman_bold_fonts[] = {
	"-adobe-times-bold-r-*-*-*-%d-%s-*-*-*-*",
	"*",
	"-adobe-times-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-times-bold-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-times-bold-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-times-bold-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-times-bold-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-times-bold-r-*-*-*-140-*-*-*-*-*-*",
	"-adobe-times-bold-r-*-*-*-140-*-*-*-*-*-*",
	"*",
	"-adobe-times-bold-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-times-bold-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-times-bold-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-times-bold-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-times-bold-r-*-*-*-240-*-*-*-*-*-*",
};
char *Helvetica_fonts[] = {
	"-adobe-helvetica-medium-r-*-*-*-%d-%s-*-*-*-*",
	"*",
	"-adobe-helvetica-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-helvetica-medium-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-helvetica-medium-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-helvetica-medium-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-helvetica-medium-r-*-*-*-140-*-*-*-*-*-*",
	"-adobe-helvetica-medium-r-*-*-*-140-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-medium-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-medium-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-medium-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-medium-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-medium-r-*-*-*-240-*-*-*-*-*-*",
};
char *Helvetica_italic_fonts[] = {
	"-adobe-helvetica-medium-o-*-*-*-%d-%s-*-*-*-*",
	"*",
	"-adobe-helvetica-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-medium-o-*-*-*-100-*-*-*-*-*-*",
	"-adobe-helvetica-medium-o-*-*-*-100-*-*-*-*-*-*",
	"-adobe-helvetica-medium-o-*-*-*-120-*-*-*-*-*-*",
	"-adobe-helvetica-medium-o-*-*-*-120-*-*-*-*-*-*",
	"-adobe-helvetica-medium-o-*-*-*-140-*-*-*-*-*-*",
	"-adobe-helvetica-medium-o-*-*-*-140-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-medium-o-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-medium-o-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-medium-o-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-medium-o-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-medium-o-*-*-*-240-*-*-*-*-*-*",
};
char *Helvetica_bold_fonts[] = {
	"-adobe-helvetica-bold-r-*-*-*-%d-%s-*-*-*-*",
	"*",
	"-adobe-helvetica-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-helvetica-bold-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-helvetica-bold-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-helvetica-bold-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-helvetica-bold-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-helvetica-bold-r-*-*-*-140-*-*-*-*-*-*",
	"-adobe-helvetica-bold-r-*-*-*-140-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-bold-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-bold-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-bold-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-bold-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-helvetica-bold-r-*-*-*-240-*-*-*-*-*-*",
};
char *Palatino_fonts[] = {
	"-*-palatino-medium-r-*-*-*-%d-%s-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-140-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-140-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-240-*-*-*-*-*-*",
};
char *Palatino_italic_fonts[] = {
	"-*-palatino-medium-i-*-*-*-%d-%s-*-*-*-*",
	"*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-140-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-140-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-o-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-o-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-o-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-o-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-o-*-*-*-240-*-*-*-*-*-*",
};
char *Palatino_bold_fonts[] = {
	"-*-palatino-bold-r-*-*-*-%d-%s-*-*-*-*",
	"*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-140-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-140-*-*-*-*-*-*",
	"*",
	"-adobe-courier-bold-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-bold-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-bold-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-bold-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-bold-r-*-*-*-240-*-*-*-*-*-*",
};
char *Expanded_fonts[] = {
	"-*-lucida-medium-r-*-*-*-%d-%s-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-140-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-140-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-240-*-*-*-*-*-*",
};
char *Expanded_italic_fonts[] = {
	"-*-lucida-medium-i-*-*-*-%d-%s-*-*-*-*",
	"*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-140-*-*-*-*-*-*",
	"-adobe-courier-medium-o-*-*-*-140-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-o-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-o-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-o-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-o-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-o-*-*-*-240-*-*-*-*-*-*",
};
char *Expanded_bold_fonts[] = {
	"-*-lucida-bold-r-*-*-*-%d-%s-*-*-*-*",
	"*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-140-*-*-*-*-*-*",
	"-adobe-courier-bold-r-*-*-*-140-*-*-*-*-*-*",
	"*",
	"-adobe-courier-bold-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-bold-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-bold-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-bold-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-bold-r-*-*-*-240-*-*-*-*-*-*",
};
char *Constant_fonts[] = {
	"-adobe-courier-medium-r-*-*-*-%d-%s-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-80-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-100-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-120-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-140-*-*-*-*-*-*",
	"-adobe-courier-medium-r-*-*-*-140-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-180-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-240-*-*-*-*-*-*",
	"*",
	"-adobe-courier-medium-r-*-*-*-240-*-*-*-*-*-*",
};

char **Font_strings[] = {
	Roman_fonts,
	Roman_italic_fonts,
	Roman_bold_fonts,
	Helvetica_fonts,
	Helvetica_italic_fonts,
	Helvetica_bold_fonts,
	Palatino_fonts,
	Palatino_italic_fonts,
	Palatino_bold_fonts,
	Expanded_fonts,
	Expanded_italic_fonts,
	Expanded_bold_fonts,
	Constant_fonts,
};
#endif /* X11 */

/* This routine searches the list pointed to by the global 'fonts' */
/* for a font with point size s and type n.  If it can't find that */
/* font then defont is used.                                       */
/* Special case:  If the user runs out of space, then the first    */
/* entry, which is always defont, is used.  This will allow the    */
/* user to write out the file with the font description intact.    */

	fontBlk *
findFont(ps, n)
	register short		ps;	/* Point size of desired font */
	register short		n;  /* Type of font - ROMAN, BOLD, ITALLICS, etc.  */
{
	register fontBlk *	f;
	register int		i;
	char			fn[70];
	char			fname[10];

	/* See if font is already loaded */

	f = fonts->next;		/* Skip over defont entry */
	while (f != fonts) {
		if (f->ps == ps && f->num == n) {
			return f;
		}
		f = f->next;
	}

	/* Must load the font. */
	if ((f = (fontBlk * )getSpace(sizeof(fontBlk))) == (fontBlk * )NULL) {
		outOfSpace ();
		f = fonts;	/* Use defont if there is no room */
		return f;
	} 

	font2string( n );

	f->ps = ps;

#ifdef X11
	newMessage("Loading font: ");
	sprintf(fn, "%s-%d", fontString, ps );
	moreMessage(fn);

	/*
	 * The zeroth element in the Font_strings array has a '%d' for font
	 *   size.  This will work well for servers with scalable fonts,
	 *   notably X11R5 or later.
	 */
	f->f = (Font *)malloc(sizeof(Font));
	sprintf( fn, Font_strings[n-1][0], ps*10, "100-100" );
	*(f->f) = getfont(fn);

	if (f->f->fid == defont.fid) {
		/* 
		 * silently try *-* for resolution if 100-100 doesn't work.
		 * The 100-100 resolutions don't exist at least on Sunos4.
		 * To make up for that, add 2 to the point size, but avoid
		 *  the ones which don't exist in the font arrays; that is,
		 *  less than 2, greater than 24, or odd sizes greater than 14.
		 */
		ps = ps + 2;
		if (ps > 24)
		    ps = 24;
		else if ((ps > 14) && ((ps % 2) == 1))
		    ps++;
		sprintf( fn, Font_strings[n-1][0], ps*10, "*-*" );
		*(f->f) = getfont(fn);
	}
	    
	if (f->f->fid == defont.fid) {
		moreMessage("\nCould not locate font: ");
		moreMessage(fn);
	} else {
		unsigned long ret;
		if (XGetFontProperty(f->f, XA_POINT_SIZE, &ret)) {
			if ((ret/10) != ps) {
				sprintf(fn, "\nInexact point size match: got %d", ret/10);
				moreMessage(fn);
				f->f->fid = defont.fid;  /* force fallback */
			}
		}
	}

	if (f->f->fid == defont.fid) {
		/*
		 * Exact match not possible.  Probably on X11R4 or earlier.
		 */
		sprintf(fn, "\nTrying font: ");
		moreMessage(fn);
		moreMessage(Font_strings[n-1][ps]);
		*(f->f) = getfont(Font_strings[n-1][ps]);
		sleep(30);  /* Hold message for user to read! */
		if (f->f->fid == defont.fid) {
			sprintf(fn, 
			    "\nCould not locate font - using default!");
			moreMessage(fn);
		}
		if( fontBells ) {
			ringbell();
		}
	}
#else /* X11 */


#ifdef DMD630
	/*See if font is in font cache. */
	sprintf( fname, "%s.%d" , fontString, ps );

	f->f = fontrequest( fname );
	if( f->f == 0 ) {
		/* Font is not in cache so attempt to load in. */
#endif

		if( fontBells )
			ringbell();
		newMessage("Loading font: ");
		cursswitch(&hourglass);
		i = ps + 1;
		do {		/* Look for a font <= R.i */
			sprintf(fn, "%s/%s.%d", envfont, fontString, --i );
		} while ((i > 0) && (access(fn, 4) != 0));

		if (i == 0) {
			newMessage("Font not found: ");
			newMessage(fn);
			sleep(60);
			f->f = &defont;
			return f;
		} 
			
		newMessage(fn);

		if ((f->f = getfont(fn)) == (Font * ) NULL) {
			newMessage("Internal error: getfont failed");
			sleep(60);
			cursSwitch();
			f->f = &defont;
			return f;
		} 

		newMessage("");
		if( fontBells )
			ringbell();

#ifdef DMD630
		/* Add font to cache. */

		/* "fontcache(...) would be preferable, but it aborts -
		   why?? */
		fontsave( fname, f->f );
	}
#endif
	cursSwitch();

#endif /* X11 */

	/* Add font to font list. */
	insertFont( f );
	f->num = n;
	f->useCount = 0;
	f->useCount++;
	return f;
}

	
#ifdef HELPSCALE
/* The first entry, "", is for LABEL_NONE */
char *scaleLabels[] = { "", "\"", "' \"", "'",
			" yd.", " mi.", " mm", " m", " km" };
#endif

	void
printNum( n )
	short n;
{
	char	s[20];
#ifdef HELPSCALE
#ifdef DMD5620
#define Sprintf sprintf
#endif
	short	tmpScaleFactor;
	short	tmpLabelType;
	int	whole_n;
	int	frac_n;
	int	precision;
	int	frac_scale;
	int	feet;
	int	inches;
	char	frac_str[20];


	tmpScaleFactor = currScaleFactor;
	tmpLabelType   = currLabelType;

	switch ( currScaleType )
	{
	case SCALE_NONE:
		sprintf(s,"%d", n );
		moreMessage(s);
		return;

	case SCALE_LABEL_ONLY:
		tmpScaleFactor = 1;
		break;

	case SCALE_FACTOR_ONLY:
		tmpLabelType = LABEL_NONE;
		break;

	case SCALE_BOTH:
	default:
		break;
	}

	precision = 0;
	frac_scale = 1;
	if ( tmpScaleFactor > 1 )
	{
		precision++;	frac_scale *= 10;
	}
	if ( tmpScaleFactor > 10 )
	{
		precision++;	frac_scale *= 10;
	}
	if ( tmpScaleFactor > 100 )
	{
		precision++;	frac_scale *= 10;
	}

	whole_n = n / tmpScaleFactor;
	frac_n = muldiv( n%tmpScaleFactor, frac_scale, tmpScaleFactor );

	sprintf(s,"%d",frac_n);
	strcpy(frac_str,".000");
	frac_str[1+precision-strlen(s)] = '\0';
	strcat(frac_str,s);

	switch (tmpLabelType)
	{
	case LABEL_FT_IN:
		feet = whole_n / 12;
		inches = whole_n % 12;
		sprintf(s,"%d'%d%s\"", feet, inches,
			((precision == 0)? "" : frac_str) );
		break;

	case LABEL_NONE:
	case LABEL_IN:
	case LABEL_FT:
	case LABEL_YD:
	case LABEL_MI:
	case LABEL_MM:
	case LABEL_METERS:
	case LABEL_KM:
		sprintf(s,"%d%s%s", whole_n, ((precision==0)?"":frac_str),
			scaleLabels[tmpLabelType] );
		break;

	default:
		sprintf(s,"%d", n );
		break;
	}
#else  /* HELPSCALE */
	sprintf(s,"%d", n );
#endif /* HELPSCALE */
	moreMessage( s );
}


#ifdef HELPSCALE
	void
printPureNum( n )
	short n;
{
	char	s[20];
	sprintf(s,"%d", n );
	moreMessage( s );
}
#endif /* HELPSCALE */

	void
printPt( p )
	Point p;
{
	char	s[30];
	sprintf(s,"(%d,%d)", p.x, p.y);
	moreMessage(s);
} 


	void
printBorder( border )
	short	border;
{
	switch( border ) {
	case SOLID:
		moreMessage("solid");
		break;
	case DASHED:
		moreMessage("dashed");
		break;
	case DOTTED:
		moreMessage("dotted");
		break;
	}
}


	void
printArrows( arrows )
	short	arrows;
{
	switch( arrows ) {
	case 0:
		moreMessage( "no arrows" );
		break;
	case startARROW:
		moreMessage( "arrow at start" );
		break;
	case endARROW:
		moreMessage( "arrow at end" );
		break;
	case doubleARROW:
		moreMessage( "double arrows" );
		break;
	}
}


static char *	FontStyles[] = {
	"Roman",
	"Roman italic",
	"Roman bold",
	"Helvetica",
	"Helvetica italic",
	"Helvetica bold",
	"Palatino",
	"Palatino italic",
	"Palatino bold",
	"Expanded",
	"Expanded italic",
	"Expanded bold",
	"Constant width"
};


	void
printFontStyle( fontStyle )
	short	fontStyle;
{
	if( fontStyle == 0 ) {
	    moreMessage( "Default" );
	} else {
	    moreMessage( FontStyles[fontStyle-1] );
	}
}


	void
printJust( just )
	short	just;
{
	switch( just ) {
	case LEFTJUST:
		moreMessage( "left justified" );
		break;
	case CENTER:
		moreMessage( "centered" );
		break;
	case RIGHTJUST:
		moreMessage( "right justified" );
		break;
	}
}


	void
printInfo(t)
	register struct thing *	t;
{
	register short	n;
	register struct thing *	s;

	if( t == TNULL ) {
		initMessage();
		return;
	};

	switch( t->type ) {
	case CIRCLE:
		newMessage( "Circle: radius=" );
		printNum( t->otherValues.radius );
		moreMessage( " origin=" );
		printPt( t->origin );
		break;
	case BOX:
		newMessage( "Box: " );
		printBorder( t->border );
		moreMessage( " height=" );
		printNum( t->otherValues.corner.y - t->origin.y );
		moreMessage( " width=" );
		printNum( t->otherValues.corner.x - t->origin.x );
		moreMessage( "\n" );
		printPt( t->origin );
		moreMessage( " to " );
		printPt( t->otherValues.corner );
		break;
	case ELLIPSE:
		newMessage( "Ellipse: height=" );
		printNum( t->otherValues.ellipse.ht );
		moreMessage( " width=" );
		printNum( t->otherValues.ellipse.wid );
		moreMessage( "\norigin=" );
		printPt( t->origin );
		break;
	case LINE:
		newMessage( "Line: length=" );
		printNum( (short) distance( t->otherValues.end , t->origin ) );
		moreMessage( " " );
		printBorder( t->border );
		moreMessage( " " );
		printArrows( t->arrow );
		moreMessage( "\n" );
		printPt( t->origin );
		moreMessage( " to " );
		printPt( t->otherValues.end );
		break;
	case ARC:
		newMessage( "Arc: radius=" );
		printNum( (short) distance( 
				t->otherValues.arc.start,
				t->origin)  );
		moreMessage( " origin=" );
		printPt( t->origin );
		moreMessage( "\nstart=" );
		printPt( t->otherValues.arc.start );
		moreMessage( " end=" );
		printPt( t->otherValues.arc.end );
		break;
	case SPLINE:
		newMessage( "Spline: " );
#ifdef HELPSCALE
		printPureNum( t->otherValues.spline.used-1 );
#else
		printNum( t->otherValues.spline.used-1 );
#endif /* HELPSCALE */
		moreMessage( " points " );
		printArrows( t->arrow );
		break;
	case TEXT:
		newMessage( "Text: " );
		printFontStyle( t->otherValues.text.f->num );
		moreMessage( "  point size=" );
#ifdef HELPSCALE
		printPureNum( t->otherValues.text.f->ps );
#else
		printNum( t->otherValues.text.f->ps );
#endif /* HELPSCALE */
		moreMessage( "\n" );
		n = numLines(t->otherValues.text.s);
		if( n == 1 ) {
			moreMessage( "1 line ");
		} else {
#ifdef HELPSCALE
			printPureNum( n );
#else
			printNum( n );
#endif /* HELPSCALE */
			moreMessage( " lines " );
		}
		printJust( t->otherValues.text.just );
		n = t->otherValues.text.spacing;
		if( n == 0 ) {
			moreMessage(" normal spacing" );
		} else {
			moreMessage( " spacing=" );
#ifdef HELPSCALE
			printPureNum( n );
#else
			printNum( n );
#endif /* HELPSCALE */
		}


		/* For Debugging purposes: */
		{   int hi, wi;
		    StringSize( t->otherValues.text.f->f,
				t->otherValues.text.spacing,
				t->otherValues.text.s,
				&hi, &wi );
		    moreMessage("\nsize width=");
		    printNum( wi );
		    moreMessage("  height=");
		    printNum( hi );
		}
		    
		moreMessage("\nBB o=(");
		printNum( t->bb.origin.x );
		moreMessage(",");
		printNum( t->bb.origin.y );
		moreMessage(") c=(");
		printNum( t->bb.corner.x );
		moreMessage(",");
		printNum( t->bb.corner.y );
		moreMessage(")");
		break;

	case MACRO:
		newMessage( "Macro: " );
		n=0;
		s = t->otherValues.list->parts;
		if( s != (struct thing *) NULL ) {
			do {
				n++;
				s = s->next;
			} while ( s != t->otherValues.list->parts );
		}
#ifdef HELPSCALE
		printPureNum( n );
#else
		printNum( n );
#endif /* HELPSCALE */
		if( n == 1 )
			moreMessage( " object height=" );
		else
			moreMessage( " objects height=" );

		printNum( t->bb.corner.y - t->bb.origin.y );
		moreMessage( " width=" );
		printNum( t->bb.corner.x - t->bb.origin.x );
		moreMessage( "\n" );
		printPt( t->bb.origin );
		moreMessage( " to " );
		printPt( t->bb.corner );
		break;
	}
}
