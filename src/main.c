/**
 ******************************************************************************
 * @file    main.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    29-July-2013
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bitmap.h"
#include "waveplayer.h"
#include "rct3.h"
#include "ir_decode.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
uint8_t BlinkSpeed = 0;

#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
#include "clock.h"

#include "TFT_TEXT/mk_text.h"
#include "TFT_FONT/mk_fonts.h"
//#include "TFT_FONT/LEDBOARD36pt.h"

#include "math.h"
#include "spi_conf.h"
#include "usart.h"

#define SYSCLK_FREQ  HSI_VALUE
#define SYSTICK_FREQ 100 // 100 Hz -> 10 ms
GPIO_InitTypeDef GPIO_InitStructure;

//void RCC_Configuration(void);

//void Delayms(__IO uint32_t nCount);

extern day;
extern volatile u8 mode, remoteClickedMode, remoteClickedUp, remoteClickedDown;

#define BUFFER_SIZE 2048

unsigned char Num[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
volatile bool updated = true;
volatile bool alarmRunning = false;
volatile bool updateTemp = false;
volatile bool updateDate = false;
volatile bool updateAlarm = false;
volatile bool rxMsgReady = false;
volatile char rxBuffer[BUFFER_SIZE];
volatile uint16_t rxPointer = 0;

/* Private function prototypes -----------------------------------------------*/

#define TABLE_SIZE 512

#define TWO_PI (3.14159 * 2)

float samples[TABLE_SIZE];

float phaseIncrement = TWO_PI / TABLE_SIZE;

float currentPhase = 0.0;
char uartResp[BUFFER_SIZE];

void GPIO_Config(void) {

	GPIO_InitTypeDef GPIO_InitStructure;

	/*************************************/
	/*!< SD_SPI_CS_GPIO, SD_SPI_MOSI_GPIO, SD_SPI_MISO_GPIO, SD_SPI_DETECT_GPIO
	 and SD_SPI_SCK_GPIO Periph clock enable */
	RCC_AHBPeriphClockCmd(
	SD_CS_GPIO_CLK | SD_SPI_MOSI_GPIO_CLK | SD_SPI_MISO_GPIO_CLK |
	SD_SPI_SCK_GPIO_CLK | SD_DETECT_GPIO_CLK, ENABLE);

	/*!< SD_SPI Periph clock enable */
	RCC_APB2PeriphClockCmd(SD_SPI_CLK, ENABLE);

	/*!< Configure SD_SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin = SD_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure SD_SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
	GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure SD_SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin = SD_SPI_MOSI_PIN;
	GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
	GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
	GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);

	/* Connect PXx to SD_SPI_SCK */
	GPIO_PinAFConfig(SD_SPI_SCK_GPIO_PORT, SD_SPI_SCK_SOURCE, SD_SPI_SCK_AF);

	/* Connect PXx to SD_SPI_MISO */
	GPIO_PinAFConfig(SD_SPI_MISO_GPIO_PORT, SD_SPI_MISO_SOURCE, SD_SPI_MISO_AF);

	/* Connect PXx to SD_SPI_MOSI */
	GPIO_PinAFConfig(SD_SPI_MOSI_GPIO_PORT, SD_SPI_MOSI_SOURCE, SD_SPI_MOSI_AF);
	/**************************************/

	/* MCO config */
	/*
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	 GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 GPIO_Init(GPIOA, &GPIO_InitStructure);

	 // pick one of the clocks to spew
	 RCC_MCOConfig(RCC_MCOSource_SYSCLK,RCC_MCODiv_8); // Put on MCO pin the: System clock selected
	 //RCC_MCOConfig(RCC_MCOSource_PLLCLK,RCC_MCODiv_1);
	 */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}

void SPI_Config(void) {
	//konfigurowanie interfejsu SPI
	SPI_InitTypeDef SPI_InitStructure;

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //transmisja z wykorzystaniem jednej linii, transmisja jednokierunkowa
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;               //tryb pracy SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;       //8-bit ramka danych
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; //stan sygnalu taktujacego przy braku transmisji - wysoki
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; //aktywne zbocze sygnalu taktujacego - 2-gie zbocze
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; //programowa obsluga linii NSS (CS)
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //prescaler szybkosci tansmisji  32MHz/2=16MHz
	//SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//prescaler szybkosci tansmisji  32MHz/4=8 MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //pierwszy bit w danych najbardziej znaczacy
	SPI_InitStructure.SPI_CRCPolynomial = 7; //stopien wielomianu do obliczania sumy CRC
	SPI_Init(SPI1, &SPI_InitStructure);                      //inicjalizacja SPI

	SPI_Cmd(SPI1, ENABLE);  	// Wlacz SPI1
}

int main(void) {

	FRESULT fresult;
	float temp;
	char tempStr[10];

	NVIC_InitTypeDef nvicStructure;

	SystemInit();

	GPIO_Config();

	//For sd card
	SPI_Config();



	/* Configure LED3 and LED4 on STM32L100C-Discovery */
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);

	delay_init(32); //32 MHz

	GPIO_SetBits(GPIOC, GPIO_Pin_2);
	delay_ms(1000);
	GPIO_ResetBits(GPIOC, GPIO_Pin_2);

	Lcd_Init2();

	Lcd_Clear2(BLACK);

	fresult = f_mount(&g_sFatFs, "0:0", 1);

			//LCD_BMP("kasia.bmp");


//	tft_puts(45, 20, "12:00", white, black);
	setCurrentFont(&LetsgoDigital60ptFontInfo);
	tft_puts(139, 0, ":", white, black);

	setCurrentFont(&Verdana26ptFontInfo);
	tft_puts(196, 205, ":", red, black);

	LCD_BMP(100,205,"bell4.bmp");

	init_usart_clocks();
	init_usart_gpio();
	init_usart();

	/****************************/
	setCurrentTime();
	getCurrentTemp(tempStr);
	/***************************/

	/* Configure RTC alarm A register to generate 8 interrupts per 1 Second */
	RTC_AlarmConfig();

	RCC_ClocksTypeDef RCC_Clocks;

	/* Initialize User_Button on STM32L100C-Discovery */
	STM_EVAL_PBInit(BUTTON_MODE, BUTTON_MODE_GPIO);
	STM_EVAL_PBInit(BUTTON_UP, BUTTON_MODE_GPIO);
	STM_EVAL_PBInit(BUTTON_DOWN, BUTTON_MODE_GPIO);
	//STM_EVAL_PBInit(BUTTON_MODE, BUTTON_MODE_GPIO);

	/* SysTick end of count event each 1ms */
	//   RCC_GetClocksFreq(&RCC_Clocks);
	//   SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
	/*************************/

	//TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);

	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 3200; //32 MHz/3200 = 10000
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//timerInitStructure.TIM_Period = 3332;
	timerInitStructure.TIM_Period = 100; //10 000/100 = 100 Hz - sampling frequency of buttons
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	//timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &timerInitStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);

	/*************************/

	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	NEC_Init();

	displayDate();
	LCD_Write_Curr_Temp(55, 170, tempStr);

	displayTime();


	//Gui_DrawFont_GBK24_bk(20,120, BLUE, WHITE, "1234 abcd");

	/*
	if (fresult == FR_OK) {
		playWav("m8m.wav"); //22kHz mono
		//playWav("rct8t.wav"); // fsamp 11025 Hz, 8bit mono
		//playWav("sine16.wav");
	}
	*/
	mode = 0;

	displayAlarm();

	mode = 0;

	while (1) {
		if (updated) {
			displayTime();
			updated = false;

			if(alarmRunning){
				alarmRunning = false;
				playWav("m8m.wav");
			}

			if (updateDate) {
				updateAndDisplayDate();
				updateDate = false;
			}

			if (updateTemp) {
				getCurrentTemp(tempStr);
				LCD_Write_Curr_Temp(55, 170, tempStr);
				updateTemp = false;
			}



		}

		if(updateAlarm){
			displayAlarm();
			updateAlarm = false;
		}

	}

}

void init_usart_clocks() {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
}

void init_usart_gpio() {
	// TX pin
	GPIO_InitTypeDef txOutput;
	GPIO_StructInit(&txOutput);
	txOutput.GPIO_Pin = GPIO_Pin_10;
	txOutput.GPIO_PuPd = GPIO_PuPd_UP;
	txOutput.GPIO_Mode = GPIO_Mode_AF;
	txOutput.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOC, &txOutput);

	// RX pin
	GPIO_InitTypeDef rxInput;
	GPIO_StructInit(&rxInput);
	rxInput.GPIO_Pin = GPIO_Pin_11;
	rxInput.GPIO_PuPd = GPIO_PuPd_NOPULL;
	rxInput.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOC, &rxInput);

	// Configure alternative functions for pins
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);
}

void init_usart() {
	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = 115200;
	uart.USART_WordLength = USART_WordLength_8b;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3, &uart);
	/* Enable RXNE interrupt */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	/* Enable USART1 global interrupt */
	NVIC_EnableIRQ(USART3_IRQn);
	USART_Cmd(USART3, ENABLE);
}

/**
 * @brief  Inserts a delay time.
 * @param  nTime: specifies the delay time length, in 1 ms.
 * @retval None
 */
void Delay(__IO uint32_t nTime) {
	TimingDelay = nTime;

	while (TimingDelay != 0)
		;
}

/**
 * @brief  Decrements the TimingDelay variable.
 * @param  None
 * @retval None
 */
void TimingDelay_Decrement(void) {
	if (TimingDelay != 0x00) {
		TimingDelay--;
	}
}

#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{}
}
#endif

/**
 * @}
 */

static void RTC_Config32768Internal(char *buf) {
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	RTC_DateTypeDef RTC_DateStruct;

//uint8_t yr;

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* SYSCFG Peripheral clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1);
	PWR_DeInit();

	/* Allow access to RTC */
	PWR_RTCAccessCmd(ENABLE);

	/* LSI used as RTC source clock */
	/* The RTC Clock may varies due to LSI frequency dispersion. */
	/* Enable the LSI OSC */
	//RCC_LSICmd(ENABLE);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	RCC_LSEConfig(RCC_LSE_ON);

	/* Wait till LSI is ready */
	//while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {
	}

	/* Select the RTC Clock Source */
	//RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	RTC_DeInit();

	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();

	// LSE 32768
	RTC_InitStructure.RTC_AsynchPrediv = 127;
	RTC_InitStructure.RTC_SynchPrediv = 255;

	/* Calendar Configuration with LSI supposed at 37KHz */
	//RTC_InitStructure.RTC_AsynchPrediv = 124;
	//RTC_InitStructure.RTC_SynchPrediv  = 295; /* (32KHz / 128) - 1 = 0xFF*/
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

	/* Set the time to 05h 20mn 00s AM */
	RTC_TimeStructure.RTC_H12 = RTC_H12_PM;

	parseNTPTime(buf, &RTC_DateStruct, &RTC_TimeStructure);

	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);

	//RTC_GetDate(RTC_Format_BCD, &RTC_DateStruct);

	//yr = RTC_DateStruct.RTC_Year;

	/* EXTI configuration *******************************************************/
	EXTI_ClearITPendingBit(EXTI_Line20);
	EXTI_InitStructure.EXTI_Line = EXTI_Line20;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

// Configuring RTC_WakeUp interrupt
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RTC_WakeUpCmd(DISABLE);
// RTC Wakeup Configuration
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);

// RTC Set WakeUp Counter
	RTC_SetWakeUpCounter(0);

// Enabling RTC_WakeUp interrupt
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

// Disabling Alarm Flags
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	RTC_AlarmCmd(RTC_Alarm_B, DISABLE);

// RTC Enable the Wakeup Function
	RTC_WakeUpCmd(ENABLE);

}

/**
 * @brief  Configures the RTC Alarm.
 * @param  None
 * @retval None
 */
static void RTC_AlarmConfig(void) {
	EXTI_InitTypeDef EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	//RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 ;

	/* EXTI configuration */
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable the RTC Alarm Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Set the alarm A Masks */
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_None;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel =
	RTC_AlarmDateWeekDaySel_WeekDay;
	//RTC_AlarmStructure.RTC_AlarmDateWeekDay = RTC_Weekday_Monday | RTC_Weekday_Tuesday | RTC_Weekday_Wednesday | RTC_Weekday_Thursday | RTC_Weekday_Friday;
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = (RTC_Weekday_Tuesday
			| RTC_Weekday_Sunday);

	//RTC_AlarmStructure.RTC_AlarmDateWeekDay = RTC_Weekday_Wednesday;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = 0x06;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x50;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x00;
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

	/* Set alarm A sub seconds and enable SubSec Alarm : generate 8 interrupts per Second */
	//RTC_AlarmSubSecondConfig(RTC_Alarm_A, 0xFF, RTC_AlarmSubSecondMask_SS14_5);
	/* Enable the RTC Alarm A Interrupt */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	/* Enable the alarm  A */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
}

void NEC_ReceiveInterrupt(NEC_FRAME f) {
	//GPIO_ToggleBits(GPIOD, GPIO_Pin_7);

	if (remoteClickedMode || remoteClickedUp || remoteClickedDown) //to avoid multiple switches
		return;

	//setCurrentFont(&Verdana26ptFontInfo);
	//tft_putint(0, 0,f.Command , white, black);

	switch (f.Command) {

	case 71:
		remoteClickedUp = 1;
		GPIO_ToggleBits(GPIOC, GPIO_Pin_9);

		break;
	case 69:
		remoteClickedDown = 1;
		GPIO_ToggleBits(GPIOC, GPIO_Pin_9);
		break;

	case 70:
		GPIO_ToggleBits(GPIOC, GPIO_Pin_9);
		remoteClickedMode = 1;

		break;

	default:
		break;

	}

}

void EXTI1_IRQHandler() {
	if (EXTI_GetITStatus(IR_EXTI_LINE) != RESET) {
		NEC_HandleEXTI();
	}
	EXTI_ClearITPendingBit(IR_EXTI_LINE);
}

void TIM3_IRQHandler() {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		NEC_TimerRanOut();
		//GPIO_ToggleBits(GPIOD, GPIO_Pin_7);

	}
}

/**********************************************************
 * USART1 interrupt request handler: on reception of a
 * character 't', toggle LED and transmit a character 'T'
 *********************************************************/
void USART3_IRQHandler(void) {

	/* RXNE handler */
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {

		rxBuffer[rxPointer] = (char) USART_ReceiveData(USART3);

		if ((rxPointer >= 3 && rxBuffer[rxPointer - 3] == 'O'
				&& (rxBuffer[rxPointer - 2] == 'K'
						|| rxBuffer[rxPointer - 2] == 'R')
				&& rxBuffer[rxPointer - 1] == '\r'
				&& rxBuffer[rxPointer] == '\n')) {
			rxBuffer[++rxPointer] = 0;
			rxMsgReady = true;
			rxPointer = 0;
		}

		rxPointer++;
		if (rxPointer == BUFFER_SIZE - 1) {	//prevent buffer overflow
			rxPointer = 0;
			//rxMsgReady = false;
		}

	}

}

void setCurrentTime() {
	print("AT+RST\r\n");

	while (rxMsgReady == false)
		;
	rxMsgReady = false;
	strcpy(uartResp, rxBuffer);

	rxBufferFlush();

	for (int i = 0; i < 10; i++) {
		delay_ms(1000);
		GPIO_ToggleBits(GPIOC, GPIO_Pin_8);
	}

	rxBufferFlush();
	print(
			"AT+CIPSNTPCFG=1,2,\"0.pl.pool.ntp.org\",\"ntp.itl.waw.pl\",\"vega.cbk.poznan.pl\"\r\n");

	while (rxMsgReady == false)
		;
	rxMsgReady = false;
	strcpy(uartResp, rxBuffer);

	for (int i = 0; i < 5; i++) {
		delay_ms(1000);
		GPIO_ToggleBits(GPIOC, GPIO_Pin_8);
	}

	rxBufferFlush();

	print("AT+CIPSNTPTIME?\r\n");

	while (rxMsgReady == false)
		;
	rxMsgReady = false;
	strcpy(uartResp, rxBuffer);

	RTC_Config32768Internal(uartResp);
}

void getCurrentTemp(char *tempStr) {
	float temp;
	const char httpReq[] =
			"GET /data/2.5/weather?q=Warsaw,pl&APPID=03af47dbe80f5630cbe86c62cff0d537 HTTP/1.1\r\nHost: api.openweathermap.org\r\n\r\n";

	rxBufferFlush();

	print("AT+CIPMODE=0\r\n");
	//print("AT+CWMODE_CUR?\r\n");

	while (rxMsgReady == false)
		;
	rxMsgReady = false;
	strcpy(uartResp, rxBuffer);

	rxBufferFlush();
	print("AT+CIPMUX=1\r\n");
	delay_ms(1000);

	print("AT+CIPSTART=4,\"TCP\",\"api.openweathermap.org\",80\r\n");
	delay_ms(1000);
	strcpy(uartResp, rxBuffer);

	rxBufferFlush();

	print("AT+CIPSEND=4,115\r\n");
	delay_ms(1000);
	strcpy(uartResp, rxBuffer);

	rxBufferFlush();

	print(httpReq);

	delay_ms(300);

	strcpy(uartResp, rxBuffer);
	char *tempStrPtr = strstr(uartResp, "\"temp\":");
	if (tempStrPtr) {
		strncpy(tempStr, tempStrPtr + 7, 6);
		tempStr[5] = 0;
		//temp = atof(tempStr)-273;
		sscanf(tempStr, "%f", &temp);
		temp = temp - 273;
		sprintf(tempStr, "%.1f~C", temp);
	}
}

void rxBufferFlush() {
	memset(rxBuffer, 0, BUFFER_SIZE - 1);
	rxPointer = 0;
	rxMsgReady = false;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
