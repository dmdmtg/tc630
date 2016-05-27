/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)jerqmon.c	1.1.1.3	(3/16/88)";

#include <dmd.h>
#include <font.h>
#ifdef DMD630
#	include <5620.h>
#	include <object.h>
#endif

/* user nice sys queue idle */
int vec[5];

#ifdef DMD630
	Texture16 *txt[5];
#else
	Texture *txt[5];
#endif

char time[30];
char mail[80];
int	size=100;
Rectangle rect, new, old;
Point pt[4];
static char host [] = {"H1 "};
static short hostXposition;
static short timeXposition;
static short mailXposition;


main(argc, argv)
	int argc;
	char *argv[];
{
	register i, now;
	int oldvec[5];

	while (--argc > 0 && **(++argv) == '-')
		switch (*(++(*argv))) {
#			ifdef DMD630
		case 'c':
				cache ((char *)0, A_NO_SHOW);
				break;
#			endif /* DMD630 */
		}

#	ifdef DMD630
		host[1] += whathost ();
#	endif /* DMD630 */

	/*
	 * these are initialized here because the compiler doesn't
	 * have enough smarts to figure out that that it has all
	 * the information that it needs to initialize them at
	 * compile time (because the macros go indirect on a pointer,
	 * but only the indirect address is really required).
	 */
	txt[0] = &T_black;
	txt[1] = &T_lightgrey;
	txt[2] = &T_darkgrey;
	txt[3] = &T_grey;
	txt[4] = &T_white;

	request(RCV|MOUSE);
reshaped:
	rectf(&display, Drect, F_CLR);
	rect=Drect;
	rect.corner.y=rect.origin.y+16+4;
	rect=inset(rect, 4);
	size=rect.corner.x-rect.origin.x;
	rectf(&display, inset(rect, -2), F_OR);
	rectf(&display, rect, F_CLR);
	for(i=0; i<4; i++)
		vec[i]=oldvec[i]=0;
	vec[4]=oldvec[4]=size; /* assumes txt[4]==black */
	pt[0].y=pt[2].y=rect.origin.y;
	pt[1].y=pt[3].y=rect.corner.y;

	hostXposition = rect.origin.x;
	timeXposition = rect.origin.x + 3*defont.info[' '].width;
	mailXposition = rect.origin.x + 22*defont.info[' '].width;

	string(&defont, host, &display,
		Pt(hostXposition, rect.corner.y+11), F_XOR);

	drawtime();
	drawmail();
	for(;;sleep(3)){
		i=own();
		if(i&RCV)
			get();
		if((i&MOUSE) && mail[0]){
			drawmail();
			mail[0]=0;
			drawmail();
		}
		new=old=rect;
		for(i=0; i<5; i++){
			now=relax(oldvec[i], vec[i]);
			old.corner.x=oldvec[i]+old.origin.x;
			new.corner.x=now+new.origin.x;
			sort(old.origin.x,old.corner.x, new.origin.x,new.corner.x);
			texture(&display, Rpt(pt[0], pt[1]), txt[i], F_XOR);
			texture(&display, Rpt(pt[2], pt[3]), txt[i], F_XOR);
			new.origin.x=new.corner.x;
			old.origin.x=old.corner.x;
			oldvec[i]=now;
		}
		if(P->state&RESHAPED){
			P->state&=~RESHAPED;
			goto reshaped;
		}
	}
}
relax(o, n)
	int o, n;
{
	register a;
	register s=1;
	if(o>n)
		s= -1;
	a=abs(o-n);
	if(a<2)
		return n;
	if(a<63)
		return o+s;
	return (31*o+n)/32;
}
sort(x)
	int x;
{
	register int *p= &x;
	register i,j;
	register t;
	for(i=0; i<3; i++){
		for(j=0; j<3; j++)
			if(p[j]>p[j+1]){
				t=p[j];
				p[j]=p[j+1];
				p[j+1]=t;
			}
	}
	for(i=0; i<4; i++)
		pt[i].x=p[i];
}
drawtime()
{
	string(&defont, time, &display, Pt(timeXposition, rect.corner.y+11), F_XOR);
}
drawmail()
{
	string(&defont, mail, &display,
		Pt(mailXposition, rect.corner.y+11), F_XOR);
}
get(){
	register i, sum, nsum, c;
	int readbuf[5];
loop:
	for(sum=0,i=0; i<5; i++)
		sum+=readbuf[i]=getchar();
	switch(c=getchar()){
	default:
		do; while(getchar()!='\n');
		goto loop;
	case 'T':
		drawtime();
		gettime(readbuf);
		drawtime();
		break;
	case 'M':
		drawmail();
		getmail(readbuf);
		drawmail();
		break;
	case '\n':
		for(nsum=0,i=0; i<4; i++)
			nsum+=vec[i]=((long)(readbuf[i])*size) / sum;
		vec[4]=size-nsum;
		break;
	}
}
getchar(){
	register c;
	if((c=rcvchar())==-1)
		wait(RCV), c=rcvchar();
	return c&0xFF;
}
gettime(s)
	int *s;
{
	register char *p=time;
	register i;
	for(i=0; i<5; i++)
		*p++ = *s++;
	while((*p=getchar()) != '\n')
		p++;
	*p=0;
}
getmail(s)
	int *s;
{
	register char *p=mail;
	register i;
	for(i=0; i<5; i++)
		if((*p++ = *s++) == '\n')
		{
			p--;
			goto out;
		}
	while((*p=getchar()) != '\n')
		p++;
out:
	*p=0;
	ringbell();
}
