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
#include "printer.h"
#include "icons.h"
#include "menus.h"
#include "pfd.h"

#define DMDPSVERS "dmdps version 1.2"

/* 
	This code was based on "blitblt", author unknown.
	It has been mostly rewritten by andy schnable.
	The code to drive the parallel port was provided
	by Alan Kaplan in MH, but I don't think he wrote it...	
	The code for the serial port was provided by
	Tome Rhodes at IW, but I don't think he wrote it...
*/	

extern sendbitmap();	/* encodes bitmap for output */
extern newsendbitmap();	/* encodes bitmap for output using new format */
extern readbitmap();	/* decodes bitmap from input */
extern printbmap();	/* formats bitmap for output */
extern read_pfd();	/* reads pfd structure elements */
extern doUNIXwork();	/* forwards stdin (if present) to the printer port */
extern touchtype();	/* forwards keyboard input (if present) to printer */

char filename[NCHFILE];	/* name of the file for read/write */
char pipewcmnd[NCHPIPE];	/* pipeline for pipe write */
char prname[NCHFILE];	/* ascii file containing pfd */
struct Printerdefs pfd;	/* printer definition structure */

Rectangle kbdrect; 	/* one line display */
Point	kbdp;		/* cursor point in one line display */

Rectangle srects[] = {
	 152, 8, 192, 32, 	/* location of lf button */
	 200, 8, 240, 32,	/* location of ff button */
	 0,   0,  0,   0
};

#define MAXCNT	2000

Bitmap  blit;		/* the screen (in a form we can manipulate) */

Bitmap *bp, *obp;	/* the selected bitmap */
Rectangle rect;		/* the selected rectangle */
struct Proc *proc,*oproc;/* the process selected. (when layer selected.) */
Point refpt;		/* position of view. */

short	status; 	/* keeps state info for revid, stop/run, and stipple */
char 	mustfree = 0;	/* should we free bp when finished with it? */
char 	hold = 0;	/* hold	io flag */
char	flush = 0;	/* flush io flag */
char	makecopy = 1;	/* make copy first flag */
char 	autores = 0;	/* automatic resolution selection on. */
char	dump = 0;	/* generate hex dump of formatted bitmap. */
#ifdef DMD630
char	maplf = 1;	/* map lf to lf/cr */
#else
char	maplf = 0;
#endif
int	in = 0;		/* amount of inset */

#if ! defined(PAR) && ! defined(DMD630)
char 	flow = 1;	/* do we try to regulate flow? */
#endif

#ifdef DMD630
extern cflag;
#endif

/* refresh display when laststate != state. */
char 	laststate = RES;/* the last state. */
char	state = SEL; 	/* the state we are in */	

char	vers;		/* which version terminal ROMS? */

main(argc, argv)
int	argc;
char	*argv[];
{
	int cnt;

	/* a version of the screen that we can mess with. */
	blit.base=addr(&display,Pt(0,0));
	blit.width=display.width;
	blit.rect=Jrect;

	vers = version() & 0x000000ff;
	request(RCV|KBD|MOUSE);
	parse_args(argc,argv);
#ifdef DMD630
	if (cflag)
		cache("dmdps",A_NO_SHOW);
#endif
	redraw(); 		

	sleep(120); /* jx on ibm with DACU FEP won't work without this.. */
	formp(INULL,INULL,INULL,"reading pfd file...");
	if (read_pfd(&pfd,prname)==-1){
		formp(INULL,INULL,INULL,"can't read pfd file.");
		sleep(60);
	}
	thinkstart();	/* initalize printer driver */
	formp(INULL,INULL,INULL,"turn on printer.");
	thinkflush();
	selnewbp();	/* get ready to select a new bitmap */

    	for (;wait(CPU);) {
		Reshape(); /* redraws only if necessary. */
		if (doUNIXwork())	/* download from dmdpr */
			cnt=0;	
		/* reset flushing download flag after download has stopped. */
		if (flush == TRUE && cnt++ < MAXCNT ) {
 			formp(INULL,INULL,INULL,"stopped flushing"); 
			alert();
			laststate = RES;
			flush=FALSE;
			cnt=0;
		}
		touchtype();	/* send kbd input to printer */
		thinkflush();

		if (state == SEL) {
			if (laststate != SEL) { 
				laststate = SEL;
 				formp(INULL,&menu2,&menu3,DMDPSVERS); 
				showbitmap(bp,inset(rect,in));
			}
			if (button3() && own()&MOUSE)
				do_bttn3();
			else if (bttn1() && own()&MOUSE)  
				do_bttn1();
			else if (bttn2() && own()&MOUSE)
				do_misc();
			if (bp) {
				state=OP;
				bttn3_menu[MSELOP] = "";
				oproc=proc;
				obp=bp;
				if (makecopy && !mustfree)
					clone_bitmap();
			}
		}
		if (state==OP) {
			if (laststate != OP) { 
			    	laststate=OP ;
			    	formp(&cross,&menu2,&menu3,"operations");
			    	showbitmap(bp,inset(rect,in));
			}
			if (button3() && own()&MOUSE) 
				do_bttn3();
			else if (bttn2() && own()&MOUSE)
				do_bttn2();
			else if (bttn1() && own()&MOUSE) 
				do_bttn1();
		}
	} /* main loop */
}

selnewbp()
{
	clearlayer();
	if (bp) {
		restore();
		if (mustfree) {
			bfree(bp);
			mustfree=0;
		}
		laststate=RES;
	}
	bp = BNULL; 
	proc = PNULL; 
	status= 0x00;
	state=SEL; 
	bttn3_menu[MSELOP] = NULL;
}

restore()
{
	if ( status&STIPPLE) {
		texture(bp,rect,&stiptext,F_XOR);
		status ^= STIPPLE;
	}
	if ( status&REVVID) {
		rectf(bp,rect,F_XOR);
		status ^= REVVID;
	}
	if ( status&STOPPED) {
		proc->state &= ~HALTED;
		status &= ~STOPPED;
	}
	in = 0; 				/* set inset to 0 */
}

readbp(in)
FILE *in;
{
	switch  (readbitmap(in)){
	case 0: formp(INULL,INULL,INULL, "no room");
		alert();
		break;
	case EOF:formp(INULL,INULL,INULL, "read failed");
		alert();
		break ;
	case 2: formp(INULL,INULL,INULL, "operation aborted");
		mustfree=2;
		break ;
	case 1: formp(INULL,INULL,INULL, "read complete");
		mustfree=2;
		return;
	}
	selnewbp();
}

doio(cmd,name)
int	cmd;
char	*name;
{
	FILE *filep;
	int rc;

	buttons(UP);
	formp(&thumbsdown,&thumbsdown,&thumbsup,"");
	laststate=RES;
	drstore(kbdrect);
	switch (cmd) {
	case WRITEPIPE:
	case READPIPE:
		kbdp = drstring("| ",LINE(kbdrect));
		break;
	case READFILE:
		kbdp = drstring("< ",LINE(kbdrect));
		break;
	case WRITEFILE:
		kbdp = drstring("> ",LINE(kbdrect));
		break;
	}
	kbdp=drstring(name,kbdp);
	if (!kbdstring(name,132,kbdp)) {
		formp(INULL,INULL,INULL, "operation aborted");
		sleep(30); laststate=RES; return;
	}
	cursswitch(&deadmouse);
	switch (cmd) {
	case READFILE:
		formp(INULL,&menu2,INULL, "reading from UN*X...");
		if ( (filep = fopen(name, "r")) == (FILE *) NULL) {
			formp(INULL,INULL,INULL, "open failed");
			selnewbp();
			alert();
			break;
		} 
		readbp(filep);
		break;
	case WRITEPIPE:
	case WRITEFILE:
		formp(INULL,&menu2,INULL, "sending to UN*X...");
		filep = ((cmd == WRITEPIPE) ? popen(name, "w" ) 
					    : fopen(name, "w"));
		if (filep == (FILE *) NULL) {
			formp(INULL,INULL,INULL, "couldn't create pipe/file");
			alert();
		} else {
			if ( newsendbitmap(bp,inset(rect,in),filep) == 1)
				formp(INULL,INULL,INULL, "output complete");
			else 
				formp(INULL,INULL,INULL, "output aborted");
		}
	}
	if (filep != (FILE *) NULL) {
		fflush(filep); 
		fclose(filep);
	}
 	laststate=RES; sleep(30);
	cursswitch(TNULL);
}

print (place,str)
Rectangle	place;
char *str;
{
	drstore(place);
	drstring(str, LINE(place));
	sleep(5);
}

formp(icon1, icon2, icon3, str) 
Texture16 *icon1;
Texture16 *icon2;
Texture16 *icon3;
char *str;
{
	fill_mouseboard(icon1, icon2, icon3);
	show_mouseboard(Pt(0,0));
	print(kbdrect,str);
}

redraw()
{
	int tx,ty;

	jrectf(Jrect,F_CLR); 
	tx=Drect.origin.x+(Drect.corner.x-Drect.origin.x)/2;
	ty=Drect.origin.y+(Drect.corner.y-Drect.origin.y)/2;
	if ( ty < Drect.origin.y+8) 
		ty=Drect.origin.y+8;
	else if ( ty > Drect.corner.y-8) 
		ty=Drect.corner.y-8;
	if ( tx < Drect.origin.x+8) 
		tx=Drect.origin.x+8;
	else if ( tx > Drect.corner.x-8) 
		tx=Drect.corner.x-8;
	refpt.y=ty; 
	refpt.x=tx;
	kbdrect=Drect;
	kbdrect.origin.y=kbdrect.corner.y-defont.height-4;
	mk_mouseboard(); 
	if (vers<5 && !(own()&MOUSE))
		texture(P->layer,Jrect,&stiptext,F_XOR);
	P->state &= ~RESHAPED;
	laststate=RES;
}

touchtype()
{
	char s[150];
	
	s[0]='\0';
	if ( own()&KBD ) {
#ifdef DMD630
		request(RCV|KBD|MOUSE|PSEND);
#endif
		formp(&thumbsdown,&thumbsdown,&thumbsup,"");
		laststate=RES;
		drstore(kbdrect);
		if ( kbdstring(s,150,LINE(kbdrect))) {
			shipstr(s); 
			if (maplf)
				shipchar('\r');	
			shipchar('\n');	
		}
#ifdef DMD630
		request(RCV|KBD|MOUSE);
#endif
	}
}
	
lexit3()	/* return true if button3 is clicked */
{
	Texture16 *prev; 
	int lexit;

	prev = (Texture16 *)cursswitch((Texture16 *) &thumbsdown);
	lexit = buttons(DOWN);buttons(UP);
	cursswitch(prev);
	return(lexit == 3);
}

buttons(updown)
int updown;
{
	do wait(MOUSE);
	while((bttn123()!=0) != updown || !ptinrect(mouse.xy,Drect));

	switch (bttn123()) {
		case 4: return 1;
		case 2: return 2;
		case 1: return 3;
	}
	return 0;
}

#define echo(ich,p)	bitblt(defont.bits,\
				Rect(ich->x,0,(ich+1)->x,defont.height),\
				&display,Pt(p.x+ich->left,p.y),F_XOR)

#define kbdcurs(p)	rectf(&display,Rect(p.x,p.y,p.x+1,p.y+defont.height),\
				F_XOR)

kbdstring(str,nchmax,p)	/* read string from keyboard with echo at p */
char *str; int nchmax; Point p; /* you pass a starting string in to edit. */
{
	int kbd, nchars; 
	Fontchar *ich;

	nchars=strlen(str); kbdcurs(p);
	for (;;) {
		sleep(3);
		if ( own()&KBD) {
			kbdcurs(p);
			do switch (kbd=kbdchar()) {
			case '\0':
				break;
			case '\r':
			case '\n':
				return nchars;
			case '\b':
				if (nchars <= 0) break;
				kbd=str[--nchars]; str[nchars]=0;
				ich=defont.info+kbd;
				p.x -= ich->width;
				echo(ich,p);
				break;
			default:
				if (nchars >= nchmax) break;
				str[nchars++]=kbd; str[nchars]=0;
				ich=defont.info+kbd;
				echo(ich,p);
				p.x += ich->width;
			} while (own()&KBD);
			kbdcurs(p);
		} else if ( own()&MOUSE && bttn12()) {
			buttons(UP);
			return 0;
		} else if ( own()&MOUSE && button3()) {
			buttons(UP);
			return nchars;
		}
	}
}

doprint(pfd)
struct Printerdefs *pfd;
{
	int returncode;

	cursswitch(&deadmouse);
	formp(INULL,&menu2,INULL, "printing...");
	if ((returncode = printbmap(pfd, bp, inset(rect,in))) == 1) 
		formp(INULL,INULL,INULL, "printing complete");
	else if ( returncode == 0 )
		formp(INULL,INULL,INULL, "printing aborted");
	else if ( returncode == -1 ) {
		formp(INULL,INULL,INULL, "unknown printer type");
		alert();
	}

	laststate=RES; sleep(20);
	cursswitch(TNULL);
	return returncode;
}

sbuthit (rects)
Rectangle rects[];
{
	register i;
	Point mickey;
	Rectangle r;

	mickey=mouse.xy;
	for ( i=0; !eqrect(rects[i],Rect(0,0,0,0)); i++) {
		r.origin=add(Drect.origin,rects[i].origin);
		r.corner=add(Drect.origin,rects[i].corner);
		if ( ptinrect(mickey,r) ) {
			rectf(&display,r,F_XOR);
			buttons(UP);
			rectf(&display,r,F_XOR);
			return i;
		}
	}
	return -1;
}

iocntrlmenu()
{
	int mhit;

	iomenutext[FLUSH][0]= flush?'*':' ';
	iomenutext[HOLD][0]= hold?'*':' ';
	
	if ( own()&MOUSE && bttn2() )	
	switch ( mhit=menuhit(&iocontrol_menu, 2)) {
	case FLUSH: 
		flush=flush?0:1;
		break;
	case HOLD: 
		hold=hold?0:1;
		break;
	}
}

#ifndef PAR
#ifndef DMD630
poptsmenu()
{
	int mhit;
	extern pbits, pspeed;

	do {
		port_opts[M7][0]= (pbits==7)?'*':' ';
		port_opts[M8][0]= (pbits==8)?'*':' ';
		port_opts[M9600][0]= (pspeed==9600)?'*':' ';
		port_opts[M4800][0]= (pspeed==4800)?'*':' ';
		port_opts[M1200][0]= (pspeed==1200)?'*':' ';

		if ( own()&MOUSE && bttn2() )
			switch (mhit=menuhit(&portopts, 2)){
			case M7: pbits=7;
				break;
			case M8: pbits=8;
				break;
			case M9600: pspeed=9600;
				break;
			case M4800: pspeed=4800;
				break;
			case M1200: pspeed=1200;
				break;
			case FLOWTICKS:
			case FLOWCHARS:
				buttons(UP); 
				if ( buttons(DOWN) == 2 )
					flowmenus(mhit,TRUE);
				break;
			}
		else if ( own()&MOUSE && bttn13()) 
			break;
		wait(CPU);
	} while ( mhit != -1 );
}
#endif
#endif

#ifndef PAR
#ifndef DMD630
flowmenus(which_one)
int which_one;
{
	int i, mhit;

	do {
		for (i=0;i<=LASTPOWER;i++) {
			tickmenu[i][0]=(pfd.flowticks&powertwo[i])?'*':' ';
			charsmenu[i][0]=(pfd.flowchars&powertwo[i])?'*':' ';
		}
		if ( which_one ==FLOWTICKS && own()&MOUSE && bttn2() 
		&& ((mhit=menuhit(&tick_menu,2)) != -1 ))
			pfd.flowticks ^= powertwo[mhit];
		else if ( which_one==FLOWCHARS  && own()&MOUSE && bttn2()
		&& ((mhit=menuhit(&chars_menu,2)) != -1 ))
			pfd.flowchars ^= powertwo[mhit];
		else if ( own()&MOUSE && bttn13()) 
			break;
		wait(CPU);
	} while ( mhit != -1 );
}
#endif 
#endif 

do_misc()
{
	int mhit;
	do {
		misc_menu[MONE2ONE]= (autores==0)?MCLIP:MSCALE;
		misc_menu[MCOPY][0]= (makecopy==1)?'*':' ';
		misc_menu[MDUMP][0]= (dump==1)?'*':' ';
		misc_menu[MHOLD][0]= (hold==1)?'*':' ';
		misc_menu[MFLUSH][0]= (flush==1)?'*':' ';
		misc_menu[MMAP][0]= (maplf==1)?'*':' ';

		if ( own()&MOUSE && bttn2())
			switch ( mhit=menuhit(&miscmenu, 2) ) {
			case MONE2ONE:
				autores=autores?0:1;
				break;
			case MCOPY:
				makecopy=makecopy?0:1;
				break;
			case MDUMP:
				dump=dump?0:1;
				break;
			case MEXIT:
				mybye();
				return;
			case MMAP:
				maplf=maplf?0:1;
				break;
			case MHOLD:
				hold=hold?0:1;
				break;
#ifndef PAR
#ifndef DMD630
			case MPOPTS:
				poptsmenu();
				break;
#endif
#endif
			case MFLUSH:
				flush=flush?0:1;
				break;
			case MREADPFD:
				formp(&thumbsdown,&thumbsdown,&thumbsup, "");
				laststate=RES;
				drstore(kbdrect);
				kbdp=drstring(prname,LINE(kbdrect));
				if (kbdstring(prname,NCHFILE,kbdp)) {
				    formp(INULL,INULL,INULL,"reading pfd...");
				    if (read_pfd(&pfd,prname)==-1){
					formp(INULL,INULL,INULL, 
						"couldn't read pfd file");
					alert();
				    } else {
				    	formp(INULL,INULL,INULL, "pfd read ok");
					sleep(20);
				    }
				} 
				return;
			}
		else if ( own()&MOUSE && bttn13()) 
			break;
		wait(CPU);
	} while (mhit != -1);
}

misc()
{
	int mhit;
	buttons(UP);
	formp(INULL,&menu2,INULL, "set options...");
	laststate=RES;
	if ( buttons(DOWN) == 2 ) do {
		if ( own()&MOUSE && bttn2() ) {
			mhit=do_misc();
			formp(INULL,&menu2,INULL, "set options...");
		}
		if ( own()&MOUSE && bttn13() ) {
			buttons(UP);
			break;
		}
		wait(CPU);
	} while (mhit != -1);
}

mybye()
{
	formp(&thumbsup,&thumbsup,&thumbsdown, "quit?");
	if (lexit3()) {
		thinkflush();
		thinkstop();
		exit(0);
	}
	laststate=RES;
} 

do_bttn2()
{
	int mhit;

	bit_output[MPRINT]=NULL;
	if ( prname[0] != '\0' ) 
		bit_output[MPRINT]="print bitmap";

	switch (mhit=menuhit(&bitoutput,2)) {
	case MWRITE: 
	case MPIPE: 
		doio(mhit==MPIPE ?WRITEPIPE:WRITEFILE,
		     mhit==MPIPE ?pipewcmnd:filename);
		break;
	case MPRINT: 
#ifdef DMD630
		request(RCV|KBD|MOUSE|PSEND);
#endif
		doprint(&pfd);
#ifdef DMD630
		request(RCV|KBD|MOUSE);
#endif
		break;
	default: state=OP;
	}
}

do_bttn3()
{
	int mhit;

	switch (mhit = menuhit(&bttn3menu,3)) {
	case MSEL_LAYER:	/* choose layers */
		formp(INULL,INULL,&target, "select a layer");
		selnewbp();
		if ((proc=debug()) != PNULL){ 
			bp=(Bitmap *)proc->layer; 
			rect=bp->rect;
			flash(bp,rect);
		}  else
			selnewbp();
		cursswitch(TNULL); /* debug is hosed */
		break;
	case MSEL_RECT: /* sweep out a rectangle */
		formp(INULL,INULL,&sweepcursor, "sweep a rectangle");
		selnewbp();
		bp= &blit; 
		rect=getrect();
		if ((rect.corner.x-rect.origin.x)<XMIN
		&&  (rect.corner.y-rect.origin.y)<YMIN)
			selnewbp();
		else
			flash(bp,rect);
		break;
	case MSEL_SCREEN:	/* whole screen */
		selnewbp();
		bp= &blit; 
		rect=Jrect;
		flash(bp,rect);
		break;
	case MSEL_FILE: 
		selnewbp();
		doio(READFILE,filename);
		rect=bp->rect;
		break;
	case MINSET: 
		flash(bp,inset(rect,in += 4));
		clearlayer();
		break;
	case MFLIPSTIP:	
		texture(bp,rect,&stiptext,F_XOR);
		status ^= STIPPLE;
		break; /* op */
	case MFLIPVID:	
		rectf(bp,rect,F_XOR);
		status ^= REVVID;
		break; /* op */
	case MCOPYOP:
		clone_bitmap();
		break;
	case MRUNHALT: 
		if ( (proc->state)&HALTED ){
			proc->state &= ~HALTED;
			status &= ~STOPPED;
		} else {
			proc->state |= HALTED;
			status |= STOPPED;
		}
		break; 
	case MISC:
		misc();
		break;
	} /* end switch */
	laststate=RES;
}

do_bttn1()
{
#ifdef DMD630
	request(RCV|KBD|MOUSE|PSEND);
#endif
	switch (sbuthit(srects)){
	case 0: shipchar('\n');
#ifdef DMD630
		shipchar('\r');
#endif
		buttons(UP);
		break;
	case 1: shipchar('\f');
		buttons(UP);
		break;
	default:
		laststate=RES;	/* force a call to showbitmap */
		break;
	}
	thinkflush();
#ifdef DMD630
	request(RCV|KBD|MOUSE);
#endif
}

clone_bitmap()
{
	Bitmap *clonemap;

	clonemap = BNULL;
	/* should test to make sure proc is still there.... */
	if (oproc) 
		obp = (Bitmap *) oproc->layer;
	if (mustfree==2)
		formp(INULL,INULL,INULL, "using a copy...");
	else if ( obp && (clonemap = balloc(rect)) ) {
		bitblt(obp,inset(rect,in),clonemap,add(rect.origin,in),F_STORE);
		restore();
		if (mustfree)
			bfree(bp);
		bp=clonemap;
		mustfree=1;
		proc=PNULL;
		formp(INULL,INULL,INULL, "made a copy...");
	} else 
		formp(INULL,INULL,INULL, "using the screen image...");
	sleep(15);
	laststate = RES;
}

clearlayer()
{
	Rectangle dstrect;

	dstrect=P->layer->rect;
	dstrect.origin=add(dstrect.origin,Pt(8,38));
	dstrect.corner=add(dstrect.corner,Pt(-8,-(defont.height+8)));
	rectf(P->layer,dstrect,F_CLR);
}

alert()
{
	ringbell();
	flash(P->layer,P->layer->rect);
	sleep(20);
}

flash(b,r)
Bitmap *b;
Rectangle r;
{
	rectf(b,r,F_XOR);
	nap(20);
	rectf(b,r,F_XOR);
}
