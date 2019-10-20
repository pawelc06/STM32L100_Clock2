
#include <string.h>
#include <stdlib.h>
#include "../GUI.h"


#include  "mk_text.h"
#include "../TFT_FONT/mk_fonts.h"

#if USE_PETIT_FAT
#include "../../PetitFS/diskio.h"
#include "../../PetitFS/pff.h"
#endif

#include "../LCD_Driver.h"

FONT_INFO currentFont;

int CX, CY;

int frame_ptr;

void setCurrentFont( FONT_INFO * font ) {
	currentFont.heightPixels 		= font->heightPixels;
	currentFont.startChar 			= font->startChar;
	currentFont.interspacePixels	= font->interspacePixels;
	currentFont.spacePixels 		= font->spacePixels;
	currentFont.charInfo 			= (FONT_CHAR_INFO*) font->charInfo;
	currentFont.data 				= (uint8_t*)(font->data);
	currentFont.filename 			= (char*)(font->filename);
}

static void send_glyph_byte(uint8_t bajt, uint8_t width) {
	uint8_t i;
	for(i=0; i<width; i++ ) {


		if(bajt&0x80) Draw_pixel();
		else Draw_bk_pixel();





		bajt<<=1;
	}
}

static void send_glyph_byte_on_bakcground(uint16_t x,uint16_t y,uint8_t bajt, uint8_t width) {
	uint8_t i;
	for(i=0; i<width; i++ ) {


		//if(bajt&0x80) Draw_pixel();

		if(bajt&0x80)
			Gui_DrawPoint(x+i,y,Color);
		//else Draw_bk_pixel();





		bajt<<=1;
	}
}

static void send_font_bitmap(int xPixel, int yPixel, uint8_t *glyph, int glyphHeight, int glyphWidth ) {

	uint8_t i, j, k, bajt;
	uint16_t p=0;

	//Set_active_window(xPixel, yPixel, xPixel+glyphWidth-1, yPixel+glyphHeight-1);
	//Lcd_SetRegion(xPixel, yPixel, xPixel+glyphWidth-1, yPixel+glyphHeight-1);
	setAddrWindow(xPixel, yPixel, xPixel+glyphWidth-1, yPixel+glyphHeight-1);

	LCD_CS_CLR;
	LCD_RS_SET;

	for(i=0; i<glyphHeight; i++ ) {
		for(j=0, k=0; j<glyphWidth; j+=8, k++) {
			bajt = glyph[ p++ ] ;
			if( ((k+1)*8)<=glyphWidth ) send_glyph_byte(bajt, 8);
			else send_glyph_byte(bajt, glyphWidth-(k*8));
		}
	}

	LCD_CS_SET;
}


static void send_font_bitmap_on_background(int xPixel, int yPixel, uint8_t *glyph, int glyphHeight, int glyphWidth ) {

	uint8_t i, j, k, bajt;
	uint16_t p=0;


	//Set_active_window(xPixel, yPixel, xPixel+glyphWidth-1, yPixel+glyphHeight-1);
	//Lcd_SetRegion(xPixel, yPixel, xPixel+glyphWidth-1, yPixel+glyphHeight-1);
	//setAddrWindow(xPixel, yPixel, xPixel+glyphWidth-1, yPixel+glyphHeight-1);

	//LCD_CS_CLR;
	//LCD_RS_SET;

	for(i=0; i<glyphHeight; i++ ) {
		for(j=0, k=0; j<glyphWidth; j+=8, k++) {
			bajt = glyph[ p++ ] ;
			if( ((k+1)*8)<=glyphWidth ) send_glyph_byte_on_bakcground(xPixel+j,yPixel+i,bajt, 8);
			else send_glyph_byte_on_bakcground(xPixel+j,yPixel+i,bajt, glyphWidth-(k*8));
		}
	}

	//LCD_CS_SET;
}


#if USE_PETIT_FAT == 1
static void send_font_bin(int xPixel, int yPixel, int glyphHeight, int glyphWidth ) {

	uint8_t i, j, k, bajt;
	uint16_t p=0;

	//Set_active_window(xPixel, yPixel, xPixel+glyphWidth-1, yPixel+glyphHeight-1);
	//Lcd_SetRegion(xPixel, yPixel, xPixel+glyphWidth-1, yPixel+glyphHeight-1);

	setAddrWindow(xPixel, yPixel, xPixel+glyphWidth-1, yPixel+glyphHeight-1);



	for(i=0; i<glyphHeight; i++ ) {
		for(j=0, k=0; j<glyphWidth; j+=8, k++) {
			bajt = sd_buf[ p++ ];
			if( ((k+1)*8)<=glyphWidth ) send_glyph_byte(bajt, 8);
			else send_glyph_byte(bajt, glyphWidth-(k*8));
		}
	}


}
#endif



void tft_mputs( int x, int y, char * s, uint32_t color, uint32_t bk_color ) {

	uint8_t gH, gW, gS, gIS;
	uint16_t offset;
	uint8_t startChar = currentFont.startChar;
	uint8_t * glyph = (uint8_t*)currentFont.data;

	Set_color32(color);
	Set_bk_color32(bk_color);
	gH = currentFont.heightPixels;
	gIS = currentFont.interspacePixels;
	gS = currentFont.spacePixels;

	while( *s ) {
		if( *s > ' ') {
			gW = currentFont.charInfo[ *s - startChar  ].widthBits ;

			offset = currentFont.charInfo[ *s - startChar  ].offset;

			send_font_bitmap(x, y, glyph+offset, gH, gW );

			/* fill interspace between letters */
			setAddrWindow(x + gW,y,x + gW + gIS,y+gH-1);
			for(offset=0;offset<gIS*gH;offset++) {
							Draw_bk_pixel();

						}

			x = x + gW + gIS;

		} else {
			//Set_active_window(x,y,x+gS-1,y+gH-1);
			//Lcd_SetRegion(x,y,x+gS-1,y+gH-1);
			setAddrWindow(x,y,x+gS-1,y+gH-1);

			//LCD_CS_CLR;
			//LCD_RS_SET;

			for(offset=0;offset<gS*gH;offset++) {
				Draw_bk_pixel();

			}
			x+=gS;

			//LCD_CS_SET;

		}
		s++;
	}

	CX=x;
	CY=y;
}

void tft_mputs_on_background( int x, int y, char * s, uint32_t color ) {

	uint8_t gH, gW, gS, gIS;
	uint16_t offset;
	uint8_t startChar = currentFont.startChar;
	uint8_t * glyph = (uint8_t*)currentFont.data;

	Set_color32(color);
	//Set_bk_color32(bk_color);
	gH = currentFont.heightPixels;
	gIS = currentFont.interspacePixels;
	gS = currentFont.spacePixels;

	while( *s ) {
		if( *s > ' ') {
			gW = currentFont.charInfo[ *s - startChar  ].widthBits ;

			offset = currentFont.charInfo[ *s - startChar  ].offset;

			send_font_bitmap_on_background(x, y, glyph+offset, gH, gW );
			x = x + gW + gIS;

		} else {

			x+=gS;

			//LCD_CS_SET;

		}
		s++;
	}

	CX=x;
	CY=y;
}

void tft_puts( int x, int y, char * s, uint32_t color, uint32_t bk_color ) {

	//y+=frame_ptr;

	if( !currentFont.filename ) {
		tft_mputs(x,y, s, color, bk_color);
		return;
	}
#if USE_PETIT_FAT == 0
	else return;
#endif


#if USE_PETIT_FAT == 1
	uint8_t gH, gW=0, gS, gIS;
	uint16_t offset;
	uint8_t startChar = currentFont.startChar;
	WORD s1;
	uint16_t pof;

	char fname1[13];
	memset(fname1, 0, 13);
	memcpy_P(fname1, currentFont.filename, strlen( currentFont.filename));
	if( pf_open(fname1) ) return;

	Set_color32(color);
	Set_bk_color32(bk_color);
	gH = currentFont.heightPixels;
	gIS = currentFont.interspacePixels;
	gS = currentFont.spacePixels;

	pf_lseek(0);
	pf_read(sd_buf, 2, &s1);
	pof = (uint16_t)(sd_buf[1]<<8) | (uint16_t)(sd_buf[0]);



	while( *s ) {
		if( *s > ' ') {

			pf_lseek( (uint16_t)((*s-startChar)*3)+2 );

			pf_read(sd_buf, 3, &s1);

			gW = sd_buf[0];
			if( !gW ) continue;

			offset = (uint16_t)(sd_buf[2]<<8) | ( (uint16_t)(sd_buf[1] ));
			offset+=pof;

			pf_lseek(offset);
			pf_read(sd_buf, SD_BUF_SIZE, &s1);
			send_font_bin(x, y, gH, gW );

			x = x + gW + gIS;
		} else {
			Set_active_window(x,y,x+gS-1,y+gH-1);
			Write_command(0x2c);
			for(offset=0;offset<gS*gH;offset++) {
				Draw_bk_pixel();
				Draw_bk_pixel();
				Draw_bk_pixel();
			}
			x+=gS;
		}
		s++;

	}

	CX=x;
	CY=y;
#endif
}



void tft_puts_on_background( int x, int y, char * s, uint32_t color) {

	//y+=frame_ptr;

	if( !currentFont.filename ) {
		tft_mputs_on_background(x,y, s, color);
		return;
	}
#if USE_PETIT_FAT == 0
	else return;
#endif


#if USE_PETIT_FAT == 1
	uint8_t gH, gW=0, gS, gIS;
	uint16_t offset;
	uint8_t startChar = currentFont.startChar;
	WORD s1;
	uint16_t pof;

	char fname1[13];
	memset(fname1, 0, 13);
	memcpy_P(fname1, currentFont.filename, strlen( currentFont.filename));
	if( pf_open(fname1) ) return;

	Set_color32(color);
	Set_bk_color32(bk_color);
	gH = currentFont.heightPixels;
	gIS = currentFont.interspacePixels;
	gS = currentFont.spacePixels;

	pf_lseek(0);
	pf_read(sd_buf, 2, &s1);
	pof = (uint16_t)(sd_buf[1]<<8) | (uint16_t)(sd_buf[0]);



	while( *s ) {
		if( *s > ' ') {

			pf_lseek( (uint16_t)((*s-startChar)*3)+2 );

			pf_read(sd_buf, 3, &s1);

			gW = sd_buf[0];
			if( !gW ) continue;

			offset = (uint16_t)(sd_buf[2]<<8) | ( (uint16_t)(sd_buf[1] ));
			offset+=pof;

			pf_lseek(offset);
			pf_read(sd_buf, SD_BUF_SIZE, &s1);
			send_font_bin(x, y, gH, gW );

			x = x + gW + gIS;
		} else {
			Set_active_window(x,y,x+gS-1,y+gH-1);
			Write_command(0x2c);
			for(offset=0;offset<gS*gH;offset++) {
				Draw_bk_pixel();
				Draw_bk_pixel();
				Draw_bk_pixel();
			}
			x+=gS;
		}
		s++;

	}

	CX=x;
	CY=y;
#endif
}






int text_len(char *s) {

	int x=0;
	uint8_t gH, gW, gS, gIS;
	uint8_t startChar = currentFont.startChar;

	gH = currentFont.heightPixels;
	gIS = currentFont.interspacePixels;
	gS = currentFont.spacePixels;

	while( *s ) {
		if( *s > ' ') {
			gW = currentFont.charInfo[ *s - startChar  ].widthBits ;
			x = x + gW + gIS;
		} else x+=gS;
		s++;
	}

	return x;
}

int text_len_P(char *s) {

	char c;
	int x=0;
	uint8_t gH, gW, gS, gIS;
	uint8_t startChar = currentFont.startChar;

	gH = currentFont.heightPixels;
	gIS = currentFont.interspacePixels;
	gS = currentFont.spacePixels;

	while( (c=s) ) {
		if( c > ' ') {
			gW = currentFont.charInfo[ c - startChar  ].widthBits ;
			x = x + gW + gIS;
		} else x+=gS;
		s++;
	}

	return x;
}





void tft_putint(int x, int y, int val, uint32_t pen, uint32_t brush) {
	char bufor[17];

	itoa_mod(val, bufor);
	tft_puts(x, y, bufor, pen, brush );
}

/*
void tft_puthex(int x, int y, uint32_t val, uint32_t pen, uint32_t brush) {
	char bufor[17];

	memset(bufor, 0, 17);
	ltoa(val, bufor, 16);
	tft_puts(x, y, bufor, pen, brush );
}

*/
