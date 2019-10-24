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

	LCD_Write_Date(55, 100, &RTC_DateStruct);

}

void displayAlarm() {

	RTC_AlarmTypeDef RTC_AlarmStructure;

	RTC_GetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = 0x06;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x50;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x00;
		//RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

	LCD_Write_AlarmTimeBCD2(100, 205, &RTC_AlarmStructure);

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

