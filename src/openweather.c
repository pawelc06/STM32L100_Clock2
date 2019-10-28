#include "openweather.h"
#include "clock.h"

extern char rxBuffer[];
extern bool rxMsgReady;
extern uint16_t rxPointer;
char uartResp[BUFFER_SIZE];

static void RTC_Config32768Internal(char *buf) {
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	RTC_DateTypeDef RTC_DateStruct;
	RTC_InitTypeDef  RTC_InitStructure;
	RTC_TimeTypeDef  RTC_TimeStructure;

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
			"AT+CIPSNTPCFG=1,1,\"0.pl.pool.ntp.org\",\"ntp.itl.waw.pl\",\"vega.cbk.poznan.pl\"\r\n");

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

uint16_t getWeatherData(){
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

		delay_ms(500);

		strcpy(uartResp, rxBuffer);
		if(uartResp){
			return(strlen(uartResp));
		} else {
			return 0;
		}

}

void getCurrentTemp(char *tempStr) {
	float temp;
		char *tempStrPtr = strstr(uartResp, "\"temp\":");
	if (tempStrPtr) {
		strncpy(tempStr, tempStrPtr + 7, 6);
		tempStr[5] = 0;
		//temp = atof(tempStr)-273;
		sscanf(tempStr, "%f", &temp);
		temp = temp - 273;
		sprintf(tempStr, "%.1f~C ", temp);
	}
}

void getSunriseAndSunset(char *sunriseStr,char * sunsetStr) {
	uint32_t sunRiseTimestamp;
	uint32_t sunSetTimestamp;
	char tempStr[11];
	char *tempStrPtr;

	time_t     sunsetTime,sunriseTime ;
	time_t timeStructure1,timeStructure2;
	struct tm  sunsetUtcStruct;
	struct tm  sunriseUtcStruct;
	struct tm  time1,time2;
    char       buf[80];

	tempStrPtr = strstr(uartResp, "\"sunrise\":");

	if (tempStrPtr) {
		strncpy(tempStr, tempStrPtr + 10, 10);
		tempStr[10] = 0;
		sunRiseTimestamp = atol(tempStr);
	}

	tempStrPtr = strstr(uartResp, "\"sunset\":");

	if (tempStrPtr) {
		strncpy(tempStr, tempStrPtr + 9, 10);
		tempStr[10] = 0;
		sunSetTimestamp = atol(tempStr);
	}

	timeStructure1 = (time_t) sunRiseTimestamp;
			time1 = *localtime(&timeStructure1);
			bool isDst = isDST(time1);

	if(isDst){
		sunriseTime = sunRiseTimestamp+7200;
		sunsetTime = sunSetTimestamp+7200;
	} else {
		sunriseTime = sunRiseTimestamp+3600;
		sunsetTime = sunSetTimestamp+3600;
	}

	    // Get current time
	    //time(&now);


	    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
	sunriseUtcStruct = *localtime(&sunriseTime);
	sunsetUtcStruct = *localtime(&sunsetTime);



	strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &sunriseUtcStruct);
	strncpy(sunriseStr,buf+15,5);
	sunriseStr[5] = 0;
	strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &sunsetUtcStruct);
	strncpy(sunsetStr,buf+15,5);
	sunsetStr[5] = 0;
}

void rxBufferFlush() {
	memset(rxBuffer, 0, BUFFER_SIZE - 1);
	rxPointer = 0;
	rxMsgReady = false;
}
