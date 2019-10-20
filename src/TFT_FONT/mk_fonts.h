/*
 * fontx.h
 *
 *  Created on: 2011-09-19
 *       Autor: Miros�aw Karda�
 */


#ifndef TFT_FONTX_H_
#define TFT_FONTX_H_

#include <stdint.h>


// ==========================================================================
// structure definition
// ==========================================================================

// This structure describes a single character's display information
typedef struct
{
	uint8_t widthBits;					// width, in bits (or pixels), of the character
	uint16_t offset;					// offset of the character's bitmap, in bytes, into the the FONT_INFO's data array

} FONT_CHAR_INFO;

// Describes a single font
typedef struct
{
	uint8_t 			heightPixels;		// height, in pages (8 pixels), of the font's characters
	uint8_t	 			startChar;			// the first character in the font (e.g. in charInfo and data)
	uint8_t				interspacePixels;	// number of pixels of interspace between characters
	uint8_t				spacePixels;		// number of pixels of space character
	FONT_CHAR_INFO*		charInfo;			// pointer to array of char information
	uint8_t*			data;				// pointer to generated array of character visual representation
	char*				filename;			// (Pointer) Font filename saved on SD card or 0 (null) otherwise

} FONT_INFO;

extern const char FontNames[][13];

enum {
	_Calibri18pt,
	_Tahoma48pt,
	_LEDBOARD36pt,
};

// Font data for Tahoma 48pt
extern const FONT_INFO Tahoma48ptFontInfo;




// Font data for Tahoma 8pt
extern const uint8_t Tahoma8ptBitmaps[];
extern const FONT_CHAR_INFO Tahoma8ptDescriptors[];
extern const FONT_INFO DefaultFontInfo;


// Font data for Courier New 10pt_bold
extern const uint8_t CourierNew10pt_boldBitmaps[];
extern const FONT_CHAR_INFO CourierNew10pt_boldDescriptors[];
extern const FONT_INFO CourierNew10pt_boldFontInfo;

// Font data for Let's go Digital 60pt
extern const uint8_t LetsgoDigital60ptBitmaps[];
extern const FONT_CHAR_INFO LetsgoDigital60ptDescriptors[];
extern const FONT_INFO LetsgoDigital60ptFontInfo;

// Font data for Let's go Digital 48pt
extern const uint8_t LetsgoDigital48ptBitmaps[];
extern const FONT_CHAR_INFO LetsgoDigital48ptDescriptors[];
extern const FONT_INFO LetsgoDigital48ptFontInfo;


// Font data for Calibri 18pt
extern const FONT_INFO Calibri18ptFontInfo;

extern const FONT_INFO LEDBOARD36ptFontInfo;

// Font data for Verdana 26pt
extern const uint8_t Verdana26ptBitmaps[];
extern const FONT_CHAR_INFO Verdana26ptDescriptors[];
extern const FONT_INFO Verdana26ptFontInfo;



#endif /* TFT_FONTX_H_ */
