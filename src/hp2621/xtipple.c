/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)xtipple.c	1.1.1.1	(11/11/87)";

#include "hp2621.h"

/* This routine places a stippled texture into the rectangle */
/* specified by r in the layer specified by P->layer.  No stippling */
/* is used if the user owns the mouse.  Instead a blank */
/* stipple pattern is used.  Note, that when STORE mode is used, */
/* anything that was in the rectangle is wiped out.  */
/* Reverse textures are used if reverse video is set. */

void
xtipple (up, r, mode)
register struct User *up;
Rectangle r;
register int mode;
{
  static Texture16
    plain = { 0 },
    fancy = { 0x1000 },
    rvplain = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
                0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
                0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
                0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
    rvfancy = { 0xEFFF, 0xFFFF, 0xFFFF, 0xFFFF,
                0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
                0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
                0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };
  Texture16 *tp;

  if (up->rvid && mode != F_XOR) {
    tp = &rvplain;
  }
  else {
    tp = &plain;
  }
  rectclip (&r, P->rect);
  texture16 (P->layer, r, tp, mode);
}
