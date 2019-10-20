/**
 ******************************************************************************
 * @file    stm32l1xx_it.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    29-July-2013
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
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
#include "stm32l1xx_it.h"
#include "stm32l1xx_rtc.h"
#include "main.h"
#include "stdbool.h"
#include "stm32l1xx_tim.h"
#include "clock.h"

volatile bool toggleFlag = false;
extern bool updated;
extern bool updateTemp;
extern uint8_t mode; //0 - normal, 1 - hours, 2 - minutes, 3 seconds
extern uint8_t remoteClickedMode;
extern uint8_t remoteClickedUp;
extern uint8_t remoteClickedDown;

extern uint16_t ssTogle;
volatile uint16_t sample;
volatile uint8_t i;
volatile uint8_t * wavPtr;
volatile uint8_t * wavPtrBegin;
FIL plik;
UINT bytesToRead, bytesRead;

extern bool canRead;

//uint8_t buffer[2][512];
#ifdef SAMPLE_WIDTH_16
extern int16_t buffer[2][512];
#else
extern uint8_t buffer[2][512];
#endif

/** @addtogroup STM32L100C-Discovery_Demo
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void) {
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void) {
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void) {
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void) {
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void) {
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void) {
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void) {
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void) {
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void) {
	disk_timerproc();

	//TimingDelay_Decrement();
	/*
	 if(toggleFlag){
	 STM_EVAL_LEDOn(LED4);
	 } else {
	 STM_EVAL_LEDOff(LED4);
	 }

	 toggleFlag = !toggleFlag;
	 */

}

void RTC_WKUP_IRQHandler(void) {
	if (RTC_GetITStatus(RTC_IT_WUT) != RESET) {
		RTC_ClearITPendingBit(RTC_IT_WUT);
		EXTI_ClearITPendingBit(EXTI_Line20); // And EXTI

		//GPIO_ToggleBits(LD4_GPIO_PORT, LD4_PIN);
		//STM_EVAL_LEDToggle(LED4);
		updated = true;
		//displayTime();
		ssTogle++;
		if (ssTogle % 60 == 0) {
			updateTemp = true;
		}
	}
}

/******************************************************************************/
/*                 STM32L1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l1xx_xx.s).                                            */
/******************************************************************************/

void DMA1_Channel2_IRQHandler(void) {
	//DMA_InitTypeDef            DMA_InitStructure;

	if (DMA_GetITStatus(DMA1_IT_TC2) != RESET) {

		DMA_ClearITPendingBit(DMA1_IT_TC2);

		if (toggleFlag) {
			STM_EVAL_LEDOn(LED3); //C9
		} else {
			STM_EVAL_LEDOff(LED3);
		}

		toggleFlag = !toggleFlag;

		//DMA1_Channel2->CMAR = (uint32_t)&buffer[0][(i++)%2];
		//DAC_DMACmd(DAC_Channel_1, DISABLE);
		//DAC_Cmd(DAC_Channel_1, DISABLE);

		/**********************************/
		DMA1_Channel2->CCR = 0x0;
		DMA1_Channel2->CNDTR = 0x200;

#ifdef SAMPLE_WIDTH_16
		/* 12 bit */
		DMA1_Channel2->CPAR = DAC_DHR12R1_Address;
#else

		/* 8- bit */
		DMA1_Channel2->CPAR = DAC_DHR8R1_Address;
#endif

		//i = (i + 1)%2;

#ifdef WAV_FROM_INT_MEMORY
		DMA1_Channel2->CMAR = (uint32_t) wavPtr;
		wavPtr += SAMPLE_BUFFER_SIZE;
		if (wavPtr >= wavPtrBegin + 67392) {
			DAC_Cmd(DAC_Channel_1, DISABLE);
			DMA_Cmd(DMA1_Channel2, DISABLE);
			DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, DISABLE);
		}
#else
		i ^= 0x01;
		DMA1_Channel2->CMAR = (uint32_t) &buffer[i][0];
#endif

		//i ^= 0x01;

#ifdef SAMPLE_WIDTH_16
		/* 12 bit */
		DMA1_Channel2->CCR = 0x2593;
#else

		/* 8-bit */
		DMA1_Channel2->CCR = 0x2093;
#endif
		/**********************************/

		DMA1->IFCR = DMA1_IT_TC2;

		canRead = true;

	}
}

/*
 void TIM6_IRQHandler(void){
 if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET){
 TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

 if (toggleFlag) {
 STM_EVAL_LEDOn(LED3);
 } else {
 STM_EVAL_LEDOff(LED3);
 }

 toggleFlag = !toggleFlag;

 DAC_SetChannel1Data(DAC_Align_12b_R, (sample++) % 16383);


 }
 }
 */

void TIM2_IRQHandler(void) {

	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	uint8_t sec, min, hours;
	uint32_t year, month, day, weekday;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		static uint8_t bstateu = 0; //ostatnie stany przycisku
		static uint8_t bstated = 0; //ostatnie stany przycisku
		static uint8_t bstatem = 0; //ostatnie stany przycisku

		if (remoteClickedMode) {
			mode = (mode + 1) % 7;
			remoteClickedMode = 0;
		}

		if (remoteClickedUp
				|| ((bstateu = (bstateu << 1 & 0xf)
						| (UP_BUTTON_GPIO_PORT->IDR >> BUTTON_UP & 1)) == 1)) {

			if (mode <= 2)
				RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);

			if (mode > 2)
				RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

			switch (mode) {
			case 1: //hours
				hours = RTC_TimeStructure.RTC_Hours;
				hours = bcd2dec(hours);
				hours = (hours + 1) % 24;
				hours = dec2bcd(hours);
				RTC_TimeStructure.RTC_Hours = hours;
				break;

			case 2: //minutes
				min = RTC_TimeStructure.RTC_Minutes;
				min = bcd2dec(min);
				min = (min + 1) % 60;
				min = dec2bcd(min);
				RTC_TimeStructure.RTC_Minutes = min;
				break;

			case 3: //year
				year = RTC_DateStructure.RTC_Year;
				year = (year + 1) % 100;
				RTC_DateStructure.RTC_Year = year;
				break;
			case 4: //month
				month = RTC_DateStructure.RTC_Month;
				month = (month + 1) % 13;
				if (!month)
					month = 1;
				RTC_DateStructure.RTC_Month = month;
				break;
			case 5: //day
				day = RTC_DateStructure.RTC_Date;
				day = (day + 1) % 32;
				if (!day)
					day = 1;
				RTC_DateStructure.RTC_Date = day;
				break;
			case 6: //day of the week
				weekday = RTC_DateStructure.RTC_WeekDay;
				weekday = (weekday + 1) % 8;
				if (!weekday)
					weekday = 1;
				RTC_DateStructure.RTC_WeekDay = weekday;
				break;

			default:
				break;
			}

			if (mode > 2)
				RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);

			if ((mode == 1) || (mode == 2)) {
				RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
				updated = true;
			}

			/*
			 if (toggleFlag) {
			 STM_EVAL_LEDOn(LED3);
			 } else {
			 STM_EVAL_LEDOff(LED3);
			 }

			 toggleFlag = !toggleFlag;
			 */
			remoteClickedUp = 0;

			return;
		}

		if (remoteClickedDown
				|| ((bstated = (bstated << 1 & 0xf)
						| (UP_BUTTON_GPIO_PORT->IDR >> BUTTON_DOWN & 1)) == 1)) {
			//(STM_EVAL_PBGetState(BUTTON_UP)))== 0){

			// byl zwolniony, teraz jest wcisniety - zmiana stanu LED
			//LED_PORT->ODR ^= 1 << GREEN_LED_BIT | 1 << BLUE_LED_BIT;

			if (mode <= 2)
				RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);

			if (mode > 2)
				RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

			switch (mode) {
			case 1:
				hours = RTC_TimeStructure.RTC_Hours;
				hours = bcd2dec(hours);
				if (hours > 0) {
					hours = hours - 1;
				} else {
					hours = 23;
				}
				hours = dec2bcd(hours);
				RTC_TimeStructure.RTC_Hours = hours;
				break;

			case 2:
				min = RTC_TimeStructure.RTC_Minutes;
				min = bcd2dec(min);
				if (min > 0) {
					min = min - 1;
				} else {
					min = 59;
				}
				min = dec2bcd(min);
				RTC_TimeStructure.RTC_Minutes = min;
				break;

			case 3: //year
				year = RTC_DateStructure.RTC_Year;
				year = year - 1;
				if (year < 0)
					year = 0;
				RTC_DateStructure.RTC_Year = year;
				break;
			case 4: //month
				month = RTC_DateStructure.RTC_Month;
				month = month - 1;
				if (month == 0)
					month = 12;
				RTC_DateStructure.RTC_Month = month;
				break;
			case 5: //day
				day = RTC_DateStructure.RTC_Date;

				if (day > 2)
					day = day - 1;

				RTC_DateStructure.RTC_Date = day;
				break;

			case 6: //day of the week
				weekday = RTC_DateStructure.RTC_WeekDay;
				weekday = weekday - 1;
				if (!day)
					day = 1;
				RTC_DateStructure.RTC_WeekDay = weekday;
				break;

			default:
				break;
			}

			if (mode > 2)
				RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);

			if ((mode == 1) || (mode == 2)) {
				RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
				updated = true;
			}
			remoteClickedDown = 0;

			return;
		}

		/*************************************/
		if (((bstatem = (bstatem << 1 & 0xf)
				| (MODE_BUTTON_GPIO_PORT->IDR >> BUTTON_MODE & 1)) == 1)) {
			//if(remoteClickedMode || ((bstatem = (bstatem << 1 & 0xf) || (MODE_BUTTON_GPIO_PORT->IDR >> BUTTON_MODE & 1)) == 1)) {

			mode = (mode + 1) % 7;

			if ((mode == 0) || (mode == 6))
				displayDate();

			if (toggleFlag) {
				STM_EVAL_LEDOn(LED4);
			} else {
				STM_EVAL_LEDOff(LED4);
			}

			toggleFlag = !toggleFlag;
			remoteClickedMode = 0;
			return;
		}
		/**************************************/

	}
}

/**
 * @brief  This function handles RTC Alarms interrupt request.
 * @param  None
 * @retval None
 */
void RTC_Alarm_IRQHandler(void) {
	if (RTC_GetITStatus(RTC_IT_ALRA) != RESET) {
		STM_EVAL_LEDToggle(LED3);
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		EXTI_ClearITPendingBit(EXTI_Line17);
	}
}

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
 {
 }*/

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
