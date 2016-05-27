/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)lens.c	1.1.1.3	(3/16/88)";

#include <dmd.h>
#ifdef DMD630
#	define Jcursinhibit Cursinhibit
#	define Jcursallow   Cursallow
#	include <5620.h>
#	define texture16 texture
#else
#	define Jcursinhibit
#	define Jcursallow
#	include <pandora.h>
#endif

Point ofac={1, 1},fac={2, 2};
Bitmap *save,*osave;
Point diag={50, 50};
int fakebttn1 = 0;
static short cached = 0;

Texture16 glass = {
	 0x7FFE, 0x8001, 0x91C1, 0xB365,
	 0xB261, 0xB0C5, 0xA181, 0xA32D,
	 0xB7E5, 0x8001, 0x7FFE, 0x001C,
	 0x66EE, 0xCA67, 0x4423, 0xEEC1,
};

Texture16 sunset = {
	 0x5006, 0xA819, 0x00A0, 0x04A0,
	 0x049F, 0x12A4, 0x0808, 0x03E0,
	 0x2412, 0x0808, 0x0808, 0x3FFF,
	 0x3C1F, 0x7E7E, 0x783E, 0xFCFC,
};

main(argc, argv)
	int argc;
	char *argv[];
{
	register got;

#ifdef DMD630
	local();

	while (--argc > 0 && **(++argv) == '-')
		switch (*(++(*argv))) {
		case 'c':
			cached = 1;
			cache ("lens", 0);
			break;
		}
#endif
	P->state|=RESHAPED;
	request(MOUSE);
	cursswitch(&glass);
	alarm(1);
	for(;;){
		got=wait(MOUSE|ALARM);
		if(P->state&RESHAPED)
			showoff(&glass);
		else if (got&MOUSE)
			if (bttn3()) {
				request(0);
				sleep(1);
				request(MOUSE);
				cursswitch(&glass);
			} else if (bttn2())
				runmenu(0);
			else if (bttn1() || fakebttn1)
				track();
		alarm(60);
	}
}

showoff(text)
Texture16 *text;
{
	Bitmap *b=balloc(Rect(0, 0, 16, 16));
	Point fac,size;
	rectf(&display,Drect,F_CLR);
	texture16(b, b->rect, text, F_STORE);
	size = sub(Drect.corner,Drect.origin);
	fac=div(sub(size,Pt(2*12,2*12)),16);
	magnify(b, b->rect, &display,
		add(Drect.origin,div(sub(size,mul(fac,16)),2)), fac);
	bfree(b);
	b = 0;
	P->state&=~RESHAPED;
}

int saving;
Rectangle osrect, os;

#define RUNSTOP	0
#define BIGGER	1
#define SMALLER	2
#define VIEWER	3
#define PAUSE	4
#define EXIT	5

char *lenstext[] = {"","bigger","smaller","new viewer","","",NULL};	

Menu lensmenu = { lenstext };

runmenu(tracking)
int tracking;
{
	Point ndiag;
	Rectangle r;

	if (tracking) {
		Jcursallow();
		lenstext[RUNSTOP] = "stop";
	} else
		lenstext[RUNSTOP] = "go";
	if (tracking && saving) {
		lenstext[PAUSE] = "pause";
		lenstext[EXIT] = "exit";
	} else {
		lenstext[PAUSE] = "exit";
		lenstext[EXIT] = NULL;
	}
	bfree(save);
	save = 0;
	switch(menuhit(&lensmenu,2)) {
		case RUNSTOP:
			if (tracking)
				undraw();
			fakebttn1 = 1;
			break;
		case BIGGER:
			if (tracking)
				undraw();
			fac=add(fac, ofac);
			/* Don't allow magnification to grow too large. */
			if (fac.x > 13) {
				fac=sub(fac, ofac);
			}
			else {
				ofac=sub(fac, ofac);
			}
			break;
		case SMALLER:
			if (tracking)
				undraw();
			ofac=sub(fac, ofac);
			fac=sub(fac, ofac);
			if(ofac.x < 1)
				ofac.x=ofac.y=1;
			break;
		case VIEWER:
			r = getrect();
			if (tracking)
				undraw();
			ndiag = sub(r.corner, r.origin);
			if ((ndiag.x > 8) && (ndiag.y > 8))
				diag = ndiag;
			do; while (bttn123());
			break;
		case PAUSE:
			if (tracking && saving) {
				while (!bttn123())
					wait(MOUSE);
				undraw();
				do; while (bttn123());
				break;
			} /* else fall through */
		case EXIT:
			if (tracking)
				undraw();

			if (!cached) {
				cursswitch(&sunset);
				showoff(&sunset);
				while (!bttn123())
					wait(MOUSE);
				while (bttn3())
					;
				if (bttn12()) {
					while (bttn123())
						wait(MOUSE);
					cursswitch(&glass);
					showoff(&glass);
					break;
				}
			}

			bfree(osave);
			osave = 0;
			sendchar('\n');
#			ifdef DMD630
				delete();
#			else
				exit();
#			endif /* DMD630 */
		default:
			if (tracking)
				undraw();	
	}
	if (tracking) {
		Jcursinhibit();
		new();
	}
}	

track(){
	fakebttn1 = 0;
	Jcursinhibit();
	new();
	do; while(bttn1());
	while(!bttn1() && !fakebttn1){
		draw();
		if(bttn2()){
			runmenu(1);
		}
	}
	undraw();
	fakebttn1 = 0;
	do; while(bttn123());
	bfree(osave);
	osave=0;
	bfree(save);
	save=0;
	cursswitch(&glass);
	Jcursallow();
}
#define QUD_MASK	014
#define QLR_MASK	003
#define	QRIGHT		001
#define	QLEFT		002
#define	QUP		004
#define	QDOWN		010
#define	QLEFT_MARGIN	XMAX/3
#define	QRIGHT_MARGIN	XMAX*2/3
#define	QUP_MARGIN	YMAX/3
#define	QDOWN_MARGIN	YMAX*2/3

newquad(x, y, quad)
	register x, y, quad;
{
	int ud, lr;
	ud=quad&QUD_MASK;
	lr=quad&QLR_MASK;
	if(x < QLEFT_MARGIN)
		lr=QLEFT;
	if(x > QRIGHT_MARGIN)
		lr=QRIGHT;
	if(y < QUP_MARGIN)
		ud=QUP;
	if(y > QDOWN_MARGIN)
		ud=QDOWN;
	return ud|lr;
}
Rectangle
bigrect(s)
	Rectangle s;
{
	static quad=QUP|QLEFT;
	Rectangle r;
	quad=newquad(s.origin.x, s.origin.y, quad);
	if(quad&QDOWN) {
		r.corner.y=s.origin.y;
		r.origin.y=r.corner.y-fac.y*diag.y;
	}
	else if (quad&QUP) {
		r.origin.y=s.corner.y;
		r.corner.y=r.origin.y+fac.y*diag.y;
	}
	if(quad&QLEFT) {
		r.origin.x=s.corner.x;
		r.corner.x=r.origin.x+fac.x*diag.x;
	}
	else if (quad&QRIGHT) {
		r.corner.x=s.origin.x;
		r.origin.x=r.corner.x-fac.x*diag.x;
	}
	if(r.origin.y<0){
		r.corner.y-=r.origin.y;
		r.origin.y=0;
	}else if(r.corner.y>=YMAX){
		r.origin.y-=r.corner.y-YMAX;
		r.corner.y=YMAX;
	}
	if(r.origin.x<0){
		r.corner.x-=r.origin.x;
		r.origin.x=0;
	}else if(r.corner.x>=XMAX){
		r.origin.x-=r.corner.x-XMAX;
		r.corner.x=XMAX;
	}
	return r;
}

outLine(r)
Rectangle r;
{
	rectf(&physical, Rect(r.origin.x,r.origin.y,r.corner.x-1,r.origin.y+1), F_XOR);
	rectf(&physical, Rect(r.origin.x+1,r.corner.y-1,r.corner.x,r.corner.y), F_XOR);
	rectf(&physical, Rect(r.corner.x-1,r.origin.y,r.corner.x,r.corner.y-1), F_XOR);
	rectf(&physical, Rect(r.origin.x,r.origin.y+1,r.origin.x+1,r.corner.y), F_XOR);
}

Point corres(q, s, r, fac)
/* find the corresponding point to q when mapping from s to r, blowing up by fac */
Point q,fac;
Rectangle s,r;
{
	q = sub(q,s.origin);
	q.x *= fac.x;
	q.y *= fac.y;
	q = add(q,r.origin);
	return q;
}

Rectangle correct(q, s, r, fac)
Rectangle q,s,r;
Point fac;
{
	q.origin = corres(q.origin,s,r,fac);
	q.corner = corres(q.corner,s,r,fac);
	return q;
}

#define HORIZONTAL	1
#define VERTICAL	0
#define MININT		-32000	
#define MAXINT		32000
	
Rectangle makerect(ox, oy, cx, cy) {
	Rectangle trect;

	trect.origin.x = ox;
	trect.origin.y = oy;
	trect.corner.x = cx;
	trect.corner.y = cy;
	return trect;
}

int partition(pr0, s, pr1, pr2, dir)
Rectangle *pr0,s,*pr1,*pr2;
/* pr0 must be a rectangle which is smaller than s */
int dir;
{
	Rectangle a1, a2, b1, b2;
	register Point so, sc;
	so=s.origin;
	sc=s.corner;
	*pr1 = *pr2 = *pr0;
	if (rectclip(pr0,s)) {
		if (dir == VERTICAL) {
			b1 = makerect(MININT,MININT,so.x,MAXINT);
			b2 = makerect(sc.x,MININT,MAXINT,MAXINT);
			a1 = makerect(so.x,MININT,sc.x,so.y);
			a2 = makerect(so.x,sc.y,sc.x,MAXINT);
		} else {
			b1 = makerect(MININT,MININT,MAXINT,so.y);
			b2 = makerect(MININT,sc.y,MAXINT,MAXINT);
			a1 = makerect(MININT,so.y,so.x,sc.y);
			a2 = makerect(sc.x,so.y,MAXINT,sc.y);
		}
		if (!rectclip(pr1,a1) && !rectclip(pr1,a2))
			pr1->corner = pr1->origin;
		if (!rectclip(pr2,b1) && !rectclip(pr2,b2))
			pr2->corner = pr2->origin;
		return 1;
	} else
		return 0;
}

hardcase(r,s)
Rectangle r,s;
{
	Rectangle q, oq, r0, r1, r2, q1, q2, ap, bp, oap, obp;
	Bitmap *bitemp;
	Point qp;
	int fax,fay;
/* 0: Figure out what part of the old expansion is still worthwhile */
	oq = s;
	partition(&oq, os, &q1, &q2, VERTICAL);
	q = correct(oq, s, r, fac);
	oq = correct(oq, os, osrect, fac);
	oap = correct(ap = inset(os,1), os, osrect, fac);
	obp = correct(bp = inset(s,1), os, osrect, fac);
	ap = correct(ap, s, r, fac);
	bp = correct(bp, s, r, fac);
/* 0.5: get rid of the outlines */
	outLine(os);
/* 1: save away the outside */
	r0 = r;
	if (partition(&r0, osrect, &r1, &r2, HORIZONTAL)) {
		bitblt(&physical, r1, save, sub(r1.origin,r.origin), F_STORE);
		bitblt(&physical, r2, save, sub(r2.origin,r.origin), F_STORE);
		bitblt(osave, rsubp(r0,osrect.origin), save,
		       sub(r0.origin,r.origin), F_STORE);
	} else
		bitblt(&physical, r, save, save->rect.origin, F_STORE);
/* 2: move the common piece to its new location, correcting outlines */
	if (rectclip(&obp,osrect)) {
		partition(&obp,oap,&r1,&r2, HORIZONTAL);
		rectf(&physical, r1, F_XOR);
		rectf(&physical, r2, F_XOR);
	}
	bitblt(&physical, oq, &physical, q.origin, F_STORE);
	if (rectclip(&ap,r)) {
		partition(&ap,bp,&r1,&r2, HORIZONTAL);
		rectf(&physical, r1, F_XOR);
		rectf(&physical, r2, F_XOR);
	}
/* 3: restore the outside of the old part */
	r0 = osrect;
	if (partition(&r0, r, &r1, &r2, HORIZONTAL)) {
		bitblt(osave, rsubp(r1,osrect.origin), &physical, r1.origin, F_STORE);
		bitblt(osave, rsubp(r2,osrect.origin), &physical, r2.origin, F_STORE);
	} else
		bitblt(osave, osave->rect, &physical, osrect.origin, F_STORE);
	bitemp = save;
	save = osave;
	osave = bitemp;
/* 4: expand the residue */
	outLine(s);
	qp = corres(q1.origin,s,r,fac);
	nmagnify(&physical, q1, save, &physical, qp, fac, F_STORE);
	qp = corres(q2.origin,s,r,fac);
	nmagnify(&physical, q2, save, &physical, qp, fac, F_STORE);
}

draw(){
	Point old;
	Rectangle r, s;
	old=sub (mouse.xy,Pt(8,8));
	s.origin.x=min(old.x, XMAX-diag.x);
	s.origin.y=min(old.y, YMAX-diag.y);
	s.corner=add(s.origin, diag);
	r=bigrect(s);
	if(!save)
		new();
	if(osave){
		if (saving &&
		   (rectXrect(os,osrect) || rectXrect(s,r) || !rectXrect(s,os)))
			undraw();
		if (saving)
			hardcase(r,s);
		else {
			bitblt(&physical, r, osave, osave->rect.origin, F_STORE);
			outLine(s);
			nmagnify(&physical, s, save, &physical, r.origin, fac, F_STORE);
		}
		do; while(!bttn12() && eqpt(sub(mouse.xy, Pt(8,8)), old));
		saving = 1;
	} else {
		outLine(s);
		nmagnify(&physical,s,save,(Bitmap *)0,save->rect.origin,fac,F_XOR);
		screenswap(save, save->rect, r);
		do; while(!bttn12() && eqpt(sub(mouse.xy, Pt(8,8)), old));
		bitblt(save, save->rect, &physical, r.origin, F_STORE);
		outLine(s);
		saving = 0;
	}
	os = s;
	osrect = r;
}

undraw(){
	if (saving){
		outLine(os);
		bitblt(osave, osave->rect, &physical, osrect.origin, F_STORE);
	}
	saving = 0;
}

new(){
	Point cr;
	bfree(save);
	bfree(osave);
	for (;;) {
		cr.x = diag.x*fac.x;
		cr.y = diag.y*fac.y;
		save=balloc(Rect(0, 0, diag.x*fac.x, diag.y*fac.y));
		if (save==0 || cr.x > XMAX || cr.y > YMAX){
			ofac=sub(fac, ofac);
			fac=sub(fac, ofac);
			if(ofac.x < 1){
				ofac.x=ofac.y=1;
				Jcursallow();
				sendchar('\n');
				exit();
			}
		}
		else {
			break;
		}
	}
	osave=balloc(Rect(0, 0, diag.x*fac.x, diag.y*fac.y));
}
