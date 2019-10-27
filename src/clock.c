#include "stm32l1xx_rtc.h"
#include "stm32l1xx_pwr.h"
#include "Lcd_Driver.h"
#include "clock.h"

volatile uint8_t day = 0;

void displayTime() {
	uint8_t rtc_time = 0;

	RTC_TimeTypeDef RTC_TimeStructure1;

	RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure1);

	LCD_Write_TimeBCD2(45, 0, &RTC_TimeStructure1);

}

void displayDate() {

	RTC_DateTypeDef RTC_DateStruct;

	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);

	day = RTC_DateStruct.RTC_Date;

	LCD_Write_Date(78, 85, &RTC_DateStruct);

}

void displayAlarm() {

	RTC_AlarmTypeDef RTC_AlarmStructure;
	//RTC_WaitForSynchro(); // 2.
	RTC_GetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
	//RTC_WaitForSynchro(); // 2.



	LCD_Write_AlarmTimeBCD2(100, 205, &RTC_AlarmStructure);

}

void writeSunriseSunsetTime(u16 xpos, u16 ypos, char *sunriseStr,char * sunsetStr) {


			tft_puts(xpos, ypos, sunriseStr, white, black);
			xpos+=180;
			tft_puts(xpos, ypos, sunsetStr, white, black);

}

void displayTemp(char *tempStr){
	LCD_Write_Curr_Temp(95, 170, tempStr);
}

void updateAndDisplayDate() {

	RTC_DateTypeDef RTC_DateStruct;

	do {
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	} while (RTC_DateStruct.RTC_Date == day);

	day = RTC_DateStruct.RTC_Date;

	LCD_Write_Date(55, 100, &RTC_DateStruct);

}

uint8_t bcd2dec(uint8_t numberbcd) {
	uint8_t h = numberbcd >> 4;

	return numberbcd - (((h << 1) + h) << 1);

}

uint8_t dec2bcd(uint8_t numberdec) {
	return (numberdec / 10 * 16 + numberdec % 10);

}

uint8_t parseNTPTime(char *buf, RTC_DateTypeDef *RTC_DateStruct,
		RTC_TimeTypeDef *RTC_TimeStructure) {
	uint8_t hour, min, sec, month, day;
	uint16_t year;
	char bufH[3];
	char bufM[3];
	char bufS[3];
	char bufYear[5];
	char bufMonth[3];
	char bufDay[3];
	char bufDayOfWeek[3];

	char *startPtr;

	hour = 13;
	min = 15;
	sec = 38;

	startPtr = strchr(buf, ':');
	if (startPtr) {
		strncpy(bufDayOfWeek, startPtr + 1, 3);
		bufDayOfWeek[3] = 0;
		RTC_DateStruct->RTC_WeekDay = getDayOfWeekNum(bufDayOfWeek);

		strncpy(bufMonth, startPtr + 5, 3);
		bufMonth[3] = 0;
		month = getMonthNum(bufMonth);

		strncpy(bufDay, startPtr + 9, 2);
		bufDay[2] = 0;
		day = atoi(bufDay);

		strncpy(bufH, startPtr + 12, 2);
		bufH[2] = 0;
		hour = atoi(bufH);

		strncpy(bufM, startPtr + 15, 2);
		bufM[2] = 0;
		min = atoi(bufM);

		strncpy(bufS, startPtr + 18, 2);
		bufS[2] = 0;
		sec = atoi(bufS);

		strncpy(bufYear, startPtr + 21, 4);
		bufYear[4] = 0;
		year = atoi(bufYear);
		RTC_DateStruct->RTC_Date = day;
		RTC_DateStruct->RTC_Month = month;
		RTC_DateStruct->RTC_Year = year - 2000;

		RTC_TimeStructure->RTC_Hours = dec2bcd(hour);
		RTC_TimeStructure->RTC_Minutes = dec2bcd(min);
		RTC_TimeStructure->RTC_Seconds = dec2bcd(sec);

		//DST adjunsmtent
		if(isDSTRTC(RTC_DateStruct)){
			RTC_TimeStructure->RTC_Hours+=1;
		}

		return 0;
	}

	return -1;
}

uint8_t getDayOfWeekNum(char *buf) {
	uint8_t n;

	if (!strcmp(buf, "Mon"))
		return 1;
	if (!strcmp(buf, "Tue"))
		return 2;
	if (!strcmp(buf, "Wed"))
		return 3;
	if (!strcmp(buf, "Thu"))
		return 4;
	if (!strcmp(buf, "Fri"))
		return 5;
	if (!strcmp(buf, "Sat"))
		return 6;
	if (!strcmp(buf, "Sun"))
		return 7;

}

uint8_t getMonthNum(char *buf) {
	uint8_t n;

	if (!strcmp(buf, "Jan"))
		return 1;
	if (!strcmp(buf, "Feb"))
		return 2;
	if (!strcmp(buf, "Mar"))
		return 3;
	if (!strcmp(buf, "Apr"))
		return 4;
	if (!strcmp(buf, "May"))
		return 5;
	if (!strcmp(buf, "Jun"))
		return 6;
	if (!strcmp(buf, "Jul"))
		return 7;
	if (!strcmp(buf, "Aug"))
		return 8;
	if (!strcmp(buf, "Sept"))
		return 9;
	if (!strcmp(buf, "Oct"))
		return 10;
	if (!strcmp(buf, "Nov"))
		return 11;
	if (!strcmp(buf, "Dec"))
		return 12;

}

char dow(int y, char m, char d)
   {
       static char t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
       y -= m < 3;
       return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
   }


char NthDate(int year, char month, char DOW, char NthWeek){
  char targetDate = 1;
  char firstDOW = dow(year,month,targetDate);
  while (firstDOW != DOW){
    firstDOW = (firstDOW+1)%7;
    targetDate++;
  }
  //Adjust for weeks
  targetDate += (NthWeek-1)*7;
  return targetDate;
}

bool isDST(struct tm timeToCheck){
	char lastSundayDay;
	if(timeToCheck.tm_mon >=3 && timeToCheck.tm_mon <=8 ){ //DST is from April to September always
		return true;
	}

	if((timeToCheck.tm_mon >=0 && timeToCheck.tm_mon <=1) || (timeToCheck.tm_mon >=10 && timeToCheck.tm_mon <=11)){ //no DST
			return false;
	}

	if((timeToCheck.tm_mon == 3)){ //Date in April
		lastSundayDay = NthDate(timeToCheck.tm_year+1900, timeToCheck.tm_mon+1, 0, 4);
		if(timeToCheck.tm_mday < lastSundayDay){
			return false;
		} else {
			return true;
		}
	}

	if((timeToCheck.tm_mon == 9)){ //Date in October
		lastSundayDay = NthDate(timeToCheck.tm_year+1900, timeToCheck.tm_mon+1, 0, 4);
		if(timeToCheck.tm_mday < lastSundayDay){
					return true;
				} else {
					return false;
				}
	}



}

bool isDSTRTC(RTC_DateTypeDef *RTC_DateStruct){
	char lastSundayDay;
	if(RTC_DateStruct->RTC_Month-1 >=3 && RTC_DateStruct->RTC_Month-1 <=8 ){ //DST is from April to September always
		return true;
	}

	if((RTC_DateStruct->RTC_Month-1 >=0 && RTC_DateStruct->RTC_Month-1 <=1) || (RTC_DateStruct->RTC_Month-1 >=10 && RTC_DateStruct->RTC_Month-1 <=11)){ //no DST
			return false;
	}

	if((RTC_DateStruct->RTC_Month-1 == 3)){ //Date in April
		lastSundayDay = NthDate(RTC_DateStruct->RTC_Year+2000, RTC_DateStruct->RTC_Month, 0, 4);
		if(RTC_DateStruct->RTC_Date < lastSundayDay){
			return false;
		} else {
			return true;
		}
	}

	if((RTC_DateStruct->RTC_Month-1 == 9)){ //Date in October
		lastSundayDay = NthDate(RTC_DateStruct->RTC_Year+2000, RTC_DateStruct->RTC_Month, 0, 4);
		if(RTC_DateStruct->RTC_Date < lastSundayDay){
					return true;
				} else {
					return false;
				}
	}



}

