#define DMDENV		0
#define XENV		1

#define CSenv		1<<0
#define CScapability	1<<1
#define CSswapin	1<<2
#define CSswapout	1<<3
#define CSdomouse	1<<4
#define CSdevpriv	1<<5
#define CSdestroy	1<<6

/* bit defines for the swapscreenstr capability bit vector */ 
#define ALLOW_AGENT	0x1
#define ALLOW_REBOOT	0x2

/* flags for remaplayer */
#define CREATE		1
#define TRANSFER	2

struct Cursor{
	unsigned x,y;
        unsigned oldx, oldy;
        char quad, oldquad;
        short inhibit;
        short up;
};

struct curtab{
        short   *map;
        short   dx;
        short   dy;
};

typedef struct swapspacestr {
	int env;		/* DMDENV, XENV, etc. */
	int capability;		/* ALLOW_AGENT etc. */
	Bitmap *screen;
	Layer *lfront;
	Layer *lback;
	
	int (*swapin)();
	int (*swapout)();
	void (*domouse)();
	void (*destroy)();

	struct Cursor cursor;
	struct curtab *curtabp;
	struct curtab usercurtab[4];
	int cursclipt;

	char *devpriv;		/* pointer to a privite structure */
	struct swapspacestr *next;
	struct swapspacestr *prev;
	long pad;
} SwapSpaceStr;


typedef struct dmdswappriv {
	Proc *current;
	int primarydmdenv;	/* 1 if yes, 0 if no */
} DMDswappriv;

extern int spacecount;                         /* number of screens in list*/
extern int swaptranslateoff;
extern SwapSpaceStr *currentssp;               /* visible space */
extern SwapSpaceStr *nextssp;          /* next space to swap in */
extern SwapSpaceStr *ssptokill;          /* space to kill */
extern int swapperdisabled;


extern SwapSpaceStr *createswapspace();
extern int swapspace();
extern SwapSpaceStr *getcurrentssp();
extern SwapSpaceStr *getssp();
extern int destroyswapspace();
extern int remaplayer();
extern void changeswapspace();
extern int disableswapper();
extern int enableswapper();
extern SwapSpaceStr *createssp();
extern int searchssp();
extern int destroyssp();
extern int linkssp();
extern int unlinkssp();
extern int initswapper();
extern int swapper();
extern int killswapspace();
extern Word *realaddr();
