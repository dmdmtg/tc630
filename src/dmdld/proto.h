/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */

/* @(#)proto.h	1.1.1.1	(5/2/89) */

/* 
**	Nonmux download protocol for the Great Pumpkin (630 DMD).
**
**	The protocol fakes the xt protocol in order to use the
**	already built-in protocol functions in the terminal.
*/ 

typedef	unsigned char	Pbyte;		/* The unit of communication */


#define NONMUXCHAN	2		/* Number of "fake" channels */
#define SEQMOD		8		/* Sequence number modulus */
#define	NPCBUFS		2		/* Double buffered protocol */
#define	PKTHDRSIZE	2		/* Seq + Size */
#define	PKTCRCSIZE	2		/* CRC 16 */
#define PKTASIZE	4		/* Address field for takeover download */
#define MAXPKTSIZE	124		/* Efficient size for system */
#define	MAXPKTDSIZE	MAXPKTSIZE-(PKTHDRSIZE+PKTCRCSIZE)

/*
 * should be at least 4K to get maximum throughput to 730 over the streams
 *   XT-driver's network XT and TCP/IP because 730's TCP delays up to 200ms
 *   before sending TCP ACK.
 */
#define DATASIZE	8192

/* 
**	Packet header mask
**	The header could be defined as a "field", but there is
**	some problem with machine dependency.
*/
#define CNTLMASK	0x80		/* cntl=0, not a control packet */
#define CHANMASK	0x38		/* channel mask */
#define SEQMASK		0x07		/* sequence mask */

/* 
**	Packet states
*/
#define	AVAIL	0
#define	WAIT	1
#define	OK	2

/*
**	Definition of a structure to hold status information
**	for a conversation with a channel.
*/

typedef struct Packet
{	char packet[MAXPKTSIZE];	/* The packet */
	unsigned char state;		/* Packet state */
	unsigned char size;		/* Packet size */
} Packet;


typedef struct Pchannel
{	struct Packet pkts[NPCBUFS];	/* The packets (double buffered) */
	unsigned char xseq;		/* Next transmit sequence number */
	unsigned char rseq;		/* Next receive sequence number */
} Pchannel;



/*
**	Transmit 
*/
extern Pchannel pconvs[];		/* Array of conversations */
extern Pchannel *pconvsend;		/* Pointer to current pconvs */


/*
**	Interface routines
*/
extern int psend();			/* Send data to channel */
extern char *precv();			/* Receive data/control from channel */
extern void ptimeout();			/* Catch alarm timeouts */
extern void pinit();			/* Initialise conversations */


/* this is used in writeswap() only, when the protocol is single buffered
*/
#define checkseq(chan)	pconvs[chan].xseq-1


#define C_SENDCHAR	1
#define C_SENDNCHARS	7


#define ACK		(unsigned char)(006)
#define NAK		(unsigned char)(025)

#define JBOOT_CHAR	1
#define JTERM_CHAR	2
#define JZOMBOOT_CHAR	7
