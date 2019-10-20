/*
*  Tahoma48pt.c
*  Created on: 2012-01-26 14:19:14
*       Autor: Miros³aw Kardaœ
*       www.atnel.pl
*/


#include "mk_fonts.h"

// Font information for Tahoma 48pt
const FONT_INFO Tahoma48ptFontInfo  =
{
    77,  // Character height
    '!',  //  Start character
    4,  //  number of pixels of interspace between characters
    16,  // number of pixels of space character
    0, // Character descriptor array
    0, // Character bitmap array
    &FontNames[_Tahoma48pt][0] // (Pointer) Font filename saved on SD card or 0 (null) otherwise
};
