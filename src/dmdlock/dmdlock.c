#ifdef DMD630
#include <dmd.h>
#include <object.h>
#include <font.h>
#include <5620.h>
#include <menu.h>
#include "machdep.h"
#include "io_exp.h"
#include "vsc.h"
#include "swapper.h"
#else
#include <blit.h>
#include <font.h>

#define	upfront
#define tolayer

#include <pandora.h>
#endif
#include "stdpass.h"

#define ban() OptL?banner("select",&unlockicon):banner("hit cr",&unlockicon)

  /*******************************************************************/
  /*
  /*	lock a DMD-like terminal while in layers
  /*
  /*	ihnp4!mhuxn!presley 07/20/84
  /*	    * 10/29/84 - make sure layer current before locking (schnable)
  /*	    * 02/01/85 - -l option for lock on layer made current (presley)
  /*	    * 02/15/85 - -u option implemented (presley)
  /*	    * 03/25/85 - code for 2.0 DMDs (schnable)
  /*	    * 03/29/85 - canonical character processing (presley)
  /*	    * 04/03/85 - menu for changing options (presley)
  /*	    * 07/22/86 - fixes for missing UKey   ARH (beehive!hastings)
  /* V1.0   * 04/02/87 - port to DMD 630 (beehive!schnable)
  /*			- stole (er, reused) dave prosser's texture code.
  /*			- stole locking algorithm from dp code too.
  /*			- ask twice for typed password.
  /*			- added some appropriate kbd flushes.
  /*			- no-op the f flag - now the default (and only choice.)
  /*			- added <cr> locking.
  /*			- added exit confirm.
  /*			- added reshape code.
  /*			- added caching (-c)
  /* V2.0   * 03/88	- changed from bttn to button function (schnable)
  /*			- fixed "current" problem where 630 would make other
  /*			  layer current.
  /* V3.0   * 04/88	- fixed problem with "current" fix (above) - (schnable)
  /*			  wait for KBD before expecting typed input...
  /*			- added lock now (reuses OptF for -L; OptL used for -l )
  /* V3.3   * 06/88	- added screen blanking (-b) (schnable)
  /*	    		- added auto timeout (-a)
  /*	    		- added versioning (LVERS)
  /*			- added screen flash while blanked
  /*			- cleaned up wait4 - condensed 2 routines into one
  /*			  (now three events to wait for...)
  /* V3.5   * 11/88	- top on lock
  /*	    * 06/89	- fix -b screen texturing bug
  /*			- (added distinctive ringing a while ago...)
  /*			- fixed -A timer reset problem (disp not redrawn).
  /* V3.6   * 08/88	- work with ROM version 2 swapper facility
  /* 	    * 10/88	- make "ROMABLE" - move bss initializations out
  /* 	    * 10/88	- use tmenuhit
  /* V3.7   * 4/90	- add uncurrent support
  /* V4.0   * 7/91	- add SVR4.0 password standards, change menus to
  /*                      reflect no access to login password, only to host
  /*			  (.dmd.lock) password, due to /etc/shadow (ericson)
  /*	    * 9/91	- fixed recache() routine to cache only if the -c
  /*	                  option was passed to the program.... (ericson)
  /* V4.1   * 7/93	- Block Current and Top functions so that iconize
  /*			  and 730screens can't steal current/top from us.
  /*			  (this is schnable's version 3.8 fixes)
  /*************************************************************************/

#define RETRY		'r'
#define LOCKED		01
#define UNLOCKED	02
#define SLPTIME		6		/* in ticks */ 
#define CRYPTIME	120		/* in ticks */
#define WARNTIME	60		/* in secs  */
#define LVERS "dmdlock4.1"
#define SWAPVERS 0x080808	

Texture16 lockicon = {
	 0xFFFF, 0xFC3F, 0xF81F, 0xF18F, 0xF3CF, 0xF3CF, 0xF007, 0xE007,
	 0xEFC7, 0xECC7, 0xECC7, 0xECC7, 0xECC7, 0xE78F, 0xF01F, 0xFFFF,
};
Texture16 unlockicon = {
	 0x0000, 0x01F0, 0x03F8, 0x031C, 0x000C, 0x000C, 0x000C, 0x03FE,
	 0x07FE, 0x040E, 0x74CE, 0x5FCE, 0x5FCE, 0x74CE, 0x061C, 0x03F8,
};
Texture16 T_foreground = {
	0xEEEE, 0xBBBB, 0xEEEE, 0xBBBB, 0xEEEE, 0xBBBB, 0xEEEE, 0xBBBB,
	0xEEEE, 0xBBBB, 0xEEEE, 0xBBBB, 0xEEEE, 0xBBBB, 0xEEEE, 0xBBBB,
};
#ifndef DMD630
Texture16 skull = {
	0x0000, 0x0000, 0x0000, 0x3002, 0x57E6, 0x3FFC, 0x0FF0, 0x0DB0,
	0x07E0, 0x0660, 0x37EC, 0x5426, 0x33C2, 0x0000, 0x0000, 0x0000,
};
#endif

#ifdef DMD630
#define T_TI	TM_TEXT | TM_ICON

typedef struct TexIco
{
	char *text;
	Bitmap *icon;
} TexIco;

TexIco RomMainMenu[] = {
#define MTYPED 0
	{ "typed password", 0 },
#define MLOGIN 1
	{ "host password", 0 },
	{ "", 0 },
#define MSEL 3
	{ "lock on select", 0 },
#define MAUTO 4
	{ "lock on timeout", 0 },
#define MSHOWAUTO 5
	{ "hide countdown", 0 },
#define MBLANK 6
	{ "blank on lock ", 0 },
#define MAXMENU 8
	0
};
TexIco MainMenu[MAXMENU];	/* stuffed with RomMainMenu */
Tmenu mainmenu = { (Titem *) MainMenu, 0, 0, 0, T_TI };
#else
char *menutext[] = {
#define MTYPED 0
	"* typed password",
#define MLOGIN 1
	"  host password",
	"",
#define MSEL 3
	"* lock on select",
#define MCR 4
	"  lock on cr    ",
#ifndef DMD630
	"",
#define MQUIT 6
	"  quit          ",
#endif
	NULL
};
Menu menu = { menutext };
#endif

extern char *crypt();	/* /etc/passwd crypt algorith */
extern Point myctob();	/* my ctob function (below) */

int OptJ;	/* -j : just kidding you jerq - don't lock (for debugging) */
int OptU;	/* -U : use host/login password (/etc/shadow -> only host) */
int OptL;	/* -l : Lock on cr or select */
int OptF;	/* -L : Lock when first downloaded */
#ifdef DMD630
int OptC;	/* -c : cache */
int OptA;	/* -a : auto-lock */
int OptAA;	/* -A : hide auto-lock */
int OptB;	/* -b : blank screen on lock*/
long lacttmr;	/* Auto-Lock timeout (offset away from from ACTTMR) */
int stime;	/* Auto-Lock timeout max value (in secs) */
int Alive;	/* a dmdlock is alive or just cached */
int Die;	/* time for the duplicate dmdlock to die */

SwapSpaceStr *ssp;	/* used to manipulate swap screen to ensure dmdlock */
			/* shows up in current swap screen */
#endif
int HasKey;		/* password has been supplied */
Texture16 *lasticon;	/* remember this for a possible reshaping... */
char *laststr;		/*    "      "    "  "    "         "        */
Bitmap blit;		/* physical screen that we can mess with */
char *UKey;		/* user's password encoded key (from unix) */
char *IKey;		/* user's login ID for password routines */
char keyword[128], 	/* local passwd typed once */
     keyword2[128];  	/* local passwd typed twice */
char line[512];		/* gp buffer -  what the user just typed... */
char line2[512];		/* gp buffer -  what the user just typed... */

#ifndef DMD630
/*
	we lock the DMD 5620 by patching Sys[154], the layers function that
	sees which mouse button was pushed. Its never button 3, so we never get
	a button 3 layers menu. Its never button 1, so you cant pick another.
*/
ptr_fint SaveButton;	/* save button function adress here. */
int MyButtFunc();	/* dummy routine that always returns 2. */
int newROM;		/* new ROM flag - true means 2.0 ROMS */
#endif

ptr_fint SaveTop;	/* save top() function pointer here. */
ptr_fint SaveCurrent;	/* save current() function pointer here. */
ptr_fint SaveProc;	/* save current() function pointer here. */
int MyTopCurFunc();	/* dummy routine to circumvent Top and Current */


main (argc, argv)
char *argv[];
{
	initglobs();
	parseargs(argc, argv);
	initdmdlock(); 

	while (1) {
		cursswitch (&unlockicon);

		/* get a password */
		if (!HasKey)  {
			flushkbd();
			getpass();
		}
		redraw();

		/* wait4event waits for a good time to lock */
		/* -F tells us to ignore this and go lock right now */
		if ( ! (OptF && own()&MOUSE) )
			wait4event();	/* look for timeout/cr/selection */
		OptF=0;			/* reset OptF - only good once. */

		/* lock the terminal - tell user... */
		cursswitch (&lockicon);
		banner("locked",&lockicon);
		dmdlock();

		/* loop looking for the correct password */
		while (1) {
			flash(1); /* flash layer or screen once */
#ifdef DEBUG
			banner1(IKey);
			banner2(UKey);
#endif
			if (GetLine (line, 500, LOCKED) == RETRY)
				continue;
#ifdef DEBUG
			banner1(line);
			strcpy(line2,crypt(line,UKey));
			banner2(line2);
			sleep(30);
#endif
			/* may have to encrypt password */
			if (OptU && UKey[0]) {
				strcpy(line2,crypt(line,UKey));
				if (strcmp(line2,UKey) == 0) 
					break;
			} else
				if (strcmp(line,keyword) == 0)
					break;
			ring(1,10); ring(9,10); ring(1,10); ring(9,10);
		}
		dmdulock();	/* if we get here, they typed it right... */
	}
}


#ifdef DMD630
SeeMenu () {

	if (own()&MOUSE && button3()) {
		Titem *ret;
		int sel;

		clrChkmark( &mainmenu );
		setChkmark( &mainmenu, OptU ? MLOGIN : MTYPED );
		setChkmark( &mainmenu, OptL ? MSEL : -1 );
		setChkmark( &mainmenu, OptB ? MBLANK : -1 );
		setChkmark( &mainmenu, OptA ? MAUTO : -1 );
		setChkmark( &mainmenu, OptAA ? MSHOWAUTO : -1 );

		ret = tmenuhit(&mainmenu, 3, TM_EXPAND);
		if ( ret == (Titem *) NULL )
			return -1;

		switch ( sel=indexHit( &mainmenu ) ) {
		case MTYPED:	/* ask for new password */
			HasKey = 0;
			OptU = 0;
			getpass();
			break;
		case MLOGIN:	/* use the host/login password */
			if (UKey[0]) {
				OptU++;
				HasKey++;
			} else
				return -1;   /* no host/login passwd to use */
			break;
		case MSEL:
			OptL = ( OptL ? 0 : 1 );
			break;
		case MSHOWAUTO:
			OptAA = ( OptAA ? 0 : 1 );
			break;
		case MAUTO:
			OptA = ( OptA ? 0 : 1 );
			break;
		case MBLANK:
			OptB = (OptB ? 0 : 1 );
			break;
		default: 
			return -1;
			break;
		}
		/* store changes for cache */ 
		recache();
		ban();
		return (sel);
	}
	return (-1);
}
#else
SeeMenu () {

	if (own()&MOUSE && button3()) {
		int mybutton;

		menutext[1] = UKey[0] ? "  host password" : "  (host passwd)" ;
		menutext[MLOGIN][0] = OptU ? '*' : ' ';
		menutext[MTYPED][0] = OptU ? ' ' : '*';
		menutext[MSEL][0] = OptL ? '*' : ' ';
		menutext[MCR][0] = OptL ? ' ' : '*';

		mybutton = menuhit(&menu,3);
		switch (mybutton) {
		 case MTYPED:	/* ask for new password */
			HasKey = 0;
			OptU = 0;
			getpass();
			break;
		 case MLOGIN:	/* use the host/login password */
			if (UKey[0]) {
				OptU++;
				HasKey++;
			} else
				return -1;	/* no host passwd to use */
			break;
		 case MSEL:
			OptL = 1;
			break;
		 case MCR:
			OptL = 0;
			break;
		 case MQUIT:	/* quit */
			banner("really?",&skull);
			if (lexit3())
				exit(0);
			break;
		}
		return (mybutton);
	}
	return (-1);
}
#endif

/* get a line from the keyboard */
GetLine (buffer, nchar, state)
	register char *buffer;
{
	register char *endptr = buffer + nchar;
	char *startptr = buffer;
	int c;
	while (buffer < endptr) {
		if (state != LOCKED)
		{
		   int mybutton = SeeMenu ();
		   /*
		    * SeeMenu() sets variables, we just return
		    */
		   if (mybutton == MTYPED 
		   || mybutton == MLOGIN 
#ifndef DMD630
		   || mybutton == MQUIT )
#else	
			)
#endif
			return (RETRY);
		}

		/* get characters */
		wait (CPU);
#ifdef DMD630
		if (state==LOCKED && OptB && !OptJ)
			SCREENOFF ;
		realcurrent();
		realtop();
		Alive++;
		if (Die) {
			Die=0;
			delete();
		}
#else
		realcurrent();
		realtop();
#endif

		if (own() & KBD) {
			if ((c = kbdchar()) == -1)
				continue;
			if (c == '\r' || c == '\n')
				break;
			/* pseudo-canonical processing */
			if (c == '\b' && buffer != startptr)
				buffer--;
			else if (c == '@')
				buffer = startptr;
			else
				*buffer++ = c;
		}
	}
	*buffer = '\0';
	return (0);
}

dmdlock()
{
	sleep(1);
	if (!OptJ) {
		lckcurtop();/* disable use of current() and top() */
		lckswap();	/* disable swapper */
		lckmouse();	/* disable mouse */
		lckscrn();	/* make screen indicate a lock is set */
		lckagent();	/* disable user agent calls */
		flushkbd();	/* get rid of any spurious queued key hits */ 
	}
}

dmdulock()
{
	if (!OptJ) {
		ulckcurtop();/* disable use of current() and top() */
		ulckswap();
		ulckagent();
		ulckscrn();
		ulckmouse();
	}
}

textureit(t)
Texture16 *t;
{
#ifndef DMD630
 	texture16(&display, 
#else
 	texture(&display, 
#endif
		Rpt(Pt(Drect.origin.x, Drect.origin.y+16), 
		    Pt(Drect.corner.x, Drect.corner.y)), t, F_STORE);
}

ulckcurtop()
{
#ifndef	DMD630
	Sys[73] = SaveCurrent;
	Sys[52] = SaveTop;
#else
	Sys[730] = SaveCurrent;
	SaveCurrent = 0;
	Sys[732] = SaveProc;
	SaveProc = 0;
	Sys[745] = SaveTop;
	SaveTop = 0;
#endif
}

lckcurtop()
{
#ifndef	DMD630
	SaveCurrent = Sys[73];
	Sys[73] = MyTopCurFunc;

	SaveTop = Sys[52];
	Sys[52] = MyTopCurFunc;
#else
	SaveCurrent = Sys[730];
	Sys[730] = MyTopCurFunc;

	SaveProc = Sys[732];
	Sys[732] = MyTopCurFunc;

	SaveTop = Sys[745];
	Sys[745] = MyTopCurFunc;
#endif
}

realtop()
{
	if( SaveTop )
		(*SaveTop)(P->layer);
	else
		top();
}

realcurrent()
{
	if( SaveCurrent )
#ifdef	DMD630
		(*SaveProc)(P);
#else
		(*SaveCurrent)(P->layer);
#endif
	else
		current();
}

lckmouse()
{
#ifndef DMD630
	if (newROM)
	{
		SaveButton = Sys[154];
		Sys[154] = MyButtFunc;
	}
#else
	bttnsdisable();
#endif
}

#ifdef DMD630
long oldagent;		/* cast to the right thing as needed... */

/* Use 'myagentrect' instead of 'agentrect' so don't need to include agent.h */
/*   because that is sometimes difficult to find for a terminal program */
struct myagentrect{
	short	command;
	short	chan;
	/* rest of the structure deleted */
};

newagent(arp,size)
struct myagentrect *arp;
int size;
{
 	arp->command = -1;	/* it will always fail */
	return(size);
}
#endif

lckagent()
{
#ifdef DMD630
	oldagent = (long)(Sys[133]);
	Sys[133] = newagent;
#endif
}

ulckmouse()
{
#ifndef DMD630
	if (newROM)
	{
		Sys[154] = SaveButton;
	}
#else
	bttnsenable();
#endif
}

ulckagent()
{
#ifdef DMD630
	Sys[133] = (int (*)())oldagent;
#endif
}

ulckscrn()
{
#ifndef DMD630
	texture16(&blit, blit.rect, &T_foreground, F_XOR);
	texture16(&blit, inset(blit.rect,6), &T_foreground, F_XOR);
#else
	if (OptB)
		SCREENON;
	else {
		texture(&blit, blit.rect, &T_foreground, F_XOR);
		texture(&blit, inset(blit.rect,6), &T_foreground, F_XOR);
	}
#endif
}

lckscrn()
{
#ifndef DMD630
	texture16(&blit, blit.rect, &T_foreground, F_XOR);
	texture16(&blit, inset(blit.rect,6), &T_foreground, F_XOR);
#else
	if (OptB)
		/* SCREENOFF */ ;
	else {
		texture(&blit, blit.rect, &T_foreground, F_XOR);
		texture(&blit, inset(blit.rect,6), &T_foreground, F_XOR);
	}
#endif
}

MyTopCurFunc()
{
	return 0;
}

MyButtFunc()
{
	return 2;
}

banner(str,icon)
char *str;
Texture16 *icon;
{
	lasticon=icon;
	laststr=str;

	rectf (&display, Drect, F_CLR);
	textureit(icon);
	banner1(str);
	banner2(LVERS);
}

flash(times)
int times;
{
	for (; times > 0 ; times--) {
		rectf (&display, Drect, F_XOR);
		nap(2);
	}
}

flushkbd()
{
	while (kbdchar() != -1)
		wait(CPU);
}

getpass()
{
	while (!HasKey) {
		banner("passwd:",&unlockicon);
		wait(KBD);
		if (GetLine(keyword,64,UNLOCKED) == RETRY) 
			continue;

#ifdef DEBUG
			banner2(keyword);
#endif
		HasKey=0;
		/* Note: all the 'complaints' below are terse because the
		 *  default window size only allows for a 9-character wide
		 *  message.
		 */
		switch ( stdpass(IKey, keyword) ) 
		{
		case PWDOK:
			HasKey++;	/* so far, so good... */
			break;
		case PWDSHORT:		/* check password length */
			complain("Too Short.");
			break;
                case PWDCIRC:	/* Password is circular shift of login */
			complain("Avoid ID.");
			break;
                case PWDNOMIX:
		/* passwords must contain at least two alpha characters */
		/* and one numeric or special character                 */
			complain("Not Special.");
			break;
		}
		if ( HasKey ) 	/* did entry meet password standards? */
		        HasKey=0;	/* yes */
		else 
			continue;	/* no */
		
		banner("again:",&unlockicon);
		if (GetLine(keyword2,64,UNLOCKED) == RETRY) 
			continue;
		if (strcmp(keyword, keyword2)) {
			complain("mismatch.");
			continue;
		} else {
			banner("unlocked",&unlockicon); 
			HasKey++;
		}
	} 
	P->state |= RESHAPED;
	recache();
}

/* complain() - put a message in the top of the dmdlock window
 *  and flash it for a short time.
 *
 *  Note: the message should be terse because the default window
 *  size only allows for a 9-character wide message.
 */
complain(message)
char * message;
{
	banner(message, &unlockicon); 
	ringbell(); 
	flash(3); sleep(SLPTIME);
	flash(3); sleep(SLPTIME);
}


#ifndef DMD630
#define DOWN 1
#define UP 0

lexit3()	/* return true if button3 is clicked */
{
	Texture16 *prev; 
	int lexit;

	banner("really?",&skull);
	prev = (Texture16 *)cursswitch((Texture16 *) &skull);
	lexit = buttons(DOWN);buttons(UP);
	cursswitch(prev);
	return(lexit == 3);
}

buttons(updown)
int updown;
{
	do wait(MOUSE);
	while((button123()!=0) != updown || !ptinrect(mouse.xy,Drect));

	switch (button123()) {
		case 4: return 1;
		case 2: return 2;
		case 1: return 3;
	}
	return 0;
}
#endif

wait4unlock()
{
	/* wait for previous unlocking to finish - give user a chance to */
	/* select another layer or set cr based locking before going on */
	if (OptL) {
		banner("unlocked",&unlockicon);
		while (OptL && (own()&MOUSE) ) { /* just unlocked - */
			wait(CPU);		 /* wait for mouse to release */
			redraw();		 /* or for OptL to change */
			SeeMenu();
		}
	}
	ban();
}

wait4event()
{
	wait4unlock();

	/* busy wait loop - look for event to fire */
	ban();					/* put out banner */
	while (1) {
		redraw();
#ifdef DMD630
		Alive++;
		if (Die) {			/* as bob would say, 	*/
			Die=0;			/* its time to goodnight... */
			delete();
		}
#endif

		OptL? wait(CPU): wait(CPU|KBD);
		if (SeeMenu() != -1)
			wait4unlock(); /* give chance to deselect */
	
		/* look for event and return when fired */
		if (OptL && (own()&MOUSE) )
			return 1;	/* made current */
		else if (!OptL && (kbdchar() == '\r' ) )
			return 1;	/* return key hit */
#ifdef DMD630
		else if (OptA) {
			showcount();
			if ( acttmr <= lacttmr ) 
				return 1; /* timer fired */
		}
#endif
	}
}

#ifdef DMD630

#define ilevel		P_Array(struct Ilevel, 371)
#define Ibutton		D_Ref(int, -80)

int (*oldbttnintr)();
int oldbttnstate;

mybttnintr()
{
	register i,c;

	i = (c = (~(*BUTTONS)&7)) == oldbttnstate;
	oldbttnstate = c;
	return(i);
}

bttnsdisable()
{
	register Ihandler *ihp;

	ihp = &ilevel[Ibutton/MAXLEVELS].ihandlers[Ibutton%MAXHANDLERS];
        oldbttnintr = ihp->service;
        ihp->service = mybttnintr;
}

bttnsenable()
{
	register Ihandler *ihp;

	ihp = &ilevel[Ibutton/MAXLEVELS].ihandlers[Ibutton%MAXHANDLERS];
	ihp->service = oldbttnintr;
}

#endif

redraw()
{
	if ( P->state & RESHAPED ) {
		banner(laststr,lasticon);
		P->state &= ~RESHAPED;
	}
}

#ifdef DMD630
Point myctob(x,y,p)
int x,y;
struct Proc *p;
{
	Point q;

	q.x=q.y=100;
	return q;
}
#endif

#ifdef DMD630
showcount()
{
	long ltime;
	static long otime;
	static warnreset=0;

	ltime=(acttmr-lacttmr)/60;
	/* display every second after a wait */
	if ( ltime != otime && ( !OptAA || ltime < WARNTIME) ) {
		if (ltime < stime-4 ) 
			sprintf(line,"%ld",ltime);
		else
			sprintf(line,"%d",stime);
		banner2(line);
		otime=ltime;
	} 

	if ( ltime == WARNTIME ) {
		warnreset=1;
		ring(1,20);
		sleep(40);
	}

	/* redraw the screen if user resets counter */
	if ( warnreset && (ltime > WARNTIME) ) {
		P->state |= RESHAPED;
		warnreset=0;
	}
}
#endif

#define drstore(r) rectf(&display,r,F_STORE)
#define drclr(r) rectf(&display,r,F_CLR)
#define drstring(s,p) string(&defont,s,&display,p,F_XOR)
#define LINE(place) add(place.origin,Pt(2,2))

print (place,str)
Rectangle	place;
char *str;
{
	drclr(place);
	drstring(str, LINE(place));
	sleep(5);
}

banner1(str) 
char *str;
{
	Rectangle kbdrect;

	kbdrect=Drect;
	kbdrect.corner.y=kbdrect.origin.y+defont.height+4;
	print(kbdrect,str);
}

banner2(str) 
char *str;
{
	Rectangle kbdrect;

	kbdrect=Drect;
	kbdrect.origin.y=kbdrect.corner.y-defont.height-4;
	print(kbdrect,str);
}

/* modified to cache ONLY if the -c option was set */
recache()
{
#ifdef DMD630
	if (OptC) 
	{
		decache();
		cache("dmdlock",A_TEXT|A_DATA|A_BSS); 
	}
#endif
}

ring(note,dur)
int note,dur;
{
#ifdef DMD630
	int i;

	if (dur<=0)
		dur=1;

        alarm(dur);
	while (wait(CPU)) {
		if (own()&ALARM)
                        break;
		click();
		for( i=0; i <= note*100; i++ )
                        ;
	}
#else
	ringbell();
#endif
}

ulckswap()
{

#ifdef DMD630
	if (version() >= SWAPVERS) {
		enableswapper();
		makeuncurrent(P);
	}
#endif
}

lckswap()
{
#ifdef DMD630
	if (version() >= SWAPVERS) {
		ssp=getssp(P);
		swapspace(ssp);
		disableswapper();
		while (wait(CPU))
		{
			if (ssp == getcurrentssp())
				break;
		}
	}
#endif
}

#ifdef DMD630
clrChkmark( tmenu )
Tmenu *tmenu;
{
	register TexIco *ci;
	register int i;

	ci = (TexIco *) tmenu->item;

	for ( i=0; ci->text != NULL ; ci++, i++ )
		ci -> icon = 0;
}

setChkmark( tmenu , index)
Tmenu *tmenu;
int index;
{
	register TexIco *ci;
	register int i;

	ci = (TexIco *) tmenu->item;

	if (index==-1)
		return;

	for ( i=0; ci->text != NULL ; ci++, i++ )
		if (index==i)
			ci->icon = &B_checkmark;
}

indexHit(tmenu)
Tmenu *tmenu;
{
	return tmenu->prevhit + tmenu->prevtop ;
}
#endif

initglobs()
{
	register int i;

	HasKey = 0;	/* password has been supplied */
	IKey = "";	/* user's login ID (from unix) */
	UKey = "";	/* user's password encoded key (from unix) */
	OptJ = 0;	/* -j : just kidding - don't lock */
	OptU = 0;	/* -U : use user host/login password */
	OptL = 0;	/* -l : Lock on cr or select */
	OptF = 0;	/* -L : Lock when first downloaded */
#ifdef DMD630
	OptC = 0;	/* -c : cache */
	OptA = 1;	/* -a : auto-lock */
	OptAA = 0;	/* -A : hide auto-lock */
	OptB = 0;	/* -b : blank screen on lock*/
	lacttmr = 0;	/* Auto-Lock timeout (offset away from from ACTTMR) */
	stime = 15*60;	/* Auto-Lock timeout max value (in secs) */
	Alive = 0;	/* a dmdlock is alive or just cached */
	Die = 0;	/* time for the duplicate dmdlock to die */
#endif

	/* set up a version of the display we can mess around with */
	blit.base=addr(&display,Pt(0,0));
	blit.width=display.width;
	blit.rect=Jrect;

#ifdef DMD630
	/* fill in menu structure */
	for (i=0 ; i<MAXMENU ; i++)
		MainMenu[i]=RomMainMenu[i];
#endif
}

parseargs(argc, argv)
int argc;
char *argv[];
{
	int i;	
	int timeout;	/* in minutes */

	/* scan options */
	for (i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (arg[0] == '-') {
			switch (arg[1]) {
			 case 'j':
				OptJ++;
				break;
#ifdef DMD630
			 case 'b':
				OptB++;
				break;
			 case 'A':
				OptAA++;
				/* drop thru... */
			 case 'a':
				OptA++;
				if ( arg[2] != '\0' ) 		/* -aXX */
				    	timeout=atoi(&arg[2]); 
				else if ( (i+1) < argc && argv[i+1][0] != '-' )
				    	timeout=atoi(argv[++i]);/* -a XX */
				else
				    timeout=15;			/* -a */
				if (timeout > 15)
					timeout=15;
				if (timeout == 0)
					OptA=0;
				else {
					lacttmr=(ACTTMR-timeout*(long)3600);
					stime=timeout*60;
				}
				break;
			 case 'c':
				OptC++;
				break;
#endif
			 case 'L':
				OptF++;
				OptL++;
				break;
			 case 'l':
				OptL++;
				break;
			 case 'u':
				OptU++;
				HasKey++;
				break;
			 case 'U':
				UKey = &arg[2];
				break;
			 case 'I':	/* login ID, for password standards */
				IKey = &arg[2];
				break;
			 /* default: ignore argument  - hope its been parsed */
			}
		}
		else
		{
			strcpy (keyword, arg);
			HasKey++;
		}
	}

	/* sanity checks */
	if (UKey[0] == '\0') {
		if (OptU)
		{
			OptU = 0;
			HasKey = 0;
		}
	}
}

initdmdlock()
{
#ifdef DMD630
	local();		/* detach from UNIX */
	P->ctob = myctob;	/* set the default outline */
	recache();		/* cache application in local memory */
	{
		int a;		/* ensure that only "this" dmdlock runs */
		a=Alive;
		sleep(30);
		if (Alive != a)	/* another dmdlock is alive */
			Die=1; 	/*  signal it to die... */
		while (Die)
			wait(CPU);
	}
#else
	newROM = (version() & 0xff) >= 5;
#endif
	request (MOUSE|KBD|ALARM);
	redraw();
}

#ifndef	DMD630
current()
{
	tolayer(P->layer);
}

top()
{
	upfront(P->layer);
}
#endif
