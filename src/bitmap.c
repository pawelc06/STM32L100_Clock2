/*
 * bitmap.c
 *
 *  Created on: 9 lip 2014
 *      Author: Pawe³
 */

#include "../FatFs_0.10/src/ff.h"
#include "../FatFs_0.10/src/diskio.h"
#include <inttypes.h>
#include "bitmap.h"
#include "Lcd_Driver.h"
#include "Lcd_Config.h"




uint32_t RGB_24to16(uint32_t color)
{
	uint32_t rgb;



	rgb = ((color & 0xF80000)>>8) | ((color & 0xFC00)>>5) | ((color & 0xF8)>>3);

	return( rgb );
}

void LCD_BMP(uint16_t x,uint16_t y,u8 * nazwa_pliku) {
u32 i = 0, j = 0, liczba_pikseli = 0, liczba_bajtow =0,ii;

u32 width = 0;
u32 heigth = 0;

u16 layers;

u16 bitsPerPixel;

u16 piksel16;
u8 temp[4];
u8 buffer[3];
uint32_t var_temp=0x0000;

WORD ile_bajtow;
FRESULT fresult;
FIL plik;


//fresult = f_mount(0, &plik);
// Otwarcie do odczytu pliku bitmapy
fresult = f_open (&plik, (const char *) nazwa_pliku, FA_READ);
// Opuszczenie dwoch pierwszych bajtow
fresult = f_read (&plik, &temp[0], 2, &ile_bajtow);
// rozmiar pliku w bajtach
fresult = f_read (&plik, (u8*) &liczba_bajtow, 4, &ile_bajtow);
// Opuszczenie 4 bajtow
fresult = f_read (&plik, &temp[0], 4, &ile_bajtow);
// Odczytanie przesuniecia (offsetu) od poczatku pliku do
// poczatku bajtow opisujacych obraz
fresult = f_read (&plik, (u8*) &i, 4, &ile_bajtow);

//size of information header
fresult = f_read (&plik,  &i, 4, &ile_bajtow);


//width
fresult = f_read (&plik,  &width, 4, &ile_bajtow);

//heigth
fresult = f_read (&plik,  &heigth, 4, &ile_bajtow);

//number of color layers
fresult = f_read (&plik,  &layers, 2, &ile_bajtow);

//bits per pixel
fresult = f_read (&plik,  &bitsPerPixel, 2, &ile_bajtow);

// Opuszczenie liczby bajtow od aktualnego miejsca
// do poczatku danych obrazu, wartosc 14, bo odczytane zostalo
// juz z pliku 2+4+4+4+4+4+4=26 bajtow
//for(j = 0; j < (i - 26); j++){
for(j = 0; j < 24; j++){
	fresult = f_read (&plik, &temp[0], 1, &ile_bajtow);
}
// Liczba pikseli obrazu = (rozmiar pliku - offset)/3 bajty na pisel
liczba_pikseli = width*heigth;
// Ustawienie parametrow pracy LCD (m. in. format BGR 5-6-5)
//LCD_WriteReg(R3, 0x1008);
//Write_Command(R3, 0x1008);

//setAddrWindow(0, 0,  X_MAX_PIXEL, Y_MAX_PIXEL);
setAddrWindow(x, y,  x+width-1, y+heigth-1);

// Odczyt bajtow z karty SD i wyslanie danych do LCD

ii=0;
for(i = 0; i < liczba_pikseli; i++)
{
//fresult = f_read (&plik, (u8*) &piksel24, 3, &ile_bajtow);
	fresult = f_read (&plik, (u8*) buffer, 3, &ile_bajtow);

var_temp=((uint32_t)(buffer[2])<<16)+((uint32_t)(buffer[1])<<8)+buffer[0];

piksel16 =(u16) LCD_RGB_24to16(var_temp);


//lcdbuffer[ii++] = piksel16;
//lcdbuffer[ii++] = (uint8_t) (piksel16 >> 8);
//lcdbuffer[ii++] = (uint8_t) (piksel16 & 0x00FF);

//LCD_WriteRAM(piksel);
//Write_Data(piksel16);
Lcd_WriteData(piksel16>>8);
Lcd_WriteData(piksel16);


}
fresult = f_close (&plik);

}

