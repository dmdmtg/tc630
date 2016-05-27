/*      Copyright (c) 1987 AT&T */
/*        All Rights Reserved   */

/*      THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T     */
/*      The copyright notice above does not evidence any        */
/*      actual or intended publication of such source code.     */

/* @(#)proto.c	1.1.1.1	(5/2/89) */

#include <signal.h>
#include <stdio.h>
#include "proto.h"


Pchannel pconvs[NPCBUFS];	/* Array of conversations */
Pchannel *pconvsend;		/* Pointer to current pconvs */



char nulls[MAXPKTSIZE-PKTHDRSIZE];	/* synchronize purpose? */

enum {
rpkts, badack, rack, rnack, unkack, xpkts, noack, timeout, dsize, speed, nstats
};

#define RPKTS		(int)rpkts
#define	BADACK		(int)badack
#define	RACK		(int)rack
#define RNACK		(int)rnack
#define	UNKACK		(int)unkack
#define	XPKTS		(int)xpkts
#define	NOACK		(int)noack
#define	TIMEOUT		(int)timeout
#define	DSIZE		(int)dsize
#define	SPEED		(int)speed
#define	NSTATS		(int)nstats

struct {
	char	*desc;
	long	count;
} stats[NSTATS] = {
	{"packets received"},
	{"unrecognized packets"},
	{"acks received"},
	{"nacks received"},
	{"unknown ack"},
	{"packets transmitted"},
	{"packets retransmitted by null acknowledgement"},
	{"packets retransmitted by timeout"},
	{"max packet data size"},
	{"bytes/sec."},
};
#define	STATS(A)	stats[A].count++


#define NUMCHANS	2
#define MAXRCVSIZE	32
#define PR_NULL		0
#define PR_SIZE		1
#define PR_DATA		2

/*
**	MAXRCVSIZE Kludge
**
**	The maximum receive packet size has been kludged to be
**	32 bytes. Normally, the terminal never sends a packet over
**	4 bytes long. The maximum was reduced from 120 so that
**	a packet with a bad size will be detected sooner. The
**	timeout method does not reset precv so the full packet
**	must be received to procede.
*/

int State;
int Chan, Seq, Size, Control;
char Data[PKTHDRSIZE + MAXRCVSIZE + PKTCRCSIZE];
char *pbufp;
int dcount;



/*
**	Nonmux download protocol initialization
**	for the Great Pumpkin (a.k.a. 630 DMD)
*/
void
pinit (lspeed, maxpktdsize)
int lspeed;
int maxpktdsize;
{
	/* ptimeout();		/* just set timeout */
	(void)signal(SIGALRM, ptimeout);	/* set the timeout for retransmission */

	stats[DSIZE].count = maxpktdsize;
	stats[SPEED].count = lspeed;
}

/*
**	Send data to the 630 DMD
**	through fake xt nonmux channel
**
**	Assumes count <= MAXPKTDSIZE
**	    and channel < NONMUXCHAN
**
**	Returns -1 if no more available transmit packets
**	otherwise returns 0.
*/

psend (chan, bufp, count)
int chan;
register char *bufp;
register int count;
{
	register int	i;
	register Pchannel *pcp = &pconvs[chan];
	register Packet *pkp = pcp->pkts;


	for (i=0; i<NPCBUFS; ++i, ++pkp) {	/* look in all packets */
		if (pkp->state != WAIT) {	/* for one available */
			pkp->state = WAIT;
			break;			/* found one, so stop looking */
		}
	}
	if (i>=NPCBUFS)		/* all packets are being used */		
		return (-1);	/* return error */

	/* packet setup */
	memcpy (&pkp->packet[PKTHDRSIZE], bufp, count);

	pkp->packet[0] = CNTLMASK | ((chan<<3)&CHANMASK) | ((pcp->xseq++)&SEQMASK);
	pkp->packet[1] = count;				/* size */
	crc (pkp->packet, count+PKTHDRSIZE);		/* crc's */

	/* packet ready, send down the line to the terminal */
	count += (PKTHDRSIZE+PKTCRCSIZE);
	pkp->size = count;
	realwrite (pkp->packet, count);
	STATS (XPKTS);		/* packets transmitted */
	pconvsend = pcp;	/* current channel in use */
	return 0;
}


/*
**	Receive packetized data from the 
**	630 DMD and depacketize it.
**
**	Two types of data are expected:
**	- send data from DMD 
**	- acknowledge data
*/
char *
precv (c)
{

	switch (State) {
		case PR_NULL:
			Data[0] = c;
			Chan = (c>>3) & 0x07;
			Seq = c & 0x07;
			Control = c & 0x40;

			if (Chan >= NUMCHANS || !(c&0x80)) {
				STATS (BADACK);
				break;
			}
			State = PR_SIZE;
			return ((char *)0);

		case PR_SIZE:
			Data[1] = c;
			Size = c;
			dcount = c + 2;		/* data size + 2 crc's */
			if (Size > MAXRCVSIZE) {
				STATS (BADACK);
				break;
			}
			pbufp = Data + 2;
			State = PR_DATA;
			return ((char *)0);

		case PR_DATA:
			*pbufp++ = c;
			if (--dcount > 0)
				return ((char *)0);

                        /* Check for the CRC's */
                        if (crc (Data, Size+PKTHDRSIZE)) { /* error if non-null */
                                STATS (BADACK);
                                break;
                        }

			State = PR_NULL;
			if (Control && Size == 0 || Data[2] == ACK) {
				ackon (Chan, Seq);
				return ((char *)Data+2);
			}
			else if (Control && Data[2] == NAK) {  /* terminal buffer overflow */
				nackon (Chan, Seq);
				return ((char *)Data+2);
			}
			else 
			/*if ((Data[2]==C_SENDCHAR)||(Data[2]==C_SENDNCHARS)) */ {
				if (Seq == pconvs[Chan].rseq) { /* correct sequence */
					STATS (RPKTS);
					sendack (Chan, Seq);
					pconvs[Chan].rseq = (Seq+1)%SEQMOD;
					return (Data+3);
				}
				if (Seq == (((int)pconvs[Chan].rseq)+SEQMOD-1)%SEQMOD) {
					sendack (Chan, Seq); /* retransmitted */
					return((char *)0);
				}
				sendnak (Chan, Seq); /* out of sequence */
				return((char *)0);
			}
		}

	State = PR_NULL;
	return ((char *)0);
}

nackon (channel, seq)
int channel, seq;
{
	register n;

	STATS (RNACK);
	n = retransmit (&pconvs[channel], seq);
	while (n--)
		STATS (NOACK);
}


retransmit (pcp, seq)
register Pchannel *pcp;
register seq;
{
	static int pseqtable[] = {0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7};
	register Packet *pkp;
	register int n, i, curseq;

	/* find the earliest unacked packet */
	for (n=0, i=NPCBUFS-1; i>=0; i--) {
	    curseq = pseqtable[(seq&SEQMASK)+SEQMOD-i];
	    for(pkp = pcp->pkts; pkp < &pcp->pkts[NPCBUFS]; pkp++)
		if ((pkp->state == WAIT) && ((pkp->packet[0]&SEQMASK) == curseq)) {
			realwrite (pkp->packet, pkp->size);
			n++;
		}
	}

	return (n);
}




ackon (channel, seq)
int channel, seq;
{
	register Pchannel *pcp = &pconvs[channel];
	register Packet *pkp = pcp->pkts;
	register int pseq;

	for (; pkp < &pcp->pkts[NPCBUFS]; pkp++) {
		if (pkp->state == WAIT) 
			if ((pseq = (pkp->packet[0]&SEQMASK)) == seq) {
				STATS (RACK);	
				pkp->state = OK;
				return;
			}
			else if (pseq == (seq-1)) {	/* skipped sequence ack */
				STATS (NOACK);	
				realwrite (pkp->packet, pkp->size);	/* retransmit */
				return;
			}
	}
}

sendack (channel, seq)
int channel, seq;
{
	static char ackstrg[] = {'\0', '\0', '\0', '\0'};

	ackstrg[0] = '\300';
	ackstrg[0] |= (channel<<3);
	ackstrg[0] |= seq;
	crc (ackstrg, PKTHDRSIZE);

	realwrite (ackstrg, 4);
}

sendnak (channel, seq)
int channel, seq;
{
	static char nakstrg[] = {'\0', '\1', (char)NAK, '\0', '\0'};

	nakstrg[0] = '\300';
	nakstrg[0] |= (channel<<3);
	nakstrg[0] |= seq;
	crc (nakstrg, PKTHDRSIZE);

	realwrite (nakstrg, 5);
}



waitack (channel)
int channel;
{
	if (pconvs[channel].pkts[0].state == WAIT)
		return (1);
	else
		return (0);
}



/*
**	Timeout retransmission of the earliest unacknowledged packet
**
**	NOTE: only one channel can be active at any time.
*/
void
ptimeout()
{
	register n;
	register seq;

	(void)signal (SIGALRM, SIG_IGN);	/* ignore all signals */

	seq = (pconvsend->xseq - 1) & SEQMASK;
	n = retransmit (pconvsend, seq);
	while (n--)
		STATS (TIMEOUT);

	(void)signal (SIGALRM, ptimeout);	/* reset timeout */
}



/*
**	Print protocol statistics
*/
void
pstats (fd)
FILE *fd;	/* output file */
{
	register int i;

	fprintf (fd, "\n\nStatistics:\n");
	for (i=0; i<NSTATS; i++)
	    if (stats[i].count)
		fprintf (fd, "\t%ld %s\n", (long)stats[i].count, stats[i].desc);
}


/*
**	Check if all packets are acknowledged
*/
checkpkts (chan)
register chan;
{
	register i;
	register Pchannel *pcp = &pconvs[chan];

	for (i = 0; i < NPCBUFS; ++i) {
		if (pcp->pkts[i].state == WAIT)
			return (1);
	}
	return (0);
}
