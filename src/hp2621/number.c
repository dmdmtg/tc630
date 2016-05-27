/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)number.c	1.1.1.1	(11/4/87)";

#include "hp2621.h"

/* Extracts a number from the input stream.  This number is returned */
/* and the terminating character is returned in 'p'. */
/* If number is accompanied by a plus or minus sign then the */
/* variable 'relative' is set to true. */

short
number (up, p, relative)
register struct User *up;
char *p;
BOOLEAN *relative;
{
  register n = 0;
  register c;
  short sign = 0;

  for (;;) {
    c = getnxtchar (up, 0);
    if (c >= '0' && c <= '9') {
      n = n*10 + c - '0';
    }
    else {
      switch (c) {
	case '+': {
	  sign = 1;
	  break;
	}
	case '-': {
	  sign = -1;
	  break;
	}
	default: {
	  *p = c;
	  *relative = sign;
	  return ((sign)?sign*n:n);
	}
      }
    }
  }
}
