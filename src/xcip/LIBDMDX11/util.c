#include "jerq.h"

char *
gcalloc (nbytes, where)
unsigned long nbytes;
char **where;
{
	*where=(char *)alloc(nbytes);
	return *where;
}

void
gcfree (s)
char *s;
{
	if (s != 0)
		free(s);
}


int      curStack = 0;
Cursor * curSave  = 0;
extern   Cursor	nocursor;

cursinhibit ()	
{
	if( curStack == 0 )
		curSave = cursswitch( &nocursor );
	curStack++;
}

cursallow ()	
{
	curStack--;
	if( curStack <= 0 ) {
		cursswitch( curSave );
		curStack = 0;
	}
}


/* misc functions	*/
border (b,r,i,f)
Bitmap *b;
Rectangle r;
int i;
Code f;
{
	rectf(b, Rect(r.origin.x, r.origin.y, r.corner.x, r.origin.y+i), f);
	rectf(b, Rect(r.origin.x, r.corner.y-i, r.corner.x, r.corner.y), f);
	rectf(b, Rect(r.origin.x, r.origin.y+i, r.origin.x+i, r.corner.y-i), f);
	rectf(b, Rect(r.corner.x-i, r.origin.y+i, r.corner.x, r.corner.y-i), f);
}

Point Pt(x,y)
{
	Point p;

	p.x = x;
	p.y = y;
	return p;
}

Rectangle Rect(x1,y1,x2,y2)
{
	Rectangle r;

	r.origin.x = x1;
	r.origin.y = y1;
	r.corner.x = x2;
	r.corner.y = y2;
	return r;
}

Rectangle Rpt(p1, p2)
Point p1, p2;
{
	Rectangle r;

	r.origin.x = p1.x;
	r.origin.y = p1.y;
	r.corner.x = p2.x;
	r.corner.y = p2.y;
	return r;
}
