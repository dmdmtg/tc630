#include <jerq.h>

Bitmap
ToBitmap(bits, bytewidth, ox, oy, cx, cy)
char *bits;
{
	Bitmap *bm;
	int dx, dy;
	register unsigned char *to;
	int i, lbytes;
#ifdef X11
	Pixmap pm;
	unsigned char *mybits;
	char *malloc();
	extern unsigned char Jrevbits[256];
	register short *sp;
	int j;
#endif /*X11*/

	dx = cx - ox;
	dy = cy - oy;
	bm = balloc(Rect(ox, oy, cx, cy));
#ifdef X11
	/*
	 * Convert bits from jerq to X11 bitmap format - assumes shorts
	 * chars and longs will also work on 68k word order machines
	 * but not on others (i.e. VAX, 386)
	 */
	lbytes = (dx + 7)/8;
	mybits = (unsigned char *)malloc(lbytes * dy + 4);
	for(i = 0; i < dy; i++) {
		to = mybits + i * lbytes;
		sp = (short *)(bits + i * bytewidth);
		for (j = (dx + 15)/16; j; j--, sp++) {
			*to++ = Jrevbits[(*sp >> 8) & 0xFF];
			*to++ = Jrevbits[*sp & 0xFF];
		}
	}
	pm = XCreateBitmapFromData(dpy, display.dr, mybits, dx, dy);
	/*
	 * Now convert the Bitmap to a Pixmap of the correct depth
	 */
	XSetForeground(dpy, gc, fgpix);
	XSetFunction(dpy, gc, GXcopy);
	XCopyPlane(dpy, pm, bm->dr, gc, 0, 0, dx, dy, 0, 0, 1);
	XFreePixmap(dpy, pm);
	free(mybits);
#endif /*X11*/
#ifdef SUNTOOLS
	lbytes = mpr_d((Pixrect *)bm->dr)->md_linebytes;
	to = (unsigned char *)mpr_d((Pixrect *)bm->dr)->md_image;
	for(i = 0; i < dy; i++) {
		bcopy(bits, to, lbytes);
		to += lbytes;
		bits += bytewidth;
	}
#endif /*SUNTOOLS*/
	return *bm;
}
