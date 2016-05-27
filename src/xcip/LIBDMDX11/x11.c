#ifdef X11
#include <stdio.h>
#include "jerq.h"

#if defined(BSD) || defined(SYSV)
#include <fcntl.h>
#ifdef SYSV
#define FNDELAY	O_NDELAY
#endif
#else	/* V9 */
#include <sys/filio.h>
#endif

/* Common */
Rectangle	Drect;
Bitmap		display, Jfscreen;
Point		Joffset;
struct Mouse	mouse;
static struct	JProc sP;
struct JProc	*P;
Font		defont;
int		mouse_alive = 0;
int		mousewin = 1;
int		jerqrcvmask = 1;
int		displayfd;
Cursor		normalcursor;
Cursor		nocursor;
static		Jlocklevel;

static short arrow_bits[] = {
	0x0000, 0x0008, 0x001c, 0x003e, 0x007c, 0x00f8, 0x41f0, 0x43e0,
	0x67c0, 0x6f80, 0x7f00, 0x7e00, 0x7c00, 0x7f00, 0x7fc0, 0x0000
};

#ifdef SVR4
static unsigned short arrow_mask_bits[] = {
#else
static short arrow_mask_bits[] = {
#endif
	0x0008, 0x001c, 0x003e, 0x007f,
	0x00fe, 0x41fc, 0xe3f8, 0xe7f0,
	0xffe0, 0xffc0, 0xff80, 0xff00,
	0xff00, 0xffc0, 0xffe0, 0xffc0
};

static short off_bits[] = {
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

/* X11 only */
int		JfuncOR, JfuncCLR, JfuncXOR;
static int	hintwidth, hintheight, hintflags;
GC		gc;
Display		*dpy;
unsigned long	fgpix, bgpix, tmppix;
Colormap	colormap;
XColor		fgcolor, bgcolor;
static unsigned long inputmask =
	ButtonPressMask|ButtonReleaseMask|ButtonMotionMask|StructureNotifyMask|	ExposureMask|KeyPressMask|PointerMotionHintMask;
unsigned char Jrevbits[256] = {
	0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 
	0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0, 
	0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8, 
	0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8, 
	0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 
	0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4, 
	0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 
	0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc, 
	0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2, 
	0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2, 
	0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 
	0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa, 
	0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 
	0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6, 
	0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 
	0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe, 
	0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 
	0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1, 
	0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 
	0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9, 
	0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 
	0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5, 
	0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed, 
	0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd, 
	0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 
	0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3, 
	0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 
	0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb, 
	0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7, 
	0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7, 
	0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 
	0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

initdisplay(argc, argv)
int argc;
char *argv[];
{
	int i;
	XSizeHints sizehints;
	XWMHints xwmhints;
	XSetWindowAttributes xswa;
	XColor ccolor;
	XColor tcolor;
	char *fgcname = 0;
	char *bgcname = 0;
	unsigned long planes;
	char *font;
	char *geom = 0;
	int flags;
	int width, height, x, y;
	char **ap;
	Visual *v;
	Font *fp;

	if(!(dpy= XOpenDisplay(NULL))){
		perror("Cannot open display\n");
		exit(-1);
	}
	displayfd = ConnectionNumber(dpy);

	font = XGetDefault(dpy, argv[0], "JerqFont");
	if(font == NULL)
	    font = "8x13"; /* EPB: closer match to 630 default than "fixed".*/
	colormap = XDefaultColormap(dpy, DefaultScreen(dpy));
	bgpix = WhitePixel(dpy, DefaultScreen(dpy));
	fgpix = BlackPixel(dpy, DefaultScreen(dpy));
	fgcname = XGetDefault(dpy,argv[0],"foreground");
	bgcname = XGetDefault(dpy,argv[0],"background");
#ifdef SYSV
	memset(&sizehints, 0, sizeof(sizehints));
#else
	bzero(&sizehints, sizeof(sizehints));
#endif
	ap = argv;
	i = argc;
	while(i-- > 0){
		if( !strcmp("-font", ap[0]) ){
			font = ap[1];
			i--; ap++;
		}
		else if( !strcmp("-foreground", ap[0]) ||
			 !strcmp("-fg", ap[0]) ){
			fgcname = ap[1];
			i--; ap++;
		}
		else if( !strcmp("-background", ap[0]) ||
			 !strcmp("-bg", ap[0]) ){
			bgcname = ap[1];
			i--; ap++;
		}
		else if( !strcmp("-reverse", ap[0]) ||
			 !strcmp("-rv", ap[0]) ){
			tmppix = bgpix;
			bgpix = fgpix;
			fgpix = tmppix;
		}
		else if( !strcmp("-geometry", ap[0]) ||
			 !strcmp("-g", ap[0]) ){
			geom = ap[1];	
			flags = XGeometry(dpy,DefaultScreen(dpy),geom,0,
					  0,1,1,0,0,&x,&y,&width,&height);
			if(WidthValue & flags){
				sizehints.flags |= USSize;
				sizehints.width = width;
			}
			if(HeightValue & flags){
	    			sizehints.flags |= USSize;
				sizehints.height = height;
			}
			if(XValue & flags){
				sizehints.flags |= USPosition;
				sizehints.x = x;
			}
			if(YValue & flags){
				sizehints.flags |= USPosition;
				sizehints.y = y + 20; /* 20 is for titlebar. */
			}
			i--; ap++;
		}
		ap++;
	}

	/* EPB: get default or user's font, if not found, then get "fixed" 
	   font. */
	fp = XLoadQueryFont(dpy, font);
	if( fp ) 
		defont = *fp;
	else
		defont = getfont("fixed");

	P = &sP;
	sizehints.width_inc = sizehints.height_inc = 1;
	sizehints.min_width = sizehints.min_height = 20;
	sizehints.flags |= PResizeInc|PMinSize;
	if( !(sizehints.flags & USSize) ){
		sizehints.width = defont.max_bounds.width * 80;
		sizehints.height = (defont.max_bounds.ascent +
				 defont.max_bounds.descent) * 24;
		sizehints.flags |= PSize;
		jerqsizehints();
		if (hintwidth)
			sizehints.width = hintwidth;
		if (hintheight)
			sizehints.height = hintheight;
		if (hintflags) {
			sizehints.min_width = hintwidth;
			sizehints.min_height = hintheight;
		}
	}
	if (fgcname || bgcname) {
		v = DefaultVisual(dpy, DefaultScreen(dpy));
		if (DefaultDepth(dpy, DefaultScreen(dpy)) != 1 &&
		    (v->class == PseudoColor || v->class == GrayScale)) {
			if (!fgcname) fgcname = "black";
			if (!bgcname) bgcname = "white";
			if (!XAllocColorCells(dpy,colormap,False,&planes,1,
					 &bgpix, 1)) {
				perror("Cannot alloc color cells\n");
				exit(1);
			}
			fgpix = bgpix | planes;
			XStoreNamedColor(dpy,colormap,fgcname,fgpix,
					 DoRed|DoGreen|DoBlue);
			XStoreNamedColor(dpy,colormap,bgcname,bgpix,
					 DoRed|DoGreen|DoBlue);
		} else {
		  if (fgcname && XAllocNamedColor(dpy,colormap,fgcname,
						  &ccolor,&tcolor))
			fgpix = ccolor.pixel;
		  if (bgcname && XAllocNamedColor(dpy,colormap,bgcname,
						  &ccolor,&tcolor))
			bgpix = ccolor.pixel;
		}
	}

	xswa.event_mask = 0;
	xswa.background_pixel = bgpix;
	xswa.border_pixel = fgpix;
	display.dr = XCreateWindow(dpy, RootWindow(dpy, DefaultScreen(dpy)),
		sizehints.x,sizehints.y, sizehints.width, sizehints.height,
		2, 0, InputOutput, DefaultVisual(dpy, DefaultScreen(dpy)),
		CWEventMask | CWBackPixel | CWBorderPixel, &xswa);
	XSetStandardProperties(dpy, display.dr, argv[0], argv[0],
				None, argv, argc, &sizehints);
	xwmhints.input = True;
	xwmhints.flags = InputHint;
	XSetWMHints(dpy, display.dr, &xwmhints);
	XSelectInput(dpy, display.dr, inputmask);
	XMapWindow(dpy, display.dr);
	fgcolor.pixel = fgpix;
	bgcolor.pixel = bgpix;
	XQueryColor(dpy, colormap, &fgcolor);
	XQueryColor(dpy, colormap, &bgcolor);
	gc = XDefaultGC(dpy, DefaultScreen(dpy));
	XSetForeground(dpy, gc, fgpix);
	XSetBackground(dpy, gc, bgpix);
	XSetWindowBackground(dpy, display.dr, bgpix);
	if ((fgpix^bgpix)&fgpix) {
		JfuncOR = GXor;
		JfuncCLR = GXandInverted;
		JfuncXOR = GXxor;
	} else {
		JfuncOR = GXand;
		JfuncCLR = GXorInverted;
		JfuncXOR = GXequiv;
	}
	XSetFont(dpy, gc, defont.fid);
	XSetLineAttributes(dpy, gc, 0, LineSolid, CapNotLast, JoinMiter);
	Drect.origin.x = 0;
	Drect.origin.y = 0;
	Drect.corner.x = sizehints.width;
	Drect.corner.y = sizehints.height;
	display.rect = Drect;
#ifdef XBUG
	Jfscreen = display;
#else
	Jfscreen.dr = RootWindow(dpy, DefaultScreen(dpy));
	Jfscreen.rect.origin.x = 0;
	Jfscreen.rect.origin.y = 0;
	Jfscreen.rect.corner.x = DisplayWidth(dpy, DefaultScreen(dpy));
	Jfscreen.rect.corner.y = DisplayHeight(dpy, DefaultScreen(dpy));
#endif /*XBUG*/
	normalcursor = ToCursor(arrow_bits, (short *)arrow_mask_bits, 1, 15);
	nocursor     = ToCursor(off_bits,   (short *)off_bits,        1, 15);
	cursswitch(&normalcursor);
	unset_clip();
	while(!(P->state & RESHAPED)) {
		while (XPending(dpy))
			handleinput();
	}
}

/* This must be called before initdisplay */
mousemotion ()
{
	mouse_alive = 1;
	inputmask |= PointerMotionMask;
}

setsizehints (width, height, flags)
{
	hintwidth = width;
	hintheight = height;
	hintflags = flags;
}

request(what)
int what;
{
	if(what & SEND)
#if defined(BSD) || defined(SYSV)
		fcntl(1, F_SETFL, FNDELAY);;
#else
		ioctl(1, FIOWNBLK, 0);
#endif
	if(!(what & RCV))
		jerqrcvmask = 0;
}

Bitmap *
balloc (r)
Rectangle r;
{
	Bitmap *b;

	b = (Bitmap *)malloc(sizeof (struct Bitmap));
	b->dr = XCreatePixmap(dpy, display.dr, r.cor.x-r.org.x,
		r.cor.y-r.org.y, DefaultDepth(dpy, DefaultScreen(dpy)));
	b->flag = BI_OFFSCREEN;
	b->rect=r;
	rectf (b, r, F_CLR);
	return b;
}

void
bfree(b)
Bitmap *b;
{
	if(b) {
		XFreePixmap(dpy, b->dr);
		free((char *)b);
	}
}

Font
getfont(s)
char *s;
{
	Font *fp;

	fp = XLoadQueryFont(dpy, s);
	return fp ? *fp : defont;
}

Point
string (f, s, b, p, c)
Font *f;
char *s;
Bitmap *b;
Point p;
Code c;
{
	XSetFont(dpy, gc, f->fid);
	if(b->flag & BI_OFFSCREEN)
		p = sub(p, b->rect.origin);
	if(c == F_STORE) {
		XSetForeground(dpy, gc, fgpix);
		XDrawImageString(dpy, b->dr, gc, p.x,
				 p.y + f->max_bounds.ascent, s, strlen(s));
	} else {
		XSetFunction(dpy, gc, c);
		if (c == F_XOR)
			XSetForeground(dpy, gc, fgpix^bgpix);
		else
			XSetForeground(dpy, gc, fgpix);
		XDrawString(dpy, b->dr, gc, p.x,
			    p.y + f->max_bounds.ascent, s, strlen(s));
	}
	XSetFont(dpy, gc, defont.fid);
	return(add(p, Pt(strwidth(f,s),0)));
}

int
strwidth (f, s)
Font *f;
register char *s;
{
	return XTextWidth(f, s, strlen(s));
}

/*
 * Convert a blit style texture to a pixmap which can be used in tiling
 * or cursor operations.
 */
Texture
ToTexture(bits)
register short *bits;
{
	unsigned char mybits[32];
	register unsigned char *to;

	for (to = mybits; to < &mybits[32]; bits++) {
		*to++ = Jrevbits[(*bits >> 8) & 0xFF];
		*to++ = Jrevbits[*bits & 0xFF];
	}
	return XCreateBitmapFromData(dpy,display.dr, mybits, 16, 16);
}

Cursor
ToCursor (source, mask, hotx, hoty)
short source[], mask[];
{
	Texture sp, mp;
	Cursor c;

	sp = ToTexture(source);
	mp = ToTexture(mask);
	c = XCreatePixmapCursor(dpy, sp, mp, &fgcolor, &bgcolor, hotx, hoty);
	XFreePixmap(dpy, sp);
	XFreePixmap(dpy, mp);
	return(c);
}

#undef button
handleinput ()
{
	XEvent ev;
	KeySym key;
	unsigned char s[128], *cp;
	int n;
	Window rw, cw;
	int xr, yr, xw, yw;
	unsigned bstate;

	for(;;){
		XNextEvent(dpy, &ev);
		switch (ev.type) {
		case ButtonPress:
			mouse.buttons |= (8 >> ev.xbutton.button);
			mouse.xy.x = ev.xbutton.x;
			mouse.xy.y = ev.xbutton.y;
			mouse.time = ev.xbutton.time;
			break;
		case ButtonRelease:
			mouse.buttons &= ~(8 >> ev.xbutton.button);
			mouse.xy.x = ev.xbutton.x;
			mouse.xy.y = ev.xbutton.y;
			mouse.time = ev.xbutton.time;
			break;
		case MotionNotify:
			XQueryPointer(dpy, display.dr,
				&rw, &cw, &xr, &yr, &xw, &yw, &bstate);
			if(button123() && bstate==0)
				continue;
			mouse.xy.x = xw;
			mouse.xy.y = yw;
			break;
		case MapNotify:
		case NoExpose:
			break;
		case ConfigureNotify:
			if (display.rect.corner.x != ev.xconfigure.width ||
			    display.rect.corner.y != ev.xconfigure.height) {
				display.rect.corner.x = ev.xconfigure.width;
				display.rect.corner.y = ev.xconfigure.height;
				Drect = display.rect;
#ifdef XBUG
				Jfscreen = display;
#endif
				unset_clip();
			}
			break;
		case Expose:
			while (XCheckTypedEvent(dpy, Expose, &ev))
				;
			rectf(&display, Drect, F_CLR);
			P->state |= RESHAPED;
			break;
		case KeyPress:
			mouse.xy.x = ev.xkey.x;
			mouse.xy.y = ev.xkey.y;
			mouse.time = ev.xkey.time;
			n = XLookupString(&ev.xkey, s, sizeof(s), NULL, NULL);
			if(n > 0){
				cp = s;
				P->state |= KBD;
				do{
					kbdread(cp++);
				} while (--n);
			}
			break;
		default:
			break;
		}
		return;
	}
}

/*
 * GRABMASK stolen from PointerGrabMask in X11R4/server/dix/events.c
 * otherwise X11R4 rejects the XGrabPointer
 */
#define GRABMASK (ButtonPressMask | ButtonReleaseMask | \
	EnterWindowMask | LeaveWindowMask |  PointerMotionHintMask \
	| KeymapStateMask | PointerMotionMask | Button1MotionMask | \
        Button2MotionMask | Button3MotionMask | ButtonMotionMask )
Jscreengrab()
{
	if (!Jlocklevel) {
#ifndef XBUG
		int dxr, dyr;
		Window child;

		XTranslateCoordinates(dpy, display.dr, DefaultRootWindow(dpy),
					0,0,&dxr,&dyr, &child);
		Joffset.x = dxr;
		Joffset.y = dyr;
#endif
#if 0
		while (XGrabPointer(dpy, display.dr, False,
                        inputmask & GRABMASK, GrabModeAsync, GrabModeAsync,
			None, *P->cursor, CurrentTime) != GrabSuccess)
				sleep(6);
#endif
		XSetSubwindowMode(dpy, gc, IncludeInferiors);
	}
	Jlocklevel++;
}

Jscreenrelease()
{
	if (--Jlocklevel <= 0) {
		Jlocklevel = 0;
#if 0
		XUngrabPointer(dpy, CurrentTime);
#endif
		XSetSubwindowMode(dpy, gc, ClipByChildren);
	}
}

ringbell ()
{
	XBell(dpy,50);
}

set_clip(r)
Rectangle r;
{
	XRectangle xr;

	xr.x = r.origin.x;
	xr.y = r.origin.y;
	xr.width = r.corner.x - r.origin.x;
	xr.height = r.corner.y - r.origin.y;
	XSetClipRectangles(dpy, gc, r.origin.x, r.origin.y, &xr, 1, Unsorted);
}

unset_clip()
{
	set_clip(Drect);
}

#endif /*X11*/
