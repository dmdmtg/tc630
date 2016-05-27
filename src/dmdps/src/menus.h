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
powertwo[LASTPOWER+1]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096};

/*
	note: don't make MWRITE or MPIPE a zero -
		some code depends on this.
*/
char *bit_output[]={ /* appears on bttn 2 after bitmap select */
#define MWRITE	0
	"write bitmap",
#define MPIPE	1 
	"pipe  bitmap", 
#define	MPRINT	2
	"print bitmap",	/* really, this is filled in as needed. */
	NULL 
};
Menu bitoutput = { bit_output };

char *bttn3_menu[]={ /* appears on bttn 3 in select state. */
#define MSEL_FILE	0
	"read file",
#define MSEL_LAYER	1
	"choose layer",
#define MSEL_SCREEN	2
	"whole screen",
#define MSEL_RECT	3
	"sweep rectangle",
#define MSELOP		4
	"",
#define	MISC		5
	"options...",
#define MCOPYOP		6
	"make a copy",
#define	MFLIPSTIP	7
	"flip stipple", 
#define	MFLIPVID	8
	"reverse video", 
#define MINSET		9
	"inset rectangle",
#define	MRUNHALT	10
	NULL, 
#define RUN_M	"run"
#define HALT_M	"halt"
	NULL 
};
Menu bttn3menu = { bttn3_menu };

char *misc_menu[] = {
#define MCOPY		0
	"* make copy      ",
#define MONE2ONE	1
#define MCLIP	"* clip to fit    "
#define MSCALE	"* scale to fit   "
	"",
#define MDUMP		2
	"  hex debug dump ",
#define MHOLD		3
	"  hold download  ",
#define MFLUSH		4
	"  flush download ",
#define MMAP		5
	"  map lf -> lf/cr",
	"",
#if defined(PAR) || defined(DMD630)
#define MREADPFD	7
#define MEXIT		8
#else
#define MPOPTS		7
#define MREADPFD	8
#define MEXIT		9
	"port opts  ",
#endif
	"set printer",
	"exit dmdps ",
	NULL
};
Menu miscmenu = { misc_menu };

#if !defined(PAR) && !defined(DMD630)
char *port_opts[]={
#define M8	0
	"  8 even",
#define M7	1
	"  7 odd ",
	"",
#define M1200	3
	"  1200  ",
#define M4800	4
	"  4800  ",
#define M9600	5
	"  9600  ",
	"",
#define FLOWCHARS	7
	"chars / bucket",
#define FLOWTICKS	8
	"wait after bucket",
	NULL
};
Menu portopts = { port_opts };
#endif

char *iomenutext[]= {
#define FLUSH	0
	"  abort io",
#define HOLD	1
	"  hold  io",
	NULL
};
Menu iocontrol_menu={ iomenutext };

#if !defined(PAR) && !defined(DMD630)
char *tickmenu[]= {
	" 1   ",
	" 2   ",
	" 4   ",
	" 8   ",
	" 16  ",
	" 32  ",
	" 64  ",
	" 128 ",
	" 256 ",
	" 512 ",
	" 1024",
	" 2048",
	" 4096",
	NULL
};
Menu tick_menu={ tickmenu };

char *charsmenu[]= {
	" 1   ",
	" 2   ",
	" 4   ",
	" 8   ",
	" 16  ",
	" 32  ",
	" 64  ",
	" 128 ",
	" 256 ",
	" 512 ",
	" 1024",
	" 2048",
	" 4096",
	NULL
};
Menu chars_menu={ charsmenu };

#endif
