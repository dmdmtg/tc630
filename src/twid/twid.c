/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)twid.c	1.1.1.3	(3/16/88)";

#include <dmd.h>
#include <dmdio.h>
#include <font.h>
#include "twid.h"
#ifdef DMD630
#	include <5620.h>
#	include <object.h>
#	define texture16 texture
#endif

Menu cmdmenu={
	cmdlist };
Menu stylemenu={
	stylelist };
Menu copymenu={
	copylist };
Menu unixmenu={
	unixlist };
Menu brushmenu={
	brushlist };
Menu txtmenu={
	txtlist };
Menu butmenu={
	butlist };
Menu codemenu={
	codelist };

Rectangle saveScreenmap;
Rectangle maprect;
Word *saveBase;
Bitmap *brushes[NBRSH];
Bitmap *brush;
Bitmap *notbrush;
Bitmap *scratch;
int	curbrush=BPOINT;
int	mode=INKING;
extern int rdbitmap();
short getbits();
int resetlayer();
int CHANGE=0;
extern Texture16 black, grey, checks, stip, pointcur, smallcur, medcur;
extern Texture16 curvecur, disccur, typing,  menucurs,  sunset;

Texture16 *txt[1+NTXT]={
	0, &black, &grey, &checks, &stip };
int ntxt=TSTIPPLE+1;
int nbrsh=BBIG+1;
Texture16 *curtxt;
Texture16 *cursor[]={
	0, &pointcur, &smallcur, &medcur, &black };
Texture16 *curcur;
Rectangle screenrect;	/* drawing area */
Rectangle msgrect;	/* typing and messages */
Code onecode, twocode;	
main(argc, argv)
	int argc;
	char *argv[];
{
	while (--argc > 0 && **(++argv) == '-')
		switch (*(++(*argv))) {
		case 'c':
#		ifdef DMD630
			cache ((char *)0, A_NO_SHOW);
#		endif /* DMD630 */
			break;
		}

	    saveBase=P->layer->base;
	    saveScreenmap=P->layer->rect;
	    screenrect=Drect;
	    if(screenrect.origin.x&WORDMASK){	/* texture16s must align */
		screenrect.origin.x|=WORDMASK;
		    screenrect.origin.x++;
	}
	    screenrect.corner.y-=defont.height;
	    msgrect=Drect;
	    msgrect.origin.y=screenrect.corner.y;
	    rectf(&display, msgrect, F_XOR);
	    request(MOUSE);
	    initbrushes();
	    settxt(TBLACK);
	    select(stylelist, INK);
	    butmf(0);	/* Start with OR, CLR */
	wait(MOUSE);
	    Cursswitch(curcur= &black, curbrush=BBIG);	/* curbrush is never 0! */
	for(;;){
		if(P->state & RESHAPED || P->state & MOVED)
		    resetlayer();
		    do 
			wait(MOUSE);

			    while(!ptinrect(mouse.xy, display.rect));
			    updatecurs();
			    if(bttn1())
			    draw(onecode);
			    if(bttn2())
			    draw(twocode);
			    if(bttn3())
			    menu();
	}
}
resetlayer()
{
	saveBase=P->layer->base;
	    saveScreenmap=P->layer->rect;
	    screenrect=Drect;
	    if(screenrect.origin.x&WORDMASK){ /* texture16s must align */
		screenrect.origin.x|=WORDMASK;
		    screenrect.origin.x++;
	}
	screenrect.corner.y-=defont.height;
	    msgrect=Drect;
	    msgrect.origin.y=screenrect.corner.y;
	    if(P->state & MOVED)
	    rectf(&display, msgrect, F_XOR);

	    rectf(&display, msgrect, F_XOR);
	    P->state &= ~RESHAPED;
	    P->state &= ~MOVED;

}
msg(s)
char *s;
{
	if( P->state & MOVED | P->state & RESHAPED){
		resetlayer();
		    CHANGE=1;
	}
	rectf(&display, msgrect, F_CLR);
	    string(&defont, s, &display, msgrect.origin, F_XOR);
	    rectf(&display, msgrect, F_XOR);
}
char *
type(){
	static char typeline[64];
	    register char *s=typeline;
	    register int t;
	    request(MOUSE|KBD);
	    Cursswitch(&typing, 0);
	    for(t=0;t<64;t++){
		wait(KBD);
		    switch(*s++=kbdchar()){
		case '\r':
			--s;
			    goto out;
		    case '\b':
			s-=2;
			    t--;
			    if(s<typeline)
			    s=typeline;
			    break;
		    case '@':
			s=typeline;
		}
		*s=0;
		    msg(typeline);
	}
	--s;
	    msg("name too long; truncated to 64 chars");
	    goto out;

out:
	    *s=0;
	    request(MOUSE);
	    return s>typeline? typeline : (char *)0;
}
clear()
{
	Rectangle r;
	    userrect(&r);
	    rectf(&display, r, F_CLR);
}

char *
GCalloc(n, p)
int n;
char **p;
{
	while(gcalloc((unsigned long)n, p)==0)
	    timeout();
}
timeout(){
	msg("out of memory; delete a layer and hit middle button");
	    for(;;){
		sleep(30);
		    wait(MOUSE);
		    if(bttn2())
		    /*if(bttn123())*/
		break;
	}
	msg("trying again");
	    buttons(UP);
}
int stylemf(), txtmf(), brshmf(), butmf(), copymf(), unixmf(),clear();
struct menu{
	Menu	*menu;
	    int	(*fnc)();
}menus[]={
	&stylemenu,	stylemf,
	    &txtmenu,	txtmf,
	    &brushmenu,	brshmf,
	    &butmenu,	butmf,
	    &copymenu,	copymf,
	    &unixmenu,	unixmf,

};
menu(){
	int m, b;
		cursallow();
	    Cursswitch((Texture16 *)0, 0);
	    m=menuhit(&cmdmenu, 3);
	    if(m==6){
	    	clear();
		Cursswitch((Texture16 *) 0, 0);
}
	    else if(m>=0){
		Cursswitch(&menucurs, 0);
		    buttons(DOWN);
		    if(bttn3()){
			Cursswitch((Texture16 *) 0, 0);
			    b=menuhit(menus[m].menu, 3);
			    if(b>=0)
			    (*menus[m].fnc)(b);
		} else
			buttons(UP);
	}
	Cursswitch(curcur, (mode<=POINTING)? curbrush : 0);
}
stylemf(b){
	switch(b){
	case INK:
		mode=INKING;
		    curcur=cursor[curbrush];
		    goto sel;
	    case POINT:
		mode=POINTING;
		    curcur=cursor[curbrush];
		    goto sel;
	    case LINE:
		curcur=0;
		    mode=LINING;
		    goto sel;
	    case CURVE:
		curcur= &curvecur;
		    mode=CURVING;
		    goto sel;
	    case DISC:
		curcur= &disccur;
		    mode=DISCING;
sel:
		    select(stylelist, b);
		    break;
	}
}
brshmf(b){
	if(b==NEW)
	    b=newbrsh();
	    if(b>NEW)
	    selectbrush(b);
}
txtmf(t){
	if(t==NEW)
	    t=newtxt();
	    if(t>NEW)
	    settxt(t);
}
butmf(c){
	onecode=codes[c][0];
	    twocode=codes[c][1];
	    select(butlist, c);
}
copymf(c){
	register m;
	    Rectangle r;
	    Point p, q;
	    register Bitmap *b;
	    register char *s;
	    switch(c){
	case COPY:
		Cursswitch(&menucurs, 0);
		    buttons(DOWN);
		    Cursswitch((Texture16 *)0, 0);
		    if(   bttn3() && (m = menuhit(&codemenu,3)) != -1
		    && userrect(&r)) {
			b = Balloc(r);
			    Cursinhibit();
			    bitblt(&display,r,b,r.origin,F_STORE);
			    if (m == F_MOVE)
			    rectf(&display,r,F_CLR);
			    p = sub(r.origin,mouse.xy);
			    while (!bttn3()) {
				q = add(p,mouse.xy);
				    bitblt(b,r,&display,q,F_XOR);
				    bitblt(b,r,&display,q,F_XOR);
			}
			bitblt(b,r,&display,q,((m == F_MOVE) ? F_STORE : m));
			    bfree(b);
			    Cursallow();
		}
		buttons(UP);
		    break;
	    case ROTATE:
		if(userrect(&r)){
			CHANGE=0;
rotagain:
			    if(CHANGE){
				resetlayer();
				    msg("ROTATE CANCELED;layer has been relocated!");
				    CHANGE=0;
				    break;
			}
			Cursinhibit();
			    rectclip(&r,maprect);
			    r = rotate(&display,r);
			    Cursallow();
			    msg("again?");
			    if((s=type()) && *s=='y')
			    goto rotagain;
		}
		msg("");
		    break;




	}
}
unixmf(u){
	register char *s;
	    Rectangle r;
	    char *filetype,*file,filename[64];
	    register m;
	    file= filename;
	    switch(u){
	case READ:
		msg("File name ?"); 
		    filetype = type();
		    strcpy(file, filetype);
		    if(file){

			msg("mode (=, ~, ^, |)?");
			    m=F_STORE;
			    if(s=type()) switch(*s){
			case '=':
			default:
				m=F_STORE;
				    break;
			    case '~':
				m=F_CLR;
				    break;
			    case '|':
				m=F_OR;
				    break;
			    case '^':
				m=F_XOR;
				    break;
			}
			rdbitmap(file,m);
			    Cursallow();
		}
		break;
	    case WRITE:
		msg("File name ?"); 
		    if((s=type()) && 
		    (msg("sweep out which rectangle"), userrect(&r))){
			Cursinhibit();
			    msg("");
			rectf(&display,r,F_XOR);
			    wrbitmap(&display, Drect.origin, r, s);
			    Cursallow();
			    msg("write done");
		}
		break;
	    case EXIT:
		Cursswitch(&sunset, 0);
		    buttons(DOWN);
		    if(bttn3()){
			buttons(UP);
			    Cursswitch((Texture16 *)0, 0);
		    P->layer->base = saveBase;
		    P->layer->rect = saveScreenmap;
			    exit();
		}
		buttons(UP);
	}
}
newbrsh(){
	Rectangle r,newr;
	    char *s;


	    if(nbrsh>=NBRSH+1){
		msg("too many brushes defined");
		    return 0;
	}
	if(userrect(&r)==0)
	    return 0;
	    msg("Brush name ?");
	    s=type();
	    if(s){
		GCalloc(strlen(s)+1, &brushlist[nbrsh]);
		    strcpy(brushlist[nbrsh], s);
		    newr=r;
		    /* 
		np=add(r.origin, r.corner);
		np=div(np,2);
		newr=rsubp(newr, np);
*/
		newr=rsubp(r, div(add(r.origin, r.corner), 2));
		    brushes[nbrsh]=Balloc(newr);
		    Cursinhibit();
		    bitblt(&display, r, brushes[nbrsh], newr.origin, F_STORE);
		    Cursallow();
		    return nbrsh++;
	}
	return 0;
}
Word holdbits[32];/*word changed to short*/
/*  ECC HACK*/
Bitmap hold={
	holdbits, 1, 0, 0, 32, 32 	};/*ECC HACK*/
newtxt(){
	Point p;
	    char *s;
	    register short *w;
	    register  h=0;
	    if(ntxt>=NTXT+1){
		msg("too many textures defined");
		    return 0;
	}
	    msg("Create texture16 by positioning square and pressing button 1 or 2");
		Cursswitch(&black, 0);
	    buttons(DOWN);
	    if(bttn3())
	    return 0;
	    p=mouse.xy;
	    Cursinhibit();
	    bitblt(&display, Rpt(sub(p, Pt(8, 8)), add(p, Pt(24, 24))),
	    &hold, Pt(0, 0), F_STORE);
	    if(bttn2())
	    rectf(&hold, hold.rect, F_XOR);
	    Cursallow();
	    buttons(UP);
	    msg("Texture name?");
	    s=type();
	    if(s){
		GCalloc(strlen(s)+1, &txtlist[ntxt]);
		    strcpy(txtlist[ntxt], s);
		    GCalloc(sizeof(txt[ntxt]->bits), &txt[ntxt]);
		    w=txt[ntxt]->bits;	/*  ECC HACK*/
		for(h=0; h<16; h++)
		    *w++ = ((short) getbits(holdbits[h],32,17));
		    return ntxt++;
	}
	return 0;
}

/* get n number of bits starting at position p from varible x ECC HACK */

short getbits(x,p, n)
unsigned x, p, n;
{
	return((x >> (p+1-n)) & ~(~0 << n-1));
}

/* call getrect() from system to sweep out a rectangle.
 */
userrect(rp)
register Rectangle *rp;
{
	Rectangle fRect();

	Cursswitch((Texture16 *)0, 0);	/* gotta synchronize */
	*rp=getrect();	/* getrect is system call */
	Cursswitch(curcur, (mode<=POINTING)? curbrush : 0);
#ifdef DMD630
	maprect=fRect(Drect.origin.x,Drect.origin.y,Drect.corner.x,Drect.corner.y-defont.height);
#else
	maprect = Drect;
	maprect.corner.y -= defont.height;
#endif
	return rectclip(rp,maprect);
}
select(pp, n)	/* mark menuitem pp[n] with a '*' */
register char **pp;
{
	register char **p;
	    register char *s, *t;
	    /* first deselect */
	for(p=pp; *p; p++)
	    if(*p[0]=='*')
	    for(s= *p, t=(*p)+1; *s++ = *t++; )
	    ;
	    /* now select */
	s=pp[n];
	    s+=strlen(s);	/* s now points at the null */
	t=s+1;
	    while(s>=pp[n])
	    *t-- = *s--;
	    *t='*';
}
Bitmap *
Balloc(r)
Rectangle r;
{
	register Bitmap *b;
	    while((b=balloc(r))==0)
	    timeout();
	    return b;
}
initbrushes()
{
	/*
	 * Sleazy; uses the fact that balloc generates bitmaps with
	 * all the bits on
	 */
	brushes[BPOINT]=Balloc(Rect(0, 0, 1, 1));
	    brushes[BSMALL]=Balloc(Rect(-2, -2, 2, 2));
	    brushes[BMED]=Balloc(Rect(-4, -4, 4, 4));
	    brushes[BBIG]=Balloc(Rect(-8, -8, 8, 8));
	    selectbrush(BBIG);
}
selectbrush(h)
{
	Rectangle r;
	    bfree(notbrush);
	    bfree(scratch);
	    brush=brushes[h];
	    r=brush->rect;
	    notbrush=Balloc(r);
	    bitblt(brush, r, notbrush, r.origin, F_STORE);
	    rectf(notbrush, r, F_XOR);
	    scratch=Balloc(Rect(0, 0, r.corner.x-r.origin.x+WORDSIZE,
	    r.corner.y-r.origin.y+WORDSIZE));
	    select(brushlist, h);
	    curbrush=h;
	    if(mode==POINTING)
	    curcur=cursor[curbrush];
	    else
		stylemf(INK);
}
settxt(t)
{
	curtxt=txt[t];
	    select(txtlist, t);
}
dropbrush(p, c)
Point p;
Code c;
{
	Rectangle br;
	    /* Center of brush is (0,0), but we must compensate for texture16 alignment */
	P->layer->base = addr(P->layer,Drect.origin);
	    P->layer->rect = Drect;
	    P->layer->rect.corner.y-=defont.height;
	    br.origin=add(p, brush->rect.origin);
	    br.origin.x&=WORDMASK;
	    br.origin.y&=WORDMASK;
	    br.corner=add(br.origin, sub(brush->rect.corner, brush->rect.origin));
	    /* Fill the scratch space with the current texture16 */
	texture16(scratch, br, curtxt, F_STORE);
	    /* Clear all bits in scratch which are not on in brush */
	bitblt(notbrush, notbrush->rect, scratch, br.origin, F_CLR);
	    /* Copy scratch to screen */
	Cursinhibit();
	    bitblt(scratch, br, &display, add(brush->rect.origin, p), c);
	    Cursallow();
	    P->layer->base = saveBase;
	    P->layer->rect = saveScreenmap;
}
draw(c)
Code c;
{
	int r=0;
	    Point p, lastp,rp;
	    switch(mode){
	case INKING:
		do{
			P->layer->base = addr(P->layer,Drect.origin);
			    P->layer->rect = Drect;
			    P->layer->rect.corner.y-=defont.height;
			    dropbrush(mouse.xy, c);
			    P->layer->base = saveBase;
			    P->layer->rect = saveScreenmap;
			    wait(CPU);
			    updatecurs();
		}while(bttn12());
		    break;
	    case POINTING:
		dropbrush(mouse.xy, c);
		    buttons(UP);
		    break;
	    case LINING:

		switch(c){
		case 0:
c01:
			lastp=mouse.xy;
			    for(; bttn23()==0; wait(MOUSE)){
				while(bttn1()){
					p=mouse.xy;
					    xsegment(lastp, p, F_XOR);
					    nap(2);
					    xsegment( lastp, p, F_XOR);
				}
				if( P->state & MOVED | P->state & RESHAPED){
					resetlayer();
					    return;								}
				Cursinhibit();
				    xsegment(lastp, p, c);
				    Cursallow();
				    lastp=p;
				    do{
					wait(CPU);
					    if(bttn3())
					    return(buttons(UP));
				}while(bttn123()==0);
			}
			return;
		    case 1:
			goto c01;
		    case 2:
c23:
			lastp=mouse.xy;
			    for(; bttn13()==0; wait(MOUSE)){
				while(bttn2()){
					p=mouse.xy;
					    xsegment(lastp, p, F_XOR);
					    nap(2);
					    xsegment( lastp, p, F_XOR);
				}
				if( P->state & MOVED | P->state & RESHAPED){
					resetlayer();
					    return;								}
				Cursinhibit();
				    xsegment(lastp, p, c);
				    Cursallow();
				    lastp=p;
				    do{
					wait(CPU);
					    if(bttn3())
					    return(buttons(UP));
				}while(bttn123()==0);
			}
			return;
		    case 3:
			goto c23;
		}
	    case CURVING:
		for(lastp=mouse.xy; bttn12(); nap(2)){
			p=mouse.xy;
			    if(!eqpt(p, lastp)){
				Cursinhibit();
				    xsegment(lastp, p, c);
				    Cursallow();
				    lastp=p;
			}
		}
		break;
	case DISCING:
		p=mouse.xy;
		    Cursswitch((Texture16 *)0, 0);
		    while(bttn12()){
			rp=sub(mouse.xy,p);
			    r=norm(rp.x,rp.y,0);
			    discture(&display,p,r,curtxt,F_XOR);
			    nap(2);
			    discture(&display,p,r,curtxt,F_XOR);
			    if(bttn3()){
				do;
					while(bttn3());
					    Cursswitch(&disccur, 0);
					    return;
			}								}		Cursinhibit();
		    discture(&display,p,r,curtxt,c);
		    Cursallow();
		    Cursswitch(&disccur,0);
		    break;
	}
}
buttons(updown)
{
	do 
		wait(MOUSE);
		    while((bttn123()!=0) != updown);
}xsegment (p, q,c)
Point p, q;
Code c;
{
	if(P->state & RESHAPED || P->state & MOVED)
	    resetlayer();
	    P->layer->base = addr(P->layer,Drect.origin);
	    P->layer->rect = Drect;
	    P->layer->rect.corner.y-=defont.height;
	    segment(P->layer , p, q, c);
	    P->layer->base = saveBase;
	    P->layer->rect = saveScreenmap;
}
