#include "cip.h"

extern Rectangle brushes[];
extern void	 printInfo();
extern void	 printPt();

#ifdef DMD5620
/* No float on DMD 5620 terminals! */
#define float int
#endif


/* Rounds the given integer based on currAlignment. */
	int 
alignInt(i) 
	int i;
{
	if( currAlignment > 0 ) {
		if( i >= 0 ) {
			return ( ( (i+(currAlignment>>1)) / currAlignment) 
				* currAlignment);
		} else {
			return ( ( (i-(currAlignment>>1)) / currAlignment) 
				* currAlignment);
		}

	} else {
		return( i );
	}
}


	int 
alignByInt(i, value ) 
	register int	i;
	register int	value;
{
	if( i >= 0 ) {
		return ( ( (i+value) / currAlignment) * currAlignment);
	} else {
		return ( ( (i-value) / currAlignment) * currAlignment);
	}
}


/* Rounds the point given based on currAlignment. */
	Point 
align(p) 
	Point p;
{
	if( currAlignment > 0 ) {
		p.x = alignInt(p.x);
		p.y = alignInt(p.y);
	}
	return p;
}


	Point 
alignUp(p) 
	Point p;
{
	if( currAlignment > 0 ) {
		p.x = alignByInt( p.x, currAlignment-1 );
		p.y = alignByInt( p.y, currAlignment-1 );
	}
	return( p );
}


	Point 
alignDown(p) 
	Point p;
{
	if( currAlignment > 0 ) {
		p.x = alignByInt( p.x, 0 );
		p.y = alignByInt( p.y, 0 );
	}
	return( p );
}


  Point 
track(p,offset,b,th) 
  Point p,offset; int b; 
  register struct thing *th;
{
  Point t, ot;
  Rectangle r;

  cursinhibit();

  if ( b != ARC ) {
    p = add(p,offset);
    ot = p;
  } else {
    ot = add(p,offset);
  }

  if( th != TNULL ) {
    do {
      t = add( align( sub(MOUSE_XY,offset) ), offset );
      if ((t.x!=ot.x)||(t.y!=ot.y)) {
	switch (b) {
	  case BOX: {
	    xbox( canon(p, ot) );
	    r = canon(p,t);
	    xbox( r );
	    th->origin = sub( r.origin, offset );
	    th->otherValues.corner = sub( r.corner, offset );
	    break;
	  }
	  case ARC: {
	    th->type = ARC;
	    th->otherValues.arc.start = p;
	    th->otherValues.arc.end = sub( ot,offset );
	    th->origin=computeArcOrigin( p, sub( ot,offset ));
	    if (!eqpt(th->otherValues.arc.start,
		      th->otherValues.arc.end)) {
	      draw(th,offset);	
	    }
	    th->otherValues.arc.end = sub( t,offset );
	    th->origin=computeArcOrigin( p, sub( t,offset ));
	    draw(th,offset);	
	    break;
	  }
	  case LINE: {
	    xsegment(p,ot);
	    xsegment(p,t);
	    th->origin =          sub( p, offset );
	    th->otherValues.end = sub( t, offset );
	    break;
	  }
	  case REVLINE: {
	    xsegment(p,ot);
	    xsegment(p,t);
	    th->origin =          sub( t, offset );
	    th->otherValues.end = sub( p, offset );
	    break;
	  }
	  case SPLINE: {
	    xsegment(p,ot);
	    xsegment(p,t);
	    break;
	  }
	  case REVSPLINE: {	/* See note 1 */
	    xsegment(ot,p);
	    xsegment(t,p);
	    break;
	  }
	  case GROWCIRCLE: {
	    draw(th,offset);
	    th->otherValues.radius=alignInt(distance(add(th->origin,offset),t));
	    draw(th,offset);
	    break;
	  }
	  case MOVE: {
	    draw(th,offset);
	    th->origin = sub(t,offset);
	    draw(th,offset);
	    break;	
	  }
	  case GROWEHT: {
	    draw(th,offset);
	    th->otherValues.ellipse.ht =  
		abs(th->origin.y-t.y+offset.y)<<1;
	    draw(th,offset);
	    break;
	  }
	  case GROWEWID: {
	    draw(th,offset);
	    th->otherValues.ellipse.wid = 
		abs(th->origin.x-t.x+offset.x)<<1;
	    draw(th,offset);
	    break;
	  }
	  case ELLIPSE: {
	    draw(th,offset);
	    th->otherValues.ellipse.wid = 
		(abs(th->origin.x-t.x+offset.x))<<1;
	    th->otherValues.ellipse.ht =  
		abs(th->origin.y-t.y+offset.y)<<1;
	    draw(th,offset);
	    break;
	  }
	}
	ot = t;
        printInfo( th );
      }
      nap(2);
    } while (bttn2());
  }
  switch (b) {
    case ARC:
      draw(th,offset);
      break;
    case BOX:
      xbox (canon (p, ot));
      boundingBox( th );
      break;
    case LINE: 
    case REVLINE: 
      xsegment(p,ot);
      break;
  }
  cursallow();
  return(sub(t,offset));
}


/* NOTE 1: */
/* Lines drawn and undrawn using XOR mode must always be drawn in the */
/* same direction.  This is because the last point on the line is not */
/* drawn.  The same line drawn twice, but in opposite directions */
/* will leave two points on the screen.  So, because of this when */
/* the tracking routine is tracking a spline from p[2] to p[1] the */
/* line must be drawn in the opposite direction (from p[1] to p[2]). */
/* Therefore, the need for the REVSPLINE value. */


  Point 
track2(offset,o1, o2, p) 
  Point offset, o1, o2, p;
{
  Point op;

  o1 = add(offset,o1);
  o2 = add(offset,o2);
  op = add(offset,p);
  do {
    cursinhibit ();
    p = add( align( sub(MOUSE_XY,offset) ), offset );
    if ((p.x!=op.x)||(p.y!=op.y)) {
      xsegment(o1,op);
      xsegment(op,o2);
      xsegment(o1,p);
      xsegment(p,o2);
    }
    op = p;
    cursallow ();
    nap(2);
  } while (bttn2());
  return(sub(p,offset));
}



  Point 
trackMacro(p,offset) 
  Point p,offset;
{
  Point t, ot;

  cursinhibit();
  p = add(p,offset);
  ot = p;

  do {
      t = add( align( sub(MOUSE_XY,offset) ), offset );
      if ((t.x!=ot.x)||(t.y!=ot.y)) {
	    xbox( canon(p, ot) );
	    xbox( canon(p, t) );
	    ot = t;
      }
      nap(2);
  } while (bttn2());

  xbox (canon (p, ot));
  cursallow();
  return(sub(t,offset));
}

	Rectangle
moveBox(p,r,offset,originonly)
	Point		p;
	Point		offset; 
	Rectangle	r; 
	int		originonly;
{
	Point		op;
	Rectangle	or; 

	cursinhibit();
	or = r;
	op = align(p); 
	op = p;
	do {
		p = align(sub(MOUSE_XY,offset)); 
		r.origin = align( add(or.origin,sub(p,op)) );

		if( !eqpt( r.origin, or.origin ) ) {

			r.corner = add( or.corner, sub(r.origin,or.origin) );

			xbox( raddp(or,offset) );
			xbox( raddp(r, offset) );

			initMessage();
			printPt( r.origin );
			if (!originonly) {
				moreMessage( " to " );
				printPt( r.corner );
			}

			op = p;
			or = r;
		}

		nap(2);

	} while (bttn2()); 

	cursallow();
	return(r);
}

arcOrigin(t,offset) 
  register struct thing *t; 
  Point offset;
{
  Point oc, c, s, e, om, m, org, mid, A, B, tempP, newP;
  long  sin_a, sin_b, cos_a, cos_b, num, den;


  cursinhibit();
  s = add(offset,t->otherValues.arc.start);
  e = add(offset,t->otherValues.arc.end);
  org = add(offset,t->origin);
  mid = div(add(s,e),2);
  oc = org;
  om = oc;
  do {
				       /*if not a full circle or just a point*/
    if ((s.x != e.x) || (s.y != e.y)) {
      m = add( align( sub(MOUSE_XY,offset) ), offset );
      if (distance(m,om) !=0) {                              /*if mouse moved*/
	if (distance(m,oc) != 0) {           /*if mouse not already at origin*/

	  if ((s.x - e.x) == 0) {            /*if it is a horizontal bisector*/
	    c.x = m.x;
	    c.y =org.y;
	  }
	  else if ((s.y -e.y) == 0) {    /*else  if it is a vertical bisector*/
	    c.x = org.x;
	    c.y =m.y;
	  }
	  else {
		       /*********************
			     sin_a = (mid.y -oc.y)/d1
			     cos_a = (mid.x -oc.x)/d1
			     sin_b = (m.y -oc.y)/d2
			     cos_b = (m.x -oc.x)/d2
			     c.x = d2*cos_a*(cos_a*cos_b -sin_a*sin_b) + oc.x;
			     c.y = d2*sin_a*(cos_a*cos_b -sin_a*sin_b) + oc.y;
		       ***********************/
      /*let's find two points on the line that bisects the arc. Nominally*/
      /*this would be the origin, and the midpoint of the line between   */
      /*the start of the arc and the end of the arc.  However, if the arc is*/
      /*a semi-circle, then these two points are coincident. Therefore */
      /* we will use the mid-point, and determine a second point whose distance*/
      /* from the mid-point is non-zero except for the vertical and */
      /*horizontal conditions which have been handled separately.*/

	     if (s.y < mid.y) {
		tempP = s;
	     }
	     else {
		tempP = e;
	     }
	     newP.x = mid.x + (mid.y - tempP.y);       /*newP is second point*/
	     newP.y = mid.y + (tempP.x - mid.x);
	     if (mid.x > newP.x) {
		 B= mid;
		 A= newP;
	     }
	     else {
		 B= newP;
		 A = mid;
	     }
	     sin_a = (long)(m.y - A.y);
	     cos_a = (long)(m.x - A.x);
	     sin_b = (long)(B.y -A.y);
	     cos_b = (long)(B.x -A.x);
	     if (m.x <A.x) {
	       sin_b = -sin_b;
	       cos_b = -cos_b;
	     }
	     num = cos_b*((cos_a*cos_b) +(sin_a*sin_b));
	     den = cos_b*cos_b + sin_b*sin_b;
	     c.x = (num/den)+ A.x;
	     c.y = ((num*sin_b)/(den*cos_b)) + A.y;
	  }
	  if (distance (c,oc) != 0)  {            /*if the origin has changed*/
	    eraseAndDrawArc(oc,e,s,c,e,s);
	    oc = c;
	    om = m;
	    t->origin = sub(c,offset);
	    printInfo(t);
	  }
	}
      }
    }
    nap(2);
  } while (bttn2());
  cursallow();
}




arcStart(t,offset) 
  register struct thing *t; 
  Point offset;
{
  Point oc, s, e, oe, os,orig_e, temp_e;
  float d,od;
  register int  i, delta;
  int  x_change, y_change, x1,y1;


  cursinhibit();
  os = add(offset,t->otherValues.arc.start);
  oc = add(offset,t->origin);
  oe = add(offset,t->otherValues.arc.end);
  orig_e = oe;
  if ((od = (float)distance(os,oc)) >0) {        /*if  original radius not 0 */
    do {
      s = add( align( sub(MOUSE_XY,offset) ), offset );
      if (distance(s,os)>2) {                                 /*if change > 2*/
	if ((d = (float)distance (s,oc)) > 0) {        /*if new radius not 0 */
	   t->otherValues.arc.start=sub(s,offset);
	   e.x = oc.x + (orig_e.x -oc.x)*(d/od);
	   e.y = oc.y + (orig_e.y -oc.y)*(d/od);
		       /*the value for "e" may be wrong due to roundoff error*/
		       /*so we are going to refine it*/
	   if(distance(e,oc) != (int)d) {           /*if we need to change it*/
	     if(distance(e,oc) < (int)d) {            /*if we need to grow it*/
	       delta =1;
	     }
	     else {
	       delta = -1;
	     }
	     temp_e.x = e.x;                     /*use values just calculated*/
	     temp_e.y = e.y;
	     x1 = orig_e.x - oc.x;
	     if (x1 <0)
		x1 = - x1;
	     y1 = orig_e.y - oc.y;
	     if (y1 < 0)
		y1 = - y1;
	     i=0;
	     while (1) {       /*we will break when we have changed it enough*/
	       ++i;

			 /*first, find out if delta x is greater than delta y*/
	       if (x1 >= y1) {
		 x_change = i*delta;
		 y_change =  (x_change * y1)/x1;
	       }
	       else {
		 y_change = i*delta;
		 x_change =  (y_change *x1)/y1;
	       }
	       if  (temp_e.x < oc.x) {
		    x_change = -x_change;
	       }
	       if (temp_e.y < oc.y) {
		    y_change = -y_change;
	       }
	       e.x = temp_e.x + x_change;
	       e.y = temp_e.y + y_change;
			  /*now we have the formulas for growing or shrinking*/
	       if (delta ==1) {
		 if(distance(e,oc) >= (int)d) {             /*if grown enough*/
		    break;
		 }
	       }
	       else if(distance(e,oc) <= (int)d) {         /*if shrunk enough*/
		 break;
	       }
	     }
	   }
	   t->otherValues.arc.end = sub(e,offset);
	   eraseAndDrawArc(oc,oe,os,oc,e,s);
	   os = s;
	   oe = e;
	   printInfo(t);
	}
      }
      nap(2);
    } while (bttn2());
  }
  else do {
    nap(2);
  } while (bttn2());
  cursallow();
}


arcEnd(t,offset) 
  register struct thing *t; 
  Point offset;
{
  Point oc, s, e, oe, os,orig_s, temp_s;
  float d,od;
  register int  i, delta;
  int  x_change, y_change, x1,y1;


  cursinhibit();
  os = add(offset,t->otherValues.arc.start);
  orig_s = os;
  oe = add(offset,t->otherValues.arc.end);
  oc = add(offset,t->origin);
  if ((od = (float)distance(oe,oc)) > 0) {   /*if  original radius not 0 */
    do {
      e = add( align( sub(MOUSE_XY,offset) ), offset );
      if (distance(e,oe)>2) {                                 /*if change > 2*/
	if ((d = (float)distance (e,oc)) > 0) {        /*if new radius not 0 */
	   t->otherValues.arc.end = sub(e,offset);
	   s.x = oc.x + (orig_s.x -oc.x)*(d/od);
	   s.y = oc.y + (orig_s.y -oc.y)*(d/od);
		       /*the value for "s" may be wrong due to roundoff error*/
		       /*so we are going to refine it*/
	   if(distance(s,oc) !=(int)d) {            /*if we need to change it*/
	     if(distance(s,oc) < (int)d) {            /*if we need to grow it*/
	       delta =1;
	     }
	     else {
	       delta = -1;
	     }
	     temp_s.x = s.x;                     /*use values just calculated*/
	     temp_s.y = s.y;
	     x1 = orig_s.x - oc.x;
	     if (x1 <0)
		x1 = - x1;
	     y1 = orig_s.y - oc.y;
	     if (y1 < 0)
		y1 = - y1;
	     i=0;
	     while (1) {       /*we will break when we have changed it enough*/
	       ++i;

			 /*first, find out if delta x is greater than delta y*/
	       if (x1 >= y1) {
		 x_change = i*delta;
		 y_change =  (x_change * y1)/x1;
	       }
	       else {
		 y_change = i*delta;
		 x_change =  (y_change *x1)/y1;
	       }

	       if (temp_s.x < oc.x) {
		    x_change = -x_change;
	       }
	       if (temp_s.y < oc.y) {
		    y_change = -y_change;
	       }
	       s.x = temp_s.x + x_change;
	       s.y = temp_s.y + y_change;
			  /*now we have the formulas for growing or shrinking*/
	       if (delta ==1) {
		 if(distance(s,oc) >= (int)d) {             /*if grown enough*/
		    break;
		 }
	       }
	       else if(distance(s,oc) <= (int)d) {         /*if shrunk enough*/
		 break;
	       }
	     }
	   }
	   t->otherValues.arc.start = sub(s,offset);
	   eraseAndDrawArc(oc,oe,os,oc,e,s);
	   oe = e;
	   os = s;
	   printInfo(t);
	}
      }
      nap(2);
    } while (bttn2());
  }
  else do  {
    nap(2);
  } while (bttn2());
  cursallow();
}


eraseAndDrawArc(oc,oe,os,c,e,s) 
Point oc, oe, os, c, e, s;
{

  cursinhibit();
  xarc(oc,os,oe);
  xsegment(oc,os);
  xsegment(oc,oe);
  xarc(c,s,e);
  xsegment(c,s);
  xsegment(c,e);
  cursallow();
}
