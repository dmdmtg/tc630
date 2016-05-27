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
/*	/cras0/usr/bjb/uucp/blitblt/s.imPRESS.h
	imPRESS.h	1.2	7/29/83 00:29:41
	replace these words with a descriptive comment
*/
static	char	h_imPRESS[] = "@(#)  imPRESS.h 1.2";

/* imPRESS.h  --  imPRESS opcodes */

#define	SP	128
#define	SP1	129
#define	OLDMMOVE	130
#define	MPLUS	131
#define	MMINUS	132
#define	MMOVE	133
#define	SMOVE	134

#define	SETABSH	135
#define	SETRELH	136
#define	SETABSV	137
#define	SETRELV	138

#define	SRULE	192
#define	BRULE	193

#define	SETHPOS	195
#define	SETVPOS	196
#define	CRLF	197
#define	SGLY	198
#define	BGLY	199
#define	DELG	200
#define	DELC	201
#define	DELF	202

#define	SETHVSYSTEM	205
#define	SETADVDIRS	206
#define	SETFAMILY	207
#define	SETIL	208
#define	SETBOL	209
#define	SETSP	210
#define	PUSH	211
#define	POP	212
#define	PAGE	213
#define	SETPUSHMASK	214
#define	ENDPAGE	219

#define	CREATFAMILYTABLE	221
#define	CREATMAP	222

#define	CREATPATH	230
#define	SETTEXTURE	231
#define	SETPEN	232
#define	FILLPATH	233
#define	DRAWPATH	234
#define	BITMAP	235
#define	SETMAGN	236

#define	imPEOF	255
