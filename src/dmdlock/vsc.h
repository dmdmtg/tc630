#define REFRESH50	((unsigned short *)(0x6000be))
#define REFRESH60	((unsigned short *)(0x60007e))
#define UNBLANK		((unsigned short *)(0x6000de))
#define BLANK		((unsigned short *)(0x6000ee))

#define FIFTY_HZ	(*REFRESH50 = 0)
#define SIXTY_HZ	(*REFRESH60 = 0)
#define SCREENON	(*UNBLANK = 0)
#define SCREENOFF	(*BLANK = 0)

/* The following defines are used by screensaver to identify devices */
#define SSCLOCK		0	/* realtime clock */
#define SSMXY		1	/* mouse movement */
#define SSKBD		2	/* keyboard input */
#define SSMB		3	/* mouse buttons */

#define	ACTTMR		(long)54000	/* number of ticks in 15 minutes */
#define acttmr		D_Ref(long, -79)	/* timer vector table slot  */
