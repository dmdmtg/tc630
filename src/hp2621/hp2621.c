/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)hp2621.c	1.1.1.2	(11/4/87)";

#include "hp2621.h"
#ifdef DMD630
#include <object.h>
Point btoc(), ctob(), fPt();
#endif

/* Main gets called by loader.  Main then calls realmain with a */
/* value of TRUE which indicates that this is the original layer. */
main(argc, argv)
int	argc;
char	*argv[];
{
  /* if there is an argument present, it is a char string to */
  /* write to the host each time we start a new layer. */
  Point p;
  if (argc >= 2) {
    s_size = strlen(argv[1]);
    if (s_size > 1) {
      if (s_size > STARTUP) {
	s_size = STARTUP - 1;
      }
      strncpy(startup, argv[1], s_size);
      startup[s_size++] = '\n';
    }
    else {
      s_size = 0;
    }

    if (argc == 3) {
      f_size = strlen(argv[2]);
      if (f_size > 1) {
	firsttime = argv[2];
      }
      else {
	f_size = 0;
      }
    }
  }
#ifdef DMD630
  P->btoc = btoc;
  P->ctob = ctob;
  p = btoc(display.rect.corner.x - display.rect.origin.x,
     display.rect.corner.y - display.rect.origin.y);
  setjwin(p.x, p.y);
  realmain(cache("hp2621", A_SHARED) ? TRUE : FALSE);
#else
  realmain(TRUE);
#endif
}

#ifdef DMD630
Point
btoc(x,y)
int x,y;
{
  x = (x - (XMARGIN + INSET) * 2) / CW;
  y = (y - (YMARGIN + INSET) * 2) / NS;
  return(fPt(x,y));
}

Point
ctob()
{
  int x, y;

  x = CW * 80 + XMARGIN * 2 + INSET * 2;
  y = NS * 24 + YMARGIN * 2 + INSET * 2;
  return(fPt(x,y));
}
#endif
