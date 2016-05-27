/*       Copyright (c) 1989 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */

/* @(#)io.c	1.1.1.1	(5/2/89) */

#include <stdio.h>
#include <sys/jioctl.h>
#include <errno.h>
#include <signal.h>
#include "load.h"
#include "proto.h"

#define MAXRETRIES	10	/* number of retrials before give up */


static int retries;
	
extern int debug;
extern int psflag;
extern int mpx;
extern int Loadtype;
extern void timeout_id();


/* Uread -	Protocol specific read routine
**
**	If the terminal is running under layers, the hex and xt protocol
**	are transparent to "dmdld".
**
**	If the terminal is running in nonmux mode, "dmdld" has to implement
**	the xt-like error correcting protocol and hex encoding if necessary.
**
**	NOTE: this function expects the things sent from the terminal
**	      all fit into one xt packet, which is true with the current
**	      handshake scenario (see main() and load()).
**
**	NOTE: since this is running on a single buffered channel, and the
**	      things read from the terminal are for identification and
**	      handshaking, we use the function "timeout_id" for timeout
**	      instead of "ptimeout" of the xt protocol.
**
*/
Uread (buf, n)
char *buf;
register n;
{
	register i;
	register char *ps;
	unsigned char c;
	void (*func)();

	if (mpx) {
		/* alarm (10);	alarm not needed in mux */
		i = read (0, buf, n);	/* block until data becomes available */
		/* alarm (0);	alarm not needed in mux */
		return (i);
	}
	else {
		if (psflag)
			fprintf (stderr, "\nrecv: ");
		func = (void (*)())signal (SIGALRM, timeout_id);
		alarm (15);	/* should be able to read in 15 secs:
				** used to be 10 secs: not long enough
				*/
		do {
			while (realread ((char *) &c) == -1);
			if (psflag)
				fprintf (stderr, "[%x]", c);
		} while (!(ps = precv(c)));
		alarm (0);
		signal (SIGALRM, func);

		/* WARNING: */
		/* we assume that Uread can read everything in one packet */
		for (i=0; i<n; ++i)
			buf[i] = ps[i];
		return (n);
	}
}

/* realread -	Encoding read routine (for nonmux/takeover only)
**
**	The hex encoding provides a safer download protocol through networks.
**
**	If hex encoding is not set, we read directly from the line.
**	If hex encoding is set, we read until we get 1 byte decoded.
*/
realread (a)
char *a;
{
	static int count;		/* 0 by default */
	static unsigned char temp;
	char c;

	if (Loadtype == BINARY_LOAD) {
	    return (read (0, a, 1));
	}
	else {	/* hex encoding */
	    while (1) {	/* keep reading until we get a byte decoded */		
		read (0, &c, 1);
		if ((c&0xe0) == 0x20 || (count += 2) == 8) {	/* first byte of packet */
			count = 0;
			temp = c;
		}
		else {
			*a = (c & 0x3f) | ((temp << count) & 0xc0);
			return (1);		/* got our byte */
		}
	    }
	}
}


void
Precv ()
{
	unsigned char c;

	alarm (15);	/* sleep at least 2 seconds:
			** used to be alarm(6): not long enough.
			*/

	if (psflag)
		fprintf (stderr, "\nrecv: ");
	while (realread ((char *) &c) == 1) {
		if (psflag)
			fprintf (stderr, "[%x]", c&0xff);
		if (precv (c)) {
			alarm (0);
			return;
		}
	}

	if (errno != EINTR )
		error (1, "read error", (char *)0);
}



/* 
**	SEND/WRITE/TRANSMIT
*/


/* Uwriteswap -		Machine dependent byte ordering send
**
**	Check for machine dependent byte ordering and send to 
**	the terminal through the single buffered channel.
**
**	Strings do not need to be swapped, but numerical values
**	(int, long) do.
*/
int swapdummy;	/* to force the array below to start at even address */
char swapbuf1[DATASIZE+PKTASIZE], swapbuf2[DATASIZE+PKTASIZE];

Uwriteswap (a, n)
char *a;
int n;
{

	swaw ((short *)a, (short *)swapbuf1, n);
	swab (swapbuf1, swapbuf2, n);

	Uwrite (swapbuf2, n);
}

/* Uwrite -	Protocol specific send routine: single buffered channel
**
**	For mux, just use the standard Unix write() function because
**	xt and hex are transparent to dmdld.
**
**	For nonmux, the xt-like error correcting protocol is handled
**	here. This function is used during the handshaking time between
**	host and terminal where the terminal can send information (not
**	acks only) to the host. To simplify things, only a single buffered
**	channel is used. When the host starts the code download, a double
**	buffered channel will be used (Psend), but at that time, the terminal
**	only sends back the acks and naks, so things are also simple.
*/		
Uwrite (a, n)
char *a;
int n;
{
	if (mpx) {		/* mux */
		if (write (1, a, n) != n)
			error (1, "Write error to terminal", (char *)0);
	}

	else {
		writeproto (1, a, n);	/* send on channel 1 */
	}
}

/* writeproto -	single bufferred channel write (nonmux/takeover only)
**
*/
writeproto (chan, a, n)
int chan;
char *a;
int n;
{
	psend (chan, a, n);		/* send the data */
	while (waitack (chan))		/* wait for acknowledgement */
		Precv();		/* look for acknowledgement */
}


/* realwrite -	hex encode if necessary (nonmux/takeover only)
**
**	If hex encoding is not set, just write directly.
**	Otherwise do some encoding before sending.
**
**	This function is called from the xt proto functions and
**	a points to a full xt packet with n as the size of it.
*/
realwrite (a, n)
char *a;
register n;
{
	unsigned char buf[4];
	register unsigned char *q = buf;
	register i, j;

	if (Loadtype == BINARY_LOAD) {
		if (write (1, a, n) != n)
			error(1, "write error to terminal", (char *)0);
	}
	else {		/* hex encoding */
		*q = 0x00;	/* encode first byte of xt packet */
		for (i=0; i<n; ) {
			for (j=1; j<=3; ++j, ++i) {	/* read 3 bytes */
				if (i<n)		/* valid data */
					*(q+j) = *(a+i);
				else {
					*(q+j) = 0;	/* just padding */
					break;
				}
			}
			while (--j) {		/* encode the 3 bytes just read */
				*q |= (((*(q+j))&0xc0)>>(j<<1));
				*(q+j) = 0x40 | ((*(q+j))&0x3f);
			}
			if (write (1, buf, 4) != 4)	/* send the 4 bytes generated */
				error(1, "write error to terminal", (char *)0);
			*q = 0x40;	/* ready for next trio of bytes */
		}

	}

	if (psflag /* && !mpx */)
		trace(a);
}

/* Psend -	Protocol specific send function: double buffered channel
**
**	After the handshaking, the host starts to send data, and the terminal
**	only acks or nacks. This simplicity allows us to double buffer the
**	channel like in "true" xt for more efficiency.
**
**	NOTE: this function is called in nonmux/takeover environment only. 
*/
void
Psend (bufp, count)
char *bufp;
int count;
{
	while (psend (1, bufp, count) == -1)	/* send on channel 1 */
		Precv ();
}



/*
**	MISCELLANEOUS
*/


/*	Waits for all packets acknowledged on channel
*/
getlastacks (chan)
register chan;
{
	retries = 0;
	while (checkpkts(chan))	/* check packets on channel 1 */
		Precv();
}



/*	Fake mux ioctl calls for nonmux environment
*/
nonmuxioctl (f, chan)
register f;
register chan;
{
	static char iostrg[2];

	getlastacks (0);	/* make sure all standing packets get acked */
	getlastacks (1);	/* since we are switching to one-buffer */
				/* conversation with the terminal */

	iostrg[0] = f;
	iostrg[1] = chan;
	if (f != JTERM_CHAR)
		writeproto (0, iostrg, 2);	/* send to nonmux channel 0 */
	else
		psend (0, iostrg, 2);
}
