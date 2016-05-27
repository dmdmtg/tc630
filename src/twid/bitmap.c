/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)bitmap.c	1.1.1.2	(11/11/87)";

#include <dmd.h>
#include <dmdio.h>
#ifdef DMD630
#include <5620.h>
#endif
#define	UP	0
#define	DOWN	1
#define NOANSWER	0

extern msg(),buttons(),Cursinhibit(),Cursallow();
extern Rectangle screenrect;
extern char *type();
Word stagebits[XMAX>>WORDSHIFT];	/* width of screen */
Bitmap stage={
	stagebits,
	    XMAX>>WORDSHIFT,
	    0, 0, XMAX, 1
};
rdbitmap(f,m)
char *f;
register m;
{
	register FILE *fd = fopen(f, "r");
	    Point p;
	    char *s;
	    int pos;
	    Rectangle r;
	    register y, n;
	    if(fd==0){
		msg("can't find file");
		    return -1;
	}
	if(fread((char *)&r, sizeof r, 1, fd)!=1 || r.origin.x <0 ||
	    r.corner.x > XMAX || r.origin.y <0 || r.corner.y > XMAX){

		msg("file not bitmap");
		    fclose(fd);
		    return -1;
	}

	/* bottom left corner of layer minus border*/
	    /* size of bitmap */
	n = r.corner.x-r.origin.x;

	    Cursswitch((Texture16 *)0, 0);
	    msg("absolute positioning (y/n)?");
	    s = type();
	    if(s == NOANSWER || *s == 'y')
	    p=add(r.origin, display.rect.origin);
	    else{
		msg("point to origin (upper left corner of bitmap) and hit a button");
		    Cursswitch((Texture16 *)0, 0);
		    buttons(DOWN);
		    p = mouse.xy;
		    buttons(UP);
			msg("");
	}
	Cursinhibit();
	    pos = p.y;
	    /* draw bitmap; one scan line each time */
	for(y=r.origin.y; y<r.corner.y ; y++){
		    pos++;
		    if(fread((char *)stagebits, (n+7)/8, 1, fd) != 1){
			msg("short file, returning it anyway");
			    sleep(50);
			    break;
		}
		if( pos < screenrect.corner.y )
		    bitblt(&stage, Rect(0, 0, n, 1), &display, Pt(p.x,p.y++), m);
		    else {
			msg("bitmap overflows layer");
			    fclose(fd);
			    Cursallow();
			    return -1;
		}
	}
	Cursallow();
	fclose(fd);
	    return 0;
}
/*
 * Write rectangle r from bitmap b to file f.
 * p is the lockstep position, origin of bitmap such that
 * textures work.  if you don't care about texture alignment,
 * use b->rect.origin

  */
wrbitmap(b, p, r, f)
register Bitmap *b;
Point p;
Rectangle r;
char *f;
{
	register FILE *fd = fopen(f, "w");
	    register x, y, n;
	    x=r.origin.x;
	    r=rsubp(r, p);
	    if(fd==0 || fwrite(&r, sizeof r, 1, fd)!=1){
		msg("can't create file");
		    return 1;
	}
	r=raddp(r, p);
	    n=r.corner.x-r.origin.x;
	    for(y=r.origin.y; y<r.corner.y; y++){
		rectf(b, Rect(x, y, x+n, y+1),F_XOR);
		bitblt(b, Rect(x, y, x+n, y+1), &stage, Pt(0, 0), F_STORE);
		    if(fwrite((char *)stagebits, (n+7)/8, 1, fd) != 1){
			msg("write error");
			    fclose(fd);
			    return 1;
		}
	}
	fclose(fd);
	    return 0;
}
