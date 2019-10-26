/*
 * openweather.h
 *
 *  Created on: 26.10.2019
 *      Author: Pawel
 */

#ifndef OPENWEATHER_H_
#define OPENWEATHER_H_
#include "stdbool.h"
#include "stm32l1xx.h"
#include <stdio.h>
#include <time.h>


#define BUFFER_SIZE 2048
uint16_t getWeatherData();
void setCurrentTime();
void getCurrentTemp(char *tempStr);
void rxBufferFlush();
static void RTC_Config32768Internal(char *buf);
void getSunriseAndSunset(char *sunriseStr,char * sunsetStr);




#endif /* OPENWEATHER_H_ */
