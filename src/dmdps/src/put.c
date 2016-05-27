/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)put.c	1.1.1.7	(8/12/87)";

#include <dmd.h>
#include <dmdio.h>
#define _SYSWRITE	3

putchar(c)
{
	return putc(c,stdout);
}

putc(c,f)
register FILE *f;
{
	register i = 0;

	if(whathost() == -1)
		return(-1);
	*f->cp++ = c;
	if (--f->count == 0 || ((c == '\n') && isatty(f)))
		i = fflush(f);
	if(i == -1)
		return(i);
	return(0xff&c);
}

fflush(f)
register FILE *f;
{
	register int n, r;

	n = r = 0;
	if ((f->flag & _IOWRT) && ((n = f->cp - f->base) > 0))
		r = _write(fileno(f),f->base,n);
	f->cp = f->base;
	f->count = BUFSIZ;
	if(r != n)
		f->flag |= _IOERR;
	return((r == n) - 1);
}

puts(s)
register char *s;
{
	if(fputs(s,stdout) != -1)
		if(putc('\n', stdout) == '\n')
			return(0);
	return(-1);
}

fputs(s,f)
register char *s;
register FILE *f;
{
	register int c;
	while (c = *s++)
		if(putc(c,f) == -1)
			return(-1);
	return(0);
}

_write(fd,buf,n)
char *buf;
{
	if(whathost() == -1)
		return(-1);
	tag(_SYSWRITE);
	jputshort(fd);
	jputbuf(buf,n);
	return(jgetshort());
}

