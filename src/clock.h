/* Private variables ---------------------------------------------------------*/
RTC_InitTypeDef  RTC_InitStructure;
RTC_TimeTypeDef  RTC_TimeStructure;
typedef uint8_t u8;
typedef uint16_t u16;
#include "stdbool.h"
#include <time.h>


static void RTC_Config(void);
static void RTC_AlarmConfig(void);
//static void RTC_Config32768Internal(char *buf);
uint8_t bcd2dec(uint8_t numberbcd);
uint8_t dec2bcd(uint8_t numberdec);

void displayDate(bool forceNoBlink);
void updateAndDisplayDate(bool forceNoBlink);
void displayTime(bool forceNoBlink);
uint8_t parseNTPTime(char *buf,RTC_DateTypeDef *RTC_DateStruct, RTC_TimeTypeDef  *RTC_TimeStructure);
uint8_t getDayOfWeekNum(char *buf);
uint8_t getMonthNum(char *buf);
void writeSunriseSunsetTime(u16 xpos, u16 ypos, char *sunriseStr,char * sunsetStr) ;
char NthDate(int year, char month, char DOW, char NthWeek);
bool isDSTRTC(RTC_DateTypeDef *RTC_DateStruct);
bool isDST(struct tm timeToCheck);
char dow(int y, char m, char d);
void displayTemp(char *tempStr);

