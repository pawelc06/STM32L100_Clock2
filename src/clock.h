/* Private variables ---------------------------------------------------------*/
RTC_InitTypeDef  RTC_InitStructure;
RTC_TimeTypeDef  RTC_TimeStructure;



static void RTC_Config(void);
static void RTC_AlarmConfig(void);
static void RTC_Config32768Internal(char *buf);
uint8_t bcd2dec(uint8_t numberbcd);
uint8_t dec2bcd(uint8_t numberdec);

void displayDate();
void updateAndDisplayDate();
void displayTime();
uint8_t parseNTPTime(char *buf,RTC_DateTypeDef *RTC_DateStruct, RTC_TimeTypeDef  *RTC_TimeStructure);
uint8_t getDayOfWeekNum(char *buf);
uint8_t getMonthNum(char *buf);

