/* 2681.h must be included before this include file is used */

/* Functions to determine the availability of expansion cards */
#define	sio_avail()	(!(DUART->ip_opcr1 & SER_EXP))
#define pio_avail()	(!(DUART->ip_opcr2 & PIO_EXP))
#define	cart_avail()	(!(DUART->ip_opcr1 & CART_AVAIL))

/* ID register */
#define	PIO_ID_REG	(unsigned char *)(0x400081)

/* ID vectors for parallel expansion boards */
#define SSI_SIO_ID	(unsigned char)0xfe


/* Interrupt levels */
#define I_AVAIL		0	/* handler slot available */
#define I_ENABLED 	1	/* handler slot active */
#define I_DISABLED	2	/* handler slot inactive */

#define MAXLEVELS	7	/* there are 7 interrupts levels: 1 through 7 */
#define MAXHANDLERS	8	/* each level can handle 8 different interrupts */

typedef struct Ihandler {
	int istate;		/* I_AVAIL, I_ENABLED or I_DISABLED */
	int (*service)();	/* handler service */
	int (*enable)();	/* handler enable function */
	int (*disable)();	/* handler disable function */
} Ihandler;

typedef struct Ilevel {
	int icount;		/* number of initialized interrupts */
	Ihandler ihandlers[MAXHANDLERS];
} Ilevel;

/* level 1 interrupts */

#define AUXTX_LEVEL	0	/* AUX EIA transmit interrupt */	
#define AUXTX_INDEX	0	/* see auxtrint() in $Libsys/acia.c */

#define HOSTTX_LEVEL	0	/* MAIN EIA transmit interrupt */
#define HOSTTX_INDEX	1	

#define MBUTTON_LEVEL	0	/* mouse button interrupt */
#define MBUTTON_INDEX 	2

/* level 2 interrupts */
#define KBD_LEVEL	1	/* keyboard interrupt */
#define KBD_INDEX	0

#define MOUSE_LEVEL	1	/* mouse tracking (and misc) interrupt */
#define MOUSE_INDEX	1

/* level 3 interrupts */
#define CLOCK_LEVEL	2	/* real time interrupt */
#define CLOCK_INDEX	0

/* level 4 interrupts */
#define AUXRX_LEVEL	3	/* AUX EIA receive interrupt */
#define AUXRX_INDEX	0

/* level 5 interrupts */
#define HOSTRX_LEVEL	4	/* MAIN EIA receive interrupt */
#define HOSTRX_INDEX	0

/* level 6 interrupts */
#define SSI_LEVEL	5	/* SSI interrupt */
#define SSI_INDEX	0

/* level 7 interrupts */
#define BOOT_LEVEL	6	/* reboot trap */
#define BOOT_INDEX	0

