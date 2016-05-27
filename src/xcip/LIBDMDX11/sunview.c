#ifdef	SUNTOOLS
#include "jerq.h"
#include <fcntl.h>
#include <signal.h>
#include <suntool/fullscreen.h>
#include <sundev/kbd.h>
#include <ctype.h>
#include <stdio.h>

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
static		Jlocklevel;

static short arrow_bits[] = {
	0x0000, 0x0008, 0x001c, 0x003e,
	0x007c, 0x00f8, 0x41f0, 0x43e0,
	0x67c0, 0x6f80, 0x7f00, 0x7e00,
	0x7c00, 0x7f00, 0x7fc0, 0x0000
};

static short arrow_mask_bits[] = {
	0x0008, 0x001c, 0x003e, 0x007f,
	0x00fe, 0x41fc, 0xe3f8, 0xe7f0,
	0xffe0, 0xffc0, 0xff80, 0xff00,
	0xff00, 0xffc0, 0xffe0, 0xffc0
};

/* Sunview only */
Pixwin		*displaypw;
int		damagedone;
static struct	fullscreen *Jfscp;
static	int	screendepth;

#define MAXKEY 64
#define KEYOFF KEY_LEFTFIRST
static char	*kcbuf[MAXKEY];
static char	kctype[MAXKEY];
static char	kbufstr[512];
static char	*kbufp = kbufstr;
#define	event_is_key(event)	\
   ((event_id(event) >= KEY_LEFTFIRST) && (event_id(event) <= KEY_BOTTOMRIGHT))

initdisplay (argc, argv)
int argc;
char **argv;
{
	extern char *getenv();
	extern struct pixrectops mem_ops;
	int gfx;
	int designee;
	struct inputmask mask;
	struct rect gfxrect;
	static winch_catcher();

	signal(SIGWINCH, winch_catcher);
	gfx = open(getenv("WINDOW_GFX"), 0);
	if(gfx < 0){
		perror("cannot get graphics window");
		exit(1);
	}
	designee = win_nametonumber(getenv("WINDOW_GFX"));
	displayfd = win_getnewwindow();
	win_insertblanket(displayfd, gfx);
	close(gfx);
	defont = pf_default();
	displaypw = pw_open(displayfd);
	win_getrect(displayfd, &gfxrect);
	screendepth = displaypw->pw_pixrect->pr_depth;
	if(!(displaypw->pw_prretained =
		mem_create(gfxrect.r_width, gfxrect.r_height, 1)))
                	perror("initdisplay: mem_create");
	display.dr = (char *)displaypw;
	Drect.origin.x = Drect.origin.y = 0;
	Drect.corner.x = gfxrect.r_width;
	Drect.corner.y = gfxrect.r_height;
	display.rect = Drect;
	P = &sP;
	input_imnull(&mask);
	if(mouse_alive)
		win_setinputcodebit(&mask, LOC_MOVE);
	win_setinputcodebit(&mask, MS_LEFT);
	win_setinputcodebit(&mask, MS_MIDDLE);
	win_setinputcodebit(&mask, MS_RIGHT);
	win_setinputcodebit(&mask, LOC_DRAG);
	win_setinputcodebit(&mask, LOC_WINENTER);
	win_setinputcodebit(&mask, LOC_WINEXIT);
	ttyswparse(&mask);		/* Parse $HOME/ttyswrc */
	mask.im_flags |= IM_NEGEVENT;
	mask.im_flags |= IM_ASCII;
	win_setinputmask(displayfd, &mask, 0, designee);
	normalcursor = ToCursor(arrow_bits, arrow_mask_bits, 1, 15);
	cursswitch(&normalcursor);
	rectf(&display, Drect, F_CLR);
	Joffset.x = 0;
	Joffset.y = 0;
	P->state |= RESHAPED;
	if (damagedone)
		fixdamage();
}

/*
 *	Catch SIGWINCH signal when window is damaged
 */
static
winch_catcher ()
{
	signal(SIGWINCH, winch_catcher);	/* for /usr/5bin/cc */
	damagedone = 1;
}

fixdamage()
{
	struct rect gfxrect;
	int x, y;
	
	damagedone = 0;
	pw_damaged(displaypw);
	win_getrect(displayfd, &gfxrect);
	if (Drect.corner.x != gfxrect.r_width ||
	    Drect.corner.y != gfxrect.r_height) {
		Drect.corner.x = gfxrect.r_width;
		Drect.corner.y = gfxrect.r_height;
		display.rect = Drect;
		P->state |= RESHAPED;
		pw_donedamaged(displaypw);
		pr_destroy(displaypw->pw_prretained);
		displaypw->pw_prretained = 0;
		rectf(&display, Drect, F_CLR);
		displaypw->pw_prretained =
		   mem_create(gfxrect.r_width, gfxrect.r_height, 1);
	}
	else {
		pw_repairretained(displaypw);
		pw_donedamaged(displaypw);
	}
}

/* This must be called before initdisplay */
mousemotion ()
{
	mouse_alive = 1;
}

setsizehints ()	{}

request(what)
int what;
{
	if(what & SEND)
		fcntl(1, F_SETFL, FNDELAY);
	if(!(what & RCV))
		jerqrcvmask = 0;
}

Bitmap *
balloc (r)
Rectangle r;
{
	Bitmap *b;

	b = (Bitmap *)malloc(sizeof (struct Bitmap));
	b->dr = (char *)mem_create(r.cor.x - r.org.x, r.cor.y - r.org.y, 1);
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
		pr_destroy((Pixrect *)b->dr);
		free((char *)b);
	}
}

Font
getfont(s)
char *s;
{
	return(pf_open(s));
}

Point
string (f, s, b, p, c)
Font *f;
char *s;
Bitmap *b;
Point p;
Code c;
{
	if(b->flag & BI_OFFSCREEN)
		p = sub(p, b->rect.origin);
	if(b->flag & BI_OFFSCREEN){
		struct pr_prpos where;
		where.pr = (Pixrect *)b->dr;
		where.pos.x = p.x;
		where.pos.y = p.y - (*f)->pf_char['A'].pc_home.y;
		pf_text(where, c, *f, s);
	}
	else
		pw_text((Pixwin *)b->dr, p.x,
			p.y - (*f)->pf_char['A'].pc_home.y,
			c, *f, s);
	return(add(p, Pt(strwidth(f,s),0)));
}

int
strwidth (f, s)
Font *f;
register char *s;
{
	struct pr_size size;
	size = pf_textwidth(strlen(s), *f, s);
	return(size.x);
}

#ifdef sun386
Texture
ToTexture(bits)
register short *bits;
{
	short *mybits = (short *)malloc((unsigned)16*sizeof(short));
	register short *to = mybits;
	extern struct pixrectops mem_ops;
	static struct mpr_data d = 
		{mpr_linebytes(16,1), (short *)0, {0, 0}, 0, 0};
	static struct pixrect pix = {&mem_ops, 16, 16, 1, (caddr_t)&d};

	while(to < &mybits[16])
		*to++ = *bits++;
	d.md_image = mybits;
	d.md_flags &= ~MP_I386;	/* turn off "already modified" bit */
	d.md_flags |= MP_STATIC;/* turn on static bit */
	pr_flip(&pix);
	return mybits;
}
#endif

Cursor
ToCursor (source, mask, hotx, hoty)
short source[], mask[];
{
	Cursor c;
#ifdef sun386
	c.bits = ToTexture(source);
#else
	c.bits = source;
#endif
	c.hotx = hotx;
	c.hoty = hoty;
	return(c);
}

#define BUTTON1	0x4
#define BUTTON2	0x2
#define BUTTON3	0x1
handleinput ()
{
        struct inputevent ie;
	int more;
	unsigned char c;
	static grabbed;
	static struct timeval tv;
	unsigned int mask;

	for(;;){
		if(input_readevent(displayfd, &ie) == -1){
			perror("input_readevent: ");
			break;
		}
		mouse.xy.x = ie.ie_locx;
		mouse.xy.y = ie.ie_locy;
		mouse.time = ie.ie_time.tv_sec*1000 + ie.ie_time.tv_usec/1000;
		if(event_is_ascii(&ie)){
			c = ie.ie_code;
			kbdread(&c);
			P->state |= KBD;
		}
		else if (event_is_key(&ie)&& event_is_down(&ie)) {
			char *cp;
			cp = kcbuf[ie.ie_code-KEYOFF];
			if (cp) {
				if (kctype[ie.ie_code-KEYOFF])
					rcvbfill(cp, strlen(cp));
				else {
					while (*cp)
						kbdread(cp++);
					P->state |= KBD;
				}
			}
		}
		else if(event_is_button(&ie)){
			switch(ie.ie_code){
			case MS_LEFT:
				if(win_inputnegevent(&ie)){
					mouse.buttons &= ~BUTTON1;
				}
				else{
					mouse.buttons |= BUTTON1;
					if(!grabbed){
						win_grabio(displayfd);
						grabbed = 1;
					}
				}
				break;
			case MS_MIDDLE:
				if(win_inputnegevent(&ie)){
					mouse.buttons &= ~BUTTON2;
				}
				else{
					mouse.buttons |= BUTTON2;
					if(!grabbed){
						win_grabio(displayfd);
						grabbed = 1;
					}
				}
				break;
			case MS_RIGHT:
				if(win_inputnegevent(&ie)){
					mouse.buttons &= ~BUTTON3;
				}
				else{
					mouse.buttons |= BUTTON3;
					if(!grabbed){
						win_grabio(displayfd);
						grabbed = 1;
					}
				}
				break;
			case LOC_WINENTER:
				mousewin=1;
				break;
			case LOC_WINEXIT:
				mousewin=0;
				break;
			}
			if(!button123() && grabbed){
				win_releaseio(displayfd);
				grabbed = 0;
			}
		}
				/* break if there is nothing to read */
		mask = 1 << displayfd;
		if ((more = select(displayfd + 1, &mask, 0, 0, &tv)) == -1){
			perror("handleinput:");
			break;
		}
		if(!more)
			break;	
	}
}

Jscreengrab()
{
	if (!Jlocklevel) {
		if (screendepth == 1) {
			Jfscp = fullscreen_init(displayfd);
			Jfscreen.dr = (char *)Jfscp->fs_pixwin;
			Jfscreen.rect.origin.x = Jfscp->fs_screenrect.r_left;
			Jfscreen.rect.origin.y = Jfscp->fs_screenrect.r_top;
			Jfscreen.rect.corner.x = Jfscp->fs_screenrect.r_left +
		 	 Jfscp->fs_screenrect.r_width;
			Jfscreen.rect.corner.y = Jfscp->fs_screenrect.r_top +
		  	 Jfscp->fs_screenrect.r_height;
		} else
			Jfscreen = display;
	}
	Jlocklevel++;
}

Jscreenrelease()
{
	if (--Jlocklevel <= 0) {
		Jlocklevel = 0;
		if (screendepth == 1)
			fullscreen_destroy(Jfscp);
	}
}

ringbell ()
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	win_bell(displayfd,tv,0);
}

ttyswparse(mask)
struct inputmask *mask;
{
	char fbuf[200];
	FILE *fp;
	char line[100], cmd[30], arg1[30], arg2[100];
	int i,k;
	
	sprintf(fbuf,"%s/.ttyswrc",getenv("HOME"));
	if ((fp = fopen(fbuf,"r")) == (FILE*) NULL) return;
	while (fgets(line, sizeof (line), fp)) {
		if (line[0] == '#') continue;
	/* crude using scanf, but quick and dirty */
		i = sscanf(line, "%[^ \t\n]%*[ \t]%[^ \t\n]%*[ \t]%[^\n]\n",cmd, arg1, arg2);
	
		if ((i == 3) && (strcmp(cmd,"mapi") == 0 || strcmp(cmd,"mapo") == 0 )) {
			k = ttyswrcdecode(arg1);
			if (k < 0) continue;
	
			k -= KEYOFF;
			if (k < 0 || k >= MAXKEY) continue;
			win_setinputcodebit(mask, k+KEYOFF);
			kcbuf[k] = kbufp;
			if (strcmp(cmd,"mapo") == 0) kctype[k] = 1;
			kparse(arg2);
		}
	}
	fclose(fp);
}

kparse(s)
char *s;
{
	int c,i;
	char *dp;
	
	while (c = *s++) {
		switch(c) {
		case '^':
			*kbufp++ = *s++ & 037;
			break;
		case '\\':
			dp = "E\033^^\\\\::n\nr\rt\tb\bf\f";
			c = *s++;
nextc:
			if (*dp++ == c) {
				*kbufp++ = *dp++;
				break;
			}
			dp++;
			if (*dp)
				goto nextc;
			if (isdigit(c)) {
				c -= '0', i = 2;
				do
					c <<= 3, c |= *s++ - '0';
				while (--i && isdigit(*s));
			}
		default:
			*kbufp++ = c;
			break;
		}
	}
	*kbufp++ = 0;
}

ttyswrcdecode( s)
	char *s;
{
	int i;

	if (strcmp(s, "LEFT") == 0)
		return (KEY_BOTTOMLEFT);
	else if (strcmp(s, "RIGHT") == 0)
		return (KEY_BOTTOMRIGHT);
	else if (isdigit(s[1])) {
		i = atoi(&s[1]);
		if (i < 1 || i > 16)
			return (-1);
		switch (s[0]) {
		case 'L':
			if (i == 1 || (i > 4 && i < 11)) {
				return (-1);
			} else
				return (KEY_LEFT(i));
		case 'R':
			return (KEY_RIGHT(i));
		case 'T':
		case 'F':
			return (KEY_TOP(i));
		}
	}
	return (-1);
}
#endif /* SUNTOOLS */
