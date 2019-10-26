/*
 * waveplayer.c
 *
 *  Created on: 18 lip 2014
 *      Author: Pawe³
 */

//#include "main.h"
#include "waveplayer.h"
#include "string.h"
#include "clock.h"

extern bool updated;
extern uint8_t * wavPtr;
extern uint8_t * wavPtrBegin;
extern uint8_t playTime;


#ifdef SAMPLE_WIDTH_16
int16_t buffer[2][512];
#else
uint8_t buffer[2][512];
#endif

extern FIL plik;
extern UINT bytesToRead, bytesRead;
bool canRead;
extern uint8_t i;

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
DAC_InitTypeDef DAC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;

/**
 * @brief  PrecConfiguration: configure PA4 and PA5 in analog,
 *                           enable DAC clock, enable DMA1 clock
 * @param  None
 * @retval None
 */
void DAC_PreConfig(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* DMA1 clock enable (to be used with DAC) */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* DAC Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	/* GPIOA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	/* Configure PA.04 (DAC_OUT1), PA.05 (DAC_OUT2) as analog */
	//GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void convertBufferTo12bit(int16_t * buffer) {
	uint16_t i;
	for (i = 0; i < SAMPLE_BUFFER_SIZE; i++) {
		buffer[i] = (buffer[i] + 32768) >> 4;
	}
}

void playWav(uint8_t * name) {

	FRESULT fresult;

	UINT bytesToRead, bytesRead;

	NVIC_InitTypeDef nvicStructure;

	/* Preconfiguration before using DAC----------------------------------------*/
	DAC_PreConfig();

	/* TIM6 Configuration ------------------------------------------------------*/
	/* TIM6 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	/* Time base configuration */


	/* TIM6 Configuration */
	TIM_TimeBaseInitTypeDef TIM6_TimeBase;
	TIM_TimeBaseStructInit(&TIM6_TimeBase);
	//TIM6_TimeBase.TIM_Period = (uint16_t)(DMA_TIMER_VAL - 1); // 0 .. N-1
	TIM6_TimeBase.TIM_Prescaler = 0;
	TIM6_TimeBase.TIM_ClockDivision = 0;
	TIM6_TimeBase.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM6_TimeBase);
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	//TIM_Cmd(TIM6, ENABLE);

	  TIM_SetAutoreload(TIM6, DMA_TIMER_VAL);

	  /* TIM6 TRGO selection */
	  //TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

	  /* Enable TIM6 update interrupt */
	  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	/* Enable the TIM6 Interrupt */
	/*
	 nvicStructure.NVIC_IRQChannel = TIM6_IRQn;
	 nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	 nvicStructure.NVIC_IRQChannelSubPriority = 0;
	 nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&nvicStructure);
	 */

	/* TIM6 enable counter */
	TIM_Cmd(TIM6, ENABLE);

	/****************************/
	/* Triangle Wave generator -----------------------------------------*/
	DAC_DeInit();

	/* Noise Wave generator --------------------------------------------*/
	/* DAC channel1 Configuration */

	//DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

	DMA_DeInit(DMA1_Channel2);
#ifdef SAMPLE_WIDTH_16
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_Address;
#else
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_Address;
#endif
	//DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_Address;
	//DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sine12bit;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &buffer[0][0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	//DMA_InitStructure.DMA_BufferSize = 32;
	DMA_InitStructure.DMA_BufferSize = SAMPLE_BUFFER_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
#ifdef SAMPLE_WIDTH_16
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
#else
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
#endif

	//  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel2, &DMA_InitStructure);

	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

	/* Enable the DMA1_Channel3 Interrupt */
	nvicStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 1;
	nvicStructure.NVIC_IRQChannelSubPriority = 3;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;

	/* Enable DMA for DAC Channel1 */
	DAC_DMACmd(DAC_Channel_1, ENABLE);

	//fresult = f_open(&plik,"z.wav", FA_READ);
	//fresult = f_open(&plik, "im16.wav", FA_READ);
	fresult = f_open(&plik, name, FA_READ);

	bytesToRead = f_size(&plik);

	//fresult = f_lseek(&plik, 44 + 22050);
	fresult = f_lseek(&plik, 0);
#ifdef SAMPLE_WIDTH_16
	fresult = f_read(&plik, &buffer[0][0], SAMPLE_BUFFER_SIZE * 2, &bytesRead);
	convertBufferTo12bit(&buffer[0][0]);

	fresult = f_read(&plik, &buffer[1][0], SAMPLE_BUFFER_SIZE * 2, &bytesRead);
	convertBufferTo12bit(&buffer[1][0]);
#else
	fresult = f_read(&plik, &buffer[0][0], SAMPLE_BUFFER_SIZE , &bytesRead);
	fresult = f_read(&plik, &buffer[1][0], SAMPLE_BUFFER_SIZE , &bytesRead);
#endif

	//fresult = f_read(&plik, &buffer[1][0], 512 , &bytesRead);

	NVIC_Init(&nvicStructure);

	DMA_Cmd(DMA1_Channel2, ENABLE);

	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	DAC_Cmd(DAC_Channel_1, ENABLE);

	canRead = false;
	i = 0;
	playTime=0;

	while (playTime<60) {
		if (canRead == true) {
#ifdef SAMPLE_WIDTH_16
			f_read(&plik, &buffer[i ^ 0x01][0], SAMPLE_BUFFER_SIZE * 2,
					&bytesRead);
			convertBufferTo12bit(&buffer[i ^ 0x01][0]);

			canRead = false;
			if (bytesRead < SAMPLE_BUFFER_SIZE * 2)
				break;
#else
			f_read(&plik, &buffer[i ^ 0x01][0], SAMPLE_BUFFER_SIZE , &bytesRead);
			canRead = false;
			if (bytesRead < SAMPLE_BUFFER_SIZE)
			break;
#endif

		}

	}

	DAC_Cmd(DAC_Channel_1, DISABLE);
	DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, DISABLE);
	fresult = f_close(&plik);

}

void playWavFromMemory(uint8_t * memBuffer) {

	FRESULT fresult;

	UINT bytesToRead, bytesRead;

	NVIC_InitTypeDef nvicStructure;
	uint8_t * mPtr = memBuffer;

	/* Preconfiguration before using DAC----------------------------------------*/
	DAC_PreConfig();

	/* TIM6 Configuration ------------------------------------------------------*/
	/* TIM6 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);


	/* TIM6 Configuration */
	  TIM_DeInit(TIM6);

	  TIM_SetAutoreload(TIM6, DMA_TIMER_VAL);

	  /* TIM6 TRGO selection */
	  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

	  /* Enable TIM6 update interrupt */
	  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);



	/* TIM6 enable counter */
	TIM_Cmd(TIM6, ENABLE);

	/****************************/
	/* Triangle Wave generator -----------------------------------------*/
	DAC_DeInit();

	/* Noise Wave generator --------------------------------------------*/
	/* DAC channel1 Configuration */

	//DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

	DMA_DeInit(DMA1_Channel2);
#ifdef SAMPLE_WIDTH_16
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_Address;
#else
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_Address;
#endif
	//DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_Address;
	//DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sine12bit;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &buffer[0][0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	//DMA_InitStructure.DMA_BufferSize = 32;
	DMA_InitStructure.DMA_BufferSize = SAMPLE_BUFFER_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
#ifdef SAMPLE_WIDTH_16
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
#else
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
#endif

	//  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel2, &DMA_InitStructure);

	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

	/* Enable the DMA1_Channel3 Interrupt */
	nvicStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 1;
	nvicStructure.NVIC_IRQChannelSubPriority = 3;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;

	/* Enable DMA for DAC Channel1 */
	DAC_DMACmd(DAC_Channel_1, ENABLE);


	//fresult = f_open(&plik, name, FA_READ);

	bytesToRead = f_size(&plik);

	//fresult = f_lseek(&plik, 44 + 22050);
	//fresult = f_lseek(&plik, 0);
#ifdef SAMPLE_WIDTH_16
	//fresult = f_read(&plik, &buffer[0][0], SAMPLE_BUFFER_SIZE * 2, &bytesRead);
	memcpy(&buffer[0][0],mPtr,SAMPLE_BUFFER_SIZE*2);
	convertBufferTo12bit(&buffer[0][0]);

	mPtr = mPtr + SAMPLE_BUFFER_SIZE*2;

	//fresult = f_read(&plik, &buffer[1][0], SAMPLE_BUFFER_SIZE * 2, &bytesRead);
	memcpy(&buffer[1][0],mPtr,SAMPLE_BUFFER_SIZE*2);
	convertBufferTo12bit(&buffer[1][0]);

	mPtr = mPtr + SAMPLE_BUFFER_SIZE*2;
#else
	//fresult = f_read(&plik, &buffer[0][0], SAMPLE_BUFFER_SIZE , &bytesRead);
	//fresult = f_read(&plik, &buffer[1][0], SAMPLE_BUFFER_SIZE , &bytesRead);

	memcpy(&buffer[0][0],mPtr,SAMPLE_BUFFER_SIZE);
	mPtr = mPtr + SAMPLE_BUFFER_SIZE;
	memcpy(&buffer[1][0],mPtr,SAMPLE_BUFFER_SIZE);
	mPtr = mPtr + SAMPLE_BUFFER_SIZE;

#endif

	//fresult = f_read(&plik, &buffer[1][0], 512 , &bytesRead);

	NVIC_Init(&nvicStructure);

	DMA_Cmd(DMA1_Channel2, ENABLE);

	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	DAC_Cmd(DAC_Channel_1, ENABLE);

	canRead = false;
	i = 0;

	while (1) {
		if (updated) {
					displayTime();
					updated = false;
				}

		if (canRead == true) {
#ifdef SAMPLE_WIDTH_16
			//f_read(&plik, &buffer[i ^ 0x01][0], SAMPLE_BUFFER_SIZE * 2,&bytesRead);
			memcpy(&buffer[i ^ 0x01][0],mPtr,SAMPLE_BUFFER_SIZE*2);
			convertBufferTo12bit(&buffer[i ^ 0x01][0]);

			canRead = false;
			if (bytesRead < SAMPLE_BUFFER_SIZE * 2)
				break;
#else
			//f_read(&plik, &buffer[i ^ 0x01][0], SAMPLE_BUFFER_SIZE , &bytesRead);
			memcpy(&buffer[i ^ 0x01][0],mPtr,SAMPLE_BUFFER_SIZE);
			mPtr = mPtr + SAMPLE_BUFFER_SIZE;
			canRead = false;
			if (mPtr >= memBuffer+67392)
			break;
#endif

		}

	}

	DAC_Cmd(DAC_Channel_1, DISABLE);
	DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, DISABLE);
	//fresult = f_close(&plik);

}

void playWavFromIntMemory(uint8_t * memBuffer) {



	UINT bytesToRead, bytesRead;

	NVIC_InitTypeDef nvicStructure;

	wavPtrBegin = memBuffer;
	wavPtr = wavPtrBegin;

	/* Preconfiguration before using DAC----------------------------------------*/
	DAC_PreConfig();

	/* TIM6 Configuration ------------------------------------------------------*/
	/* TIM6 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);


	/* TIM6 Configuration */
	  TIM_DeInit(TIM6);

	  TIM_SetAutoreload(TIM6, DMA_TIMER_VAL);

	  /* TIM6 TRGO selection */
	  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

	  /* Enable TIM6 update interrupt */
	  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);



	/* TIM6 enable counter */
	TIM_Cmd(TIM6, ENABLE);

	/****************************/

	DAC_DeInit();




	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

	DMA_DeInit(DMA1_Channel2);
#ifdef SAMPLE_WIDTH_16
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_Address;
#else
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_Address;
#endif

	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) wavPtr;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

	DMA_InitStructure.DMA_BufferSize = SAMPLE_BUFFER_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
#ifdef SAMPLE_WIDTH_16
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
#else
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
#endif


	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel2, &DMA_InitStructure);

	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

	/* Enable the DMA1_Channel2 Interrupt */
	nvicStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 8;
	nvicStructure.NVIC_IRQChannelSubPriority = 3;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;

	/* Enable DMA for DAC Channel1 */
	DAC_DMACmd(DAC_Channel_1, ENABLE);




	NVIC_Init(&nvicStructure);

	DMA_Cmd(DMA1_Channel2, ENABLE);

	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	DAC_Cmd(DAC_Channel_1, ENABLE);








}
