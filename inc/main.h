/**
  ******************************************************************************
  * @file    main.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    29-July-2013
  * @brief   Header for main.c module
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#define USE_PETIT_FAT 0

/* Private define ------------------------------------------------------------*/
#define DAC_DHR12R2_Address      0x40007414

/* 8-bit */
#define DAC_DHR8R1_Address       0x40007410

/* channel1 12-bit left aligned data holding register */
#define DAC_DHR12L1_Address       0x4000740C

/* DAC channel1 12-bit right-aligned data holding register */
#define DAC_DHR12R1_Address       0x40007408

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "stm32l100c_discovery.h"
#include "../FatFs_0.10/src/ff.h"
#include "stdbool.h"
#include <stdlib.h>

//#include "rct3.h"

//#define SAMPLE_WIDTH_16 1
//#define WAV_FROM_INT_MEMORY 0
#define SAMPLE_BUFFER_SIZE 512

/* Exported types ------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
static FATFS g_sFatFs; //obiekt FATFs
