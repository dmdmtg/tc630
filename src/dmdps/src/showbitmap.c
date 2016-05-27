/* */
/*									*/
/*	Copyright (c) 1987,1988,1989,1990,1991,1992   AT&T		*/
/*			All Rights Reserved				*/
/*									*/
/*	  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.		*/
/*	    The copyright notice above does not evidence any		*/
/*	   actual or intended publication of such source code.		*/
/*									*/
/* */
#include "dmdps.h"
#include "defs.h"

extern Texture16 cross, stiptext;
extern Point refpt;

Point muldivpt(o,p,q)
Point o,p,q;
{
	static Point pt;

	pt.x = muldiv(o.x,p.x,q.x);
	pt.y = muldiv(o.y,p.y,q.y);
	return pt;
}

#define MBOARDER	2
Point recenter(m,d)
Point m;
Rectangle d;
{
	if (m.y + MBOARDER < d.origin.y)
		m.y=d.origin.y;
	else if (m.y - MBOARDER > d.corner.y)
		m.y=d.corner.y;
	if (m.x + MBOARDER < d.origin.x)
		m.x=d.origin.x;
	else if (m.x - MBOARDER > d.corner.x)
		m.x=d.corner.x;
	return m;
}

/*
	view(s,d,sp,dp) display source rect s in dest rect d such
			that the relative point sp in source is 
			anchored to the relative point dp in dest.
*/
Rectangle view(s,d,sp,dp)
Rectangle s,d;	/* source rect, destrect */
Point sp,dp;	/* view will keep these points "touching" */
{
	static Rectangle sv;

	sv.origin=add(s.origin,sub(sp,dp)); 
	sv.corner=add(sv.origin,sub(d.corner,d.origin));
	rectclip(&sv,s);
	return sv;
}

showbitmap(bp,srcrect)
Bitmap *bp;		/* source bitmap */
Rectangle srcrect;	/* source rectangle */
{
	Point 		sp, 	/* source point */
			dp, 	/* dest point */
			sz, 	/* size of source rect */
			dz;	/* size of dest rect */
	Rectangle 	srcview, /* the subrectangle of src to to show */
			dstrect; /* the place to show it */

	/* set dstrect to show the src bitmap in this layer */
	Reshape();
	dstrect=P->layer->rect;
	dstrect.origin=add(dstrect.origin,Pt(8,38));
	dstrect.corner=add(dstrect.corner,Pt(-8,-(defont.height+10)));

	/* sanity check */
	if (!bp) { rectf(P->layer,dstrect,F_CLR); return; }

	/* compute sizes once */
	sz=sub(srcrect.corner,srcrect.origin);
	dz=sub(dstrect.corner,dstrect.origin);

	/* snap mouse state, and prepare mouse for image scrolling */
	cursswitch(&cross);	/* show refpt with a cross */
	cursset(refpt);		/* start off where we left off.. */
	do {
		/* keep mouse from rolling off layer */
		if (own()&MOUSE && bttn1())
			refpt=recenter(mouse.xy,dstrect);

		/* find view of source to bitblt into dstrect */
		dp=sub(refpt,dstrect.origin);	/* point in display */
		sp=muldivpt(dp,sz,dz);		/* relative point in source */
		srcview=view(srcrect,dstrect,sp,dp);
	    	bitblt(bp,srcview,P->layer,dstrect.origin,F_STORE);

		/* keep mouse from getting too far ahead of display */
		if (own()&MOUSE && bttn1())
			cursset(refpt);	

		sleep(3);
	} while ( own()&MOUSE && bttn1() );

	/* restore mouse */
	cursswitch( (Texture16 *)NULL);		
}
