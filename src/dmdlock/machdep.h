/*
 * Machine Dependent Definitions
 */


#define DISPBASE	(Word *)(0x760000)
#define BRAM_SIZE	8185 /* size of BRAM in bytes -7, for int vectors */
#define ROMMIN		(char *)0x000000
#define ROMMAX		(char *)0x100000
#define BRAMMIN		(char *)0xffc001
#define BRAMMAX		(char *)0x1000000

/** In dmd.h for now - BLIT
#define	XMAX		1024
#define	YMAX		1024
***/
#define TVIDEO          (DUART->scc_sopbc1 = 0x08)
#define RVIDEO          (DUART->scc_ropbc1 = 0x08)
#define BonW()          RVIDEO
#define WonB()          TVIDEO
#define XMOUSE          ((unsigned short *)(0x200040))
#define YMOUSE          ((unsigned short *)(0x200042))
#define BUTTONS         ((unsigned char *)(0x200045))
