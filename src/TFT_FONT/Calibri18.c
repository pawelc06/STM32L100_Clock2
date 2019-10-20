/*
*  Calibri18pt.c
*  Created on: 2011-12-22 15:45:12
*       Autor: Miros³aw Kardaœ
*       www.atnel.pl
*/


#include "mk_fonts.h"

// Font information for Calibri 18pt
const FONT_INFO Calibri18ptFontInfo =
{
    23,  // Character height
    '!',  //  Start character
    3,  //  number of pixels of interspace between characters
    8,  // number of pixels of space character
    0, // Character descriptor array
    0, // Character bitmap array
    &FontNames[_Calibri18pt][0] // (Pointer) Font filename saved on SD card or 0 (null) otherwise
};
