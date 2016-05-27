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

#include "icon/tile_a"
#include "icon/tile_b"
#include "icon/tile_c"
#include "icon/tile_d"
#include "icon/tile_e"
#include "icon/tile_f"
#include "icon/mouse1"
#include "icon/mouse2"
#include "icon/mouse3"
#include "icon/blank"
#include "icon/f1"
#include "icon/f2"
#include "icon/l1"

#define IPt(x,y) Pt( ( (y)==1 ? 9+48*(x) : 28+48*(x) ) ,12)

Bitmap	*Tiles = (Bitmap *) NULL,
	*Tile  = (Bitmap *) NULL;

#define TILES_ACROSS	15
#define TILES_DOWN	2

Texture16 *pattern[TILES_DOWN][TILES_ACROSS] = {
{ &tile_a,&tile_b,&tile_c, &tile_a,&tile_b,&tile_c, &tile_a,&tile_b,&tile_c, 
	&tile_a,&tile_b,&tile_c, &tile_a,&tile_b,&tile_c},
{ &tile_d,&tile_e,&tile_f, &tile_d,&tile_e,&tile_f, &tile_d,&tile_e,&tile_f,
	&tile_d,&tile_e,&tile_f, &tile_d,&tile_e,&tile_f},
};

mk_mouseboard()
{
register i,j;

if (Tiles == (Bitmap *) NULL)
	Tiles = balloc(Rect(0,0,TILES_ACROSS*16,TILES_DOWN*16));

if (Tile == (Bitmap *) NULL)
	Tile  = balloc(Rect(0,0,16,16));

for ( i=0 ; i<TILES_DOWN ; i++ )
	for ( j=0 ; j<TILES_ACROSS ; j++ ) 
		placetile(pattern[i][j], Pt(j*16,i*16));

placetile(&mouse1, IPt(0,1));
placetile(&mouse2, IPt(1,1));
placetile(&mouse3, IPt(2,1));
placetile(&l1, IPt(3,1));
placetile(&f2, IPt(3,2));
placetile(&f1, IPt(4,1));
placetile(&f2, IPt(4,2));
}

fill_mouseboard(icon1,icon2,icon3)
Texture16 *icon1, *icon2, *icon3;
{
	if ( icon1 == (Texture16 *) NULL) 
		placetile(&blank, IPt(0,2));
	else	placetile(icon1, IPt(0,2));

	if ( icon2 == (Texture16 *) NULL) 
		placetile(&blank, IPt(1,2));
	else	placetile(icon2, IPt(1,2));

	if ( icon3 == (Texture16 *) NULL) 
		placetile(&blank, IPt(2,2));
	else	placetile(icon3, IPt(2,2));
}

show_mouseboard(pnt)
Point pnt;
{
    bitblt(Tiles, Tiles->rect, &display, Drect.origin, F_STORE);
}

placetile(tile,position)
Texture16 *tile;
Point position;
{
#ifdef DMD630
	texture( Tile, Tile->rect, tile, F_STORE);
#else
	texture16( Tile, Tile->rect, tile, F_STORE);
#endif
	bitblt( Tile, Tile->rect, Tiles, position, F_STORE);
}
