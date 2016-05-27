/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */

/*

static char _2Vsccsid[]="@(#)jx.c	1.1.1.6	(7/21/91)";
*/

/*
 * sysint.c - this guy sits on the host end and interprets
 *	library calls.
 */

#include <stdio.h>
#include "comm.h"
#include "termio.h"
#include "quiet.h"
#include <sys/jioctl.h>
#ifndef TIOCEXCL
#include <sgtty.h>
#endif

int jerqno;
char *getenv(),*cttyname();
FILE *files[8];
FILE *tojerq,*fromjerq;
FILE *popen();
short hex_mode = 0;
short hex_count = 0;
char hex_obuf[BUFSIZ];

main(argc,argv)
int argc;
char *argv[];
{
	hex_mode = (argc > 1 && (strcmp(argv[1],"-h") == 0));
	jerqno = 3;
	fromjerq = fdopen(jerqno, "r");
	tojerq = fdopen(jerqno, "w");

#ifdef TIOCEXCL
	(void) ioctl( jerqno, TIOCEXCL, 0);
#endif
	setbuf(fromjerq,NULL);
	listen();
#ifdef TIOCNXCL
	(void) ioctl( jerqno, TIOCNXCL , 0);	
#endif
	return 0;
}

jputchar(c)
{
	if (!hex_mode)
		putc(c,tojerq);
	else
		hex_obuf[hex_count++] = c;
}

jputshort(s)
short	s;
{
	jputchar(s>>8);
	jputchar(s&0xFF);
}

jputbuf(p,n)
register char *p;
register short n;
{
	jputshort(n);
	while (n-- > 0)
		jputchar(*p++);
}

jputstr(s)
register char *s;
{
	register char c;
	do {
		jputchar(c = *s++);
	} while (c != '\0');
}

jgetchar()
{
	int c;
	static short count;
	static unsigned char temp;

	if ((c = getc(fromjerq)) == EOF)
		exit(2);
	if (!hex_mode)
		return(c);
	else {
		while (c < 0x40 && c != 0x20)
			if ((c = getc(fromjerq)) == EOF)
				exit(3);
		if (c == 0x20) {
			count = 6;
			if ((c = getc(fromjerq)) == EOF)
				exit(4);
		}
		if ((count += 2) == 8) {
			count = 2;
			temp = c;
			if ((c = getc(fromjerq)) == EOF)
				exit(5);
		}
		return((c & 0x3f) | ((temp << count) & 0xc0));
	}
}

short
jgetshort()
{
	short	s;
	s = jgetchar() << 8;
	s += jgetchar();
	return s;
}

jgetbuf(p)
register char *p;
{
	register short i,n;
	n = jgetshort();
	for (i = 0; i < n; i++)
		*p++ = jgetchar();
	return(n);
}

jgetstr(s)
register char *s;
{
	while (*s++ = jgetchar());
}

jfflush()
{
	unsigned char c1, c2, c3;
	char *s;
	short i;

	if (!hex_mode)
		fflush(tojerq);
	else {
		putc(0x20,tojerq);
		for (i = 0, s = hex_obuf ; i < hex_count ; i+=3) {
			c1 = *s++;
			if (i+1 < hex_count)
				c2 = *s++;
			if (i+2 < hex_count)
				c3 = *s++;
			putc(0x40 | (c1&0xc0)>>2 | (c2&0xc0)>>4 | (c3&0xc0)>>6,tojerq);
			putc(0x40 | (c1 & 0x3f),tojerq);
			if (i+1 < hex_count)
				putc(0x40 | (c2 & 0x3f),tojerq);
			if (i+2 < hex_count)
				putc(0x40 | (c3 & 0x3f),tojerq);
		}
		fflush(tojerq);
		hex_count = 0;
	}
}

listen()
{
	int n,c,i;
	char filename[1024],buf[1024],mode[2];
	char vname[1024];
	char *xname;
	files[0] = stdin;
	files[1] = stdout;
	files[2] = stderr;
	mode[1] = '\0';
	jputchar(_SYSINIT);
	jfflush();
	while ((c = jgetchar()) != EOF) {
		while (c != _SYSTAG) {	/* I KNOW it's awful */
			if (c == EOF || c == 0177)
				return(0);
			c = jgetchar();
		}
		c = jgetchar();
		switch (c) {
		case _SYSSYNC:
			jputchar(_SYSSYNC);
			break;
		case _SYSOPEN:
			i = jgetshort();
			jgetstr(filename);
			*mode = jgetchar();
			if ((files[i] = fopen(filename,mode)) == NULL)
				jputshort(-1);
			else
				jputshort(i);
			break;
		case _SYSPOPEN:
			i = jgetshort();
			jgetstr(filename);
			*mode = jgetchar();
			if ((files[i] = popen(filename,mode)) == NULL)
				jputshort(-1);
			else
				jputshort(i);
			break;
		case _SYSCLOSE:
			i = jgetshort();
			jputshort(fclose(files[i]));
			break;
		case _SYSPCLOSE:
			i = jgetshort();
			jputshort(pclose(files[i]));
			break;
		case _SYSWRITE:
			i = jgetshort();
			n = jgetbuf(buf);
			jputshort(fwrite(buf,1,n,files[i]));
			fflush(files[i]);
			break;
		case _SYSEXIT:
			i = jgetshort();
			jputshort(i);
			jfflush();
			return(i);
		case _SYSREAD:
			i = jgetshort();
			n = jgetshort();
			n = fread(buf,1,n,files[i]);
			jputbuf(buf,n);
			break;
		case _SYSACCESS:
			jgetstr(filename);
			i = jgetshort();
			jputshort(access(filename,i));
			break;
		case _GETENV:
			jgetstr( vname ); 
			xname = getenv( vname );
			jputstr( xname );
			break;
		default:
			fprintf(stderr,"unrecognized system call: %d\n",c);
		}
		jfflush();
	}
}



