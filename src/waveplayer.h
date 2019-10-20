/*
 * waveplayer.h
 *
 *  Created on: 18 lip 2014
 *      Author: Pawe³
 */

#ifndef WAVEPLAYER_H_
#define WAVEPLAYER_H_

#include "main.h"

//#define SAMPLING_FREQ 11025
#define SAMPLING_FREQ 22050
//#define SAMPLING_FREQ 44100
//#define DMA_TIMER_VAL 1451 //for 22050
//#define DMA_TIMER_VAL (2000.0/SAMPLING_FREQ)/31.25E-06
//#define DMA_TIMER_VAL 2902 //for 11025 Hz
#define DMA_TIMER_VAL (1000.0/SAMPLING_FREQ)/31.25E-06

//#define SAMPLE_WIDTH_16 1

void DAC_PreConfig(void);

void convertBufferTo12bit(int16_t * buffer);

void playWav(uint8_t * name);
void playWavFromMemory(uint8_t * memBuffer);
void playWavFromIntMemory(uint8_t * memBuffer);

#endif /* WAVEPLAYER_H_ */
