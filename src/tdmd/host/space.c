/*       Copyright (c) 1987 AT&T   */
/*       All Rights Reserved       */

/*       THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T   */
/*       The copyright notice above does not evidence any      */
/*       actual or intended publication of such source code.   */


static char _2Vsccsid[]="@(#)space.c	1.1.1.3	(11/13/87)";

/* includes */
#include "jplot.h"


/*
 * The purpose of the space command is twofold: first, to provide the
 * information necessary for scaling, and second, to correct for display
 * devices that transform the data in one of four ways.
 *
 * Scaling information is derived from the *absolute* difference between
 * the origin and the corner specified on the space command.
 *
 * The origin of the space command also specifies how far the origin used
 * by the data is displaced from 0,0.  The purpose of this point is to
 * provide information about how the device assumed by the data would
 * transform the data.
 *
 * Devices transform the data in one of four ways.  Each of these
 * transformations correspond to one of the four corners of the screen.
 * If the data and the display device both expect that the orgin is at
 * 0,0, at the lower-left corner of the screen, the transformation is
 * basically a no-op.  But if the data was created, e.g. for a device that
 * paints from the lower-left corner of the screen, and the device used
 * to display it starts from the upper-left corner of the screen, the
 * image will be inverted.
 * 
 * When the origin is displaced, the data can be thought as representing
 * distances *toward* the origin, instead of away from it.  Therefore,
 * to find the real value of the coordinate, it should be subtracted
 * from the amount of the displacement.  Those axii that are not
 * displaced, however, can be used directly.
 *
 * Both the absolute scale factor, and an adjustment factor used to
 * prepare the coordinates for calculating the correct position, are
 * incorporated in scalex and scaley.
 * 
 * The adjustment factor makes those coordinates negative whose origins are
 * displaced, so that when they are added to the displacement, the distance
 * from the actual origin are determined.
 *
 * See the algebra at xysc().
 */
void
space(x0,y0,x1,y1)
{
	/*
	 * porig is the origin assumed by the input data.
	 */
	porigx = x0;
	porigy = y0;

	/*
	 * the scale factor is the ratio between the 630's screen
	 * size, and the space that the data assumes.  scalex and
	 * scaley also hold the adjustment factors for transforming
	 * the data for use on the 630 dmd.
	 */
	scalex = deltx/(x1-x0);
	scaley = delty/(y1-y0);

}
