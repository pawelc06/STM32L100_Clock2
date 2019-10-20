/*
 * bitmap.h
 *
 *  Created on: 9 lip 2014
 *      Author: Pawe³
 */

#ifndef BITMAP_H_
#define BITMAP_H_

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

uint32_t RGB_24to16(uint32_t color);
void LCD_BMP(u8 * nazwa_pliku);


#endif /* BITMAP_H_ */
