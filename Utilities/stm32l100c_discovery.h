/**
  ******************************************************************************
  * @file    stm32l100c_discovery.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    29-July-2013
  * @brief   This file contains definitions for STM32L100C-Discovery's Leds, push-
  *          buttons hardware resources.
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
#ifndef __STM32L100C_DISCOVERY_H
#define __STM32L100C_DISCOVERY_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32L100C_DISCOVERY
  * @{
  */
      
/** @addtogroup STM32L100C_DISCOVERY_LOW_LEVEL
  * @{
  */ 

/** @defgroup STM32L100C_DISCOVERY_LOW_LEVEL_Exported_Types
  * @{
  */
typedef enum 
{
  LED3 = 0,
  LED4 = 1
} Led_TypeDef;

typedef enum 
{
  USER_BUTTON = 0,
  BUTTON_UP = 1,
  BUTTON_DOWN = 2,
  BUTTON_MODE = 3
} Button_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;
  
/**
  * @}
  */ 

/** @defgroup STM32L100C_DISCOVERY_LOW_LEVEL_Exported_Constants
  * @{
  */ 

/** @addtogroup STM32L100C_DISCOVERY_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             2

#define LED3_PIN                         GPIO_Pin_9
#define LED3_GPIO_PORT                   GPIOC
#define LED3_GPIO_CLK                    RCC_AHBPeriph_GPIOC
  
#define LED4_PIN                         GPIO_Pin_8
#define LED4_GPIO_PORT                   GPIOC
#define LED4_GPIO_CLK                    RCC_AHBPeriph_GPIOC
/**
  * @}
  */ 

/** @addtogroup SSTM32L100C_DISCOVERY_LOW_LEVEL_BUTTON
  * @{
  */  
#define BUTTONn                          4

/**
 * @brief USER push-button
 */
#define USER_BUTTON_PIN                GPIO_Pin_0
#define USER_BUTTON_GPIO_PORT          GPIOA
#define USER_BUTTON_GPIO_CLK           RCC_AHBPeriph_GPIOA
#define USER_BUTTON_EXTI_LINE          EXTI_Line0
#define USER_BUTTON_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define USER_BUTTON_EXTI_PIN_SOURCE    EXTI_PinSource0
#define USER_BUTTON_EXTI_IRQn          EXTI0_IRQn 

#define UP_BUTTON_PIN                GPIO_Pin_1
#define UP_BUTTON_GPIO_PORT          GPIOA
#define UP_BUTTON_GPIO_CLK           RCC_AHBPeriph_GPIOA
#define UP_BUTTON_EXTI_LINE          EXTI_Line1
#define UP_BUTTON_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define UP_BUTTON_EXTI_PIN_SOURCE    EXTI_PinSource1
#define UP_BUTTON_EXTI_IRQn          EXTI1_IRQn

#define DOWN_BUTTON_PIN                GPIO_Pin_2
#define DOWN_BUTTON_GPIO_PORT          GPIOA
#define DOWN_BUTTON_GPIO_CLK           RCC_AHBPeriph_GPIOA
#define DOWN_BUTTON_EXTI_LINE          EXTI_Line2
#define DOWN_BUTTON_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define DOWN_BUTTON_EXTI_PIN_SOURCE    EXTI_PinSource2
#define DOWN_BUTTON_EXTI_IRQn          EXTI2_IRQn

#define MODE_BUTTON_PIN                GPIO_Pin_3
#define MODE_BUTTON_GPIO_PORT          GPIOA
#define MODE_BUTTON_GPIO_CLK           RCC_AHBPeriph_GPIOA
#define MODE_BUTTON_EXTI_LINE          EXTI_Line3
#define MODE_BUTTON_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define MODE_BUTTON_EXTI_PIN_SOURCE    EXTI_PinSource3
#define MODE_BUTTON_EXTI_IRQn          EXTI3_IRQn


/**
  * @}
  */ 
  
    
/** @defgroup STM32L100C_DISCOVERY_LOW_LEVEL_Exported_Macros
  * @{
  */  
/**
  * @}
  */ 

/** @defgroup STM32L100C_DISCOVERY_LOW_LEVEL_Exported_Functions
  * @{
  */
void STM_EVAL_LEDInit(Led_TypeDef Led);
void STM_EVAL_LEDOn(Led_TypeDef Led);
void STM_EVAL_LEDOff(Led_TypeDef Led);
void STM_EVAL_LEDToggle(Led_TypeDef Led);
void STM_EVAL_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint32_t STM_EVAL_PBGetState(Button_TypeDef Button);

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32L100C_DISCOVERY_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
