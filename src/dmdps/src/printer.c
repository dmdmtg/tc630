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
#include "pfd.h"

extern flow;	
extern struct Printerdefs pfd;

int pbits = 0, 
    pspeed = 0; 

throttle()
{
	static int count;

	if ( flow ) {
		if (count++ >= pfd.flowchars) {
			sleep(pfd.flowticks);
			count=0;
		}
	}
}

shipstr(s)
char *s;
{
	for(;*s;s++) shipchar(*s);
}

shipnchar(n,s)
int n;
char s[];
{
	int i;
	for(i=0;i<n;i++) 
		shipchar(s[i]);
}

shipchar(c)
char c;
{
#ifndef DMD630
	register fudge, i, b;

	/*
	* Some printers can't do 8 bit mode, even parity. (HPink, T5300).  
	* For those, we present 7 bit, odd parity which we transmit
	* over the UART in 8 bit mode. The stupid old keyboard that
	* we share the UART with only knows 8 bit/even. We send
	* 8 bit even to the printer port, but we set the parity of the first
	* 7 bits in the 8th bit. When the UART generates the even parity in 
	* the 9th bit, it will always be set.
	*	 
	* In other words, this makes the output from the send only port look
	* like 7 bits + odd parity + 2 stop bits (extra one is ignored)
	*/
	
	if( pbits == 7 ) {
		for(i=0,fudge=0,b=c; i<7 ; i++) {
			if(b & 0x01)
				fudge++;
			b >>= 1;
		}
		if((fudge&0x01)==0)	/* set 7 bit parity. */
			c |= 0x80;
	}
	psendbps(c,pspeed);	

	if (flow)
		throttle();
}

psendbps(c,bps)
char c;
int bps;
{
	int cmd;


	if (bps == 9600)
		cmd=BD9600BPS;
	else if (bps == 1200)
		cmd=BD1200BPS;
	else if ( pspeed == 4800 )	{ /* psendchar does it better. */
		psendchar(c);
		return;
	}

	pwait(); /* wait for port to free */

	/* program the chip. see the spec sheet if you care. */
	DUART->b_cmnd = ENB_TX;
	if (DUART->b_sr_csr & (XMT_EMT|XMT_RDY)) {
		DUART->scc_sopbc = 0x08; /* switch to printer */
		DUART->b_sr_csr = BD4800BPS & 0xf0 | cmd & 0xf;
		DUART->b_data = c;	 /* send data */
	}
}

pwait()
{
	while ( !(DUART->b_sr_csr & (XMT_EMT|XMT_RDY)) )
		wait(CPU);
#endif
}
