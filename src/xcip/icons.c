/* Define all textures, cursors and bitmaps here.  */
/*                                                 */
/* For X11, these must be sent to the X server.    */

#include "cip.h"


/**** crossHair cursor ****/
#ifdef X11
    Cursor crossHairs;
    unsigned short crossHairs_bits[] = {
#else
    Texture16 crossHairs = {
#endif
         0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0000, 0xFC7E,
         0x0000, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x0000,
    };


/**** hourglass cursor ****/
#ifdef X11
    Cursor hourglass;
    unsigned short hourglass_bits[] = {
#else
    Texture16 hourglass = {
#endif
         0x7FFE, 0x7FFE, 0x2814, 0x2814, 0x27E4, 0x27E4, 0x23C4, 0x2184,
         0x2184, 0x2244, 0x24A4, 0x2424, 0x2894, 0x29D4, 0x7FFE, 0x7FFE,
    };


/**** rusure cursor ****/ 
#ifdef X11
    Cursor rusure;
    unsigned short rusure_bits[] = {
#else
    Texture16 rusure = {
#endif
	0X0000, 0X0EA0, 0X0AA0, 0X08A0, 0X08A0, 0X08E0, 0X0000, 0X0000,
	0XEAEE, 0X8AAA, 0XEA8E, 0X2A88, 0XEE8E, 0X0000, 0X0000, 0X0000,
    };


/**** textCursor cursor ****/
#ifdef X11
    Cursor textCursor;
    unsigned short textCursor_bits[] = {
#else
    Texture16 textCursor = {
#endif
        0x0000, 0x0000, 0x0000, 0xEE97, 0x4892, 0x4862, 0x4C62, 0x4C62,
        0x4862, 0x4892, 0x4E92, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    };



/**** grid texture ****/
#ifdef X11
    Texture grid;
    unsigned short grid_bits[] = { 0,0,0,0,0,0x0200,0,0, 0,0,0,0,0,0,0,0 };
#else
    Texture grid;
#endif


/**** grid8 texture ****/
#ifdef X11
    Texture grid8;
    unsigned short grid8_bits[] = {0,0,0,0,0,0x0202,0,0,0,0,0,0,0,0x0202,0,0};
#else
    Texture grid8;
#endif


#ifdef FINEALIGN
/**** fineGrid texture ****/
#ifdef X11
    Texture fineGrid;
    unsigned short fineGrid_bits[] = { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 };
#else
    Texture fineGrid;
#endif /* X11 */
#endif /* FINEALIGN */

/**** copyright icon ****/
#ifdef X11
    Texture copyright;
    unsigned short copyright_bits[] = {
#else
    Texture16 copyright = {
#endif
	0x0000, 0x0380, 0x0C60, 0x1010, 0x2388, 0x2448, 0x4404, 0x4404,
	0x4404, 0x2448, 0x2388, 0x1010, 0x0C60, 0x0380, 0x0000, 0x0000,
    };




/**** lowerTriangle icon ****/
#ifdef X11
    Texture lowerTriangle;
    unsigned short lowerTriangle_bits[] = {
#else
    Texture16 lowerTriangle = {
#endif
         0x8000, 0xC000, 0xE000, 0xF000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    };


/**** upperTriangle icon ****/
#ifdef X11
    Texture upperTriangle;
    unsigned short  upperTriangle_bits[] = {
#else
    Texture16 upperTriangle = {
#endif
         0xF000, 0x7000, 0x3000, 0x1000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    };



/**** Markerbm & markermaskbm icon ****/

/* Note: for DMDs, a different approach is used. */
#ifdef X11
    Bitmap markerbm;
    unsigned short marker_bits[] = {      /* marker icon */
         0x0000, 0x0000, 0x0000, 0x0080, 0x0080, 0x01C0, 0x01C0, 0x03E0,
         0x03E0, 0x07F0, 0x07F0, 0x0FF8, 0x0FF8, 0x0FF8, 0x0FF8, 0x0000,
    };

    Bitmap markermaskbm;
    unsigned short markerMask_bits[] = {  /* one bit bigger marker icon */
         0x0000, 0x0000, 0x0080, 0x01C0, 0x01C0, 0x03E0, 0x03E0, 0x07F0,
         0x07F0, 0x0FF8, 0x0FF8, 0x1FFC, 0x1FFC, 0x1FFC, 0x1FFC, 0x1FFC,
    };

    Bitmap bgsave;
    unsigned short plain_bits[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    };
#endif


#ifdef X11

	void 
initGraphics()
{
	crossHairs = ToCursor((short *)crossHairs_bits, 
			      (short *)crossHairs_bits, 7, 7);
	hourglass  = ToCursor((short *)hourglass_bits, 
			      (short *)hourglass_bits, 7, 7);
	rusure     = ToCursor((short *)rusure_bits, (short *)rusure_bits, 7, 7);
	textCursor = ToCursor((short *)textCursor_bits, 
			      (short *)textCursor_bits, 7, 7);

	grid  = ToTexture((short *)grid_bits );
	grid8 = ToTexture((short *)grid8_bits );
#ifdef FINEALIGN
	fineGrid = ToTexture((short *)fineGrid_bits );
#endif /* FINEALIGN */

	copyright     = ToTexture((char *)copyright_bits );
	lowerTriangle = ToTexture((char *)lowerTriangle_bits );
	upperTriangle = ToTexture((char *)upperTriangle_bits );

	markerbm     = ToBitmap((short *)marker_bits,     2, 0, 0, 16, 16 );
	markermaskbm = ToBitmap((short *)markerMask_bits, 2, 0, 0, 16, 16 );
	bgsave       = ToBitmap((short *)plain_bits,      2, 0, 0, 16, 16 );
}

#endif /* X11 */
