/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)cursor.c	1.1.1.2	(11/11/87)";

#include <dmd.h>
#include <font.h>
#include "twid.h"
#ifdef DMD630
#include <5620.h>
#endif
static Point curspt;
static bigbrush;
static visible=1;
extern Rectangle saveScreenmap;
extern Word *saveBase;
Cursinhibit(){
	if(bigbrush)
	    flipcursor(curspt);
	    else
		cursinhibit();
}
Cursallow(){
	if(!bigbrush)
	    cursallow();
	    /* else will get redrawn by updatecurs() */
}
flipcursor(p)
Point p;
{

	if( P->state & MOVED | P->state & RESHAPED)
		resetlayer();
	P->layer->base = addr(P->layer,Drect.origin);
	    P->layer->rect = Drect;
	    P->layer->rect.corner.y-=defont.height;
	    bitblt(brush, brush->rect, &display, add(p, brush->rect.origin), F_XOR);
	    visible^=1;
	    P->layer->base = saveBase;
	    P->layer->rect = saveScreenmap;

}
/*
 * Strange routine: b is cursor/texture, isbrush is the
 * brush number.  isbrush==>0 means it's not a brush, it's a cursor
 * (e.g. the box cursor); isbrush>0 means it's a brush, and b
 * is either the cursor (isbrush<BBIG) or to be ignored
 */
Texture16 *
Cursswitch(b, isbrush)
Texture16 *b;
{
	if(visible)
	    flipcursor(curspt);
	    if(!isbrush){	/* just a regular cursswitch() */
		if(bigbrush){
			bigbrush=0;
			    cursallow();
		}
		cursswitch(b);
	} else{		/* a brush, big or small */
		bigbrush=0;
		    if(isbrush <= BBIG)
		    cursswitch(b);
		    else{
			bigbrush=1;
			    cursinhibit();
			    /* NOTE: Cursor/brush is not visible now! */
		}
	}
}
updatecurs(){
	if(!bigbrush)	{   /* mpx is doing it for us */
	nap(3);
	
	    return;
}
	    /* Big brush! */
	if(visible)
	    flipcursor(curspt);
	    curspt=mouse.xy;
	    flipcursor(curspt);
}
