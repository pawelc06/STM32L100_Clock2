/*
 * spi_conf.h
 *
 *  Created on: 9 lip 2014
 *      Author: Pawe³
 */

#ifndef SPI_CONF_H_
#define SPI_CONF_H_

/****************************/
#define SD_SPI                           SPI1
#define SD_SPI_CLK                       RCC_APB2Periph_SPI1
#define SD_SPI_SCK_PIN                   GPIO_Pin_5                 /* PA.5 */
#define SD_SPI_SCK_GPIO_PORT             GPIOA                       /* GPIOB */
#define SD_SPI_SCK_GPIO_CLK              RCC_AHBPeriph_GPIOA
#define SD_SPI_SCK_SOURCE                GPIO_PinSource5
#define SD_SPI_SCK_AF                    GPIO_AF_SPI1

#define SD_SPI_MISO_PIN                  GPIO_Pin_6                 /* PB.14 */
#define SD_SPI_MISO_GPIO_PORT            GPIOA                       /* GPIOB */
#define SD_SPI_MISO_GPIO_CLK             RCC_AHBPeriph_GPIOA
#define SD_SPI_MISO_SOURCE               GPIO_PinSource6
#define SD_SPI_MISO_AF                   GPIO_AF_SPI1

#define SD_SPI_MOSI_PIN                  GPIO_Pin_7                 /* PB.15 */
#define SD_SPI_MOSI_GPIO_PORT            GPIOA                       /* GPIOB */
#define SD_SPI_MOSI_GPIO_CLK             RCC_AHBPeriph_GPIOA
#define SD_SPI_MOSI_SOURCE               GPIO_PinSource7
#define SD_SPI_MOSI_AF                   GPIO_AF_SPI1

#define SD_CS_PIN                        GPIO_Pin_4                  /* PD.07 */
#define SD_CS_GPIO_PORT                  GPIOC                       /* GPIOD */
#define SD_CS_GPIO_CLK                   RCC_AHBPeriph_GPIOC

#define SD_DETECT_PIN                    GPIO_Pin_11                  /* PE.06 */
#define SD_DETECT_EXTI_LINE              EXTI_Line6
#define SD_DETECT_EXTI_PIN_SOURCE        EXTI_PinSource6

#define SD_DETECT_GPIO_PORT              GPIOA                       /* GPIOE */
#define SD_DETECT_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define SD_DETECT_EXTI_PORT_SOURCE       EXTI_PortSourceGPIOA
#define SD_DETECT_EXTI_IRQn              EXTI9_5_IRQn
/**************************/


#endif /* SPI_CONF_H_ */
