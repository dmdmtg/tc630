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
#include <font.h>

char *outstr;

sprintc (str, fmt, args)
char *str;
char *fmt;
unsigned args;
{
    register c;
    unsigned *p = &args;
    char *s;
    int fieldwidth,i;

    static char ddigits[] = "0123456789";
    static char odigits[] = "01234567";
    static char hdigits[] = "0123456789abcdef";
    
	outstr=str;
	while ( c = *fmt++ )
		switch (c) {	
		case '\\': /* quote char */
			c = *fmt++;
			if ( c >= '0' && c <= '7' ) { /* octal constant */
				i=0;
				do { 	
					i = i*8 + (c-'0'); 
					c = *fmt++;
				} while (c >= '0' && c <= '7'); 
				*outstr++ = (char) i&0x0ff;
				fmt--; /* put last char back. */
			} else
				*outstr++=c;
			break;
		case '%':
			c = *fmt++;
			if ( c >= '0' && c <= '9') { /* decimal fieldwidth */
				fieldwidth=0;
				do { 	
					fieldwidth  = fieldwidth*10 + (c-'0'); 
					c = *fmt++;
				} while (c >= '0' && c <= '9'); 
			} else 	
				fieldwidth = -1;

			switch (c) {
	 		case 'd': /* decimal */
				lputnum (*(int *)p++,10,ddigits,fieldwidth);
				break;
			case 'o': /* octal */
				lputnum (*(int *)p++,8,odigits,fieldwidth);
				break;
			case 'x': /* hexadecimal */
				lputnum (*(int *)p++,16,hdigits,fieldwidth);
				break;
			case 'u': /* unsigned decimal */
				lputnum (*(int *)p++,0,ddigits,fieldwidth);
				break;
			case 'c': /* single character */
				if (fieldwidth == -1 ) {
					*outstr++ =((char *)p++)[3];
				} else if (fieldwidth == 2) {
					*outstr++ =((char *)p  )[3];
					*outstr++ =((char *)p++)[2];
				}
				break;
			case 's': /* string */
				s = *(char **)p++;	/* don't copy /0 */
				while ((char)(c = *s++))
					*outstr++ = (char) c;
				break;
			default: /* not conversion char */
				*outstr++=c;
				break;
			}
			break;
		default:
			*outstr++=c;
			break;
		}

	*outstr='\0';
	return outstr-str;
}

lputnum (n,base,s,fieldwidth)
register long n;
unsigned base;
char s[];
int fieldwidth;
{  
	if ( (fieldwidth == 0) || ((fieldwidth < -1 ) && (n == 0)))
		return;

	if (n<0 && base == 10)
	{	*outstr++='-';
		n = -n;
	}
	lputnum (n/base, base, s, --fieldwidth);
	*outstr++ = s[n%base];
}

