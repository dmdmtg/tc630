/*	Copyright (c) 1987 AT&T	*/
/*	All Rights Reserved	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
	host <-> terminal protocol
*/

/* static _comm_sccsid[]="@(#)comm.h	1.1.1.3	(6/23/88)"; */

#define REQ	ACK	/* host->term; asks for a NAK \n if you are proof */
#define NAK	025	/* term->host; yes I am proof; goes into troff mode */
#define ACK	017
#define EOT	04	/* end of text */

#define	FRAME	0367	/* host->term; draw a frame */
#define	ASCII	0366	/* host->term; go into ascii mode */
#define EXIT	0365	/* host<->term; bugger off */
#define GETFONT	0364	/* term->host (n); respond NAK or ACK+font */
#define POS	0363	/* host->term (pt); set current point */
#define RIGHT	0362	/* host->term (n); move right n points (signed) */
#define FONT	0361	/* host->term (n); set current font to number n */
#define NEWFONT	0360	/* host->term (string); load font- respond ACK n or GETFONT */

#define CIRCLE	0357	/* host->term (pt, sh); */
#define ELLIPSE	0356	/* host->term (pt, sh, sh) */
#define SPLINE	0355	/* host->term (n, pt1, pt2, .., ptn); */
#define ARC	0354	/* host->term (pt, pt, pt); */
#define LINE	0353	/* host->term (pt, pt); */
#define PAGE	0352	/* host->term; new page- respond ACK or EXIT or PAGE n */
#define CLEAR	0351	/* host->term; clear the screen */
#define WIND	0350	/* term->host (n, n); set window size in bits */

#define SLICE	0347	/* term->host; like PAGE except just a slice */
#define SCALE	0346	/* term->host; set to scaled mode */
#define WINDOW	0345	/* term->host; set to window mode */


#define NFONTS	50
#define MAXPAGE 0337	/* only used in term/slave.c */
