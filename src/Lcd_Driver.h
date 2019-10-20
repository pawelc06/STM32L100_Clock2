

typedef uint8_t u8;
typedef uint16_t u16;
#include "stdbool.h"
#include "stm32l1xx_rtc.h"
#include "TFT_FONT/mk_fonts.h"


#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//灰色0 3165 00110 001011 00101
#define GRAY1   0x8410      	//灰色1      00000 000000 00000
#define GRAY2   0x4208      	//灰色2  1111111111011111

// deklaracje sta硑ch kolor體
#define red 	0xff0000
#define maroon 	0x8f0000
#define blue 	0x0000ff
#define navy 	0x00008f
#define green   0x00cf00
#define lime    0xafff00
#define yellow  0xffff00
#define cyan  	0x00ffff
#define fuchsia 0xff00ff
#define purple  0x8f008f
#define orange  0xff7c00
#define honig   0xf6c915
#define gray	0x7f7f7f
#define silver	0xafafaf

#define black   0x000000
#define white  	0xffffff


#define LCD_CTRL   	  	GPIOB		//定义TFT数据端口
//#define LCD_DATA   	  	GPIOD		//定义TFT数据端口

#define LCD_CS        	GPIO_Pin_12  //MCU_PB2--->>TFT --PIN_CS
#define LCD_SCL        	GPIO_Pin_13	//PB13--->>TFT --SCL

#define LCD_SDA        	GPIO_Pin_15	//PB15 MOSI--->>TFT --SDA 
#define LCD_SDO        	GPIO_Pin_14	//PB14 MISO--->>TFT --SDA

#define LCD_RS         	GPIO_Pin_10	//PB11--->>TFT --RS
#define LCD_RST     	GPIO_Pin_0	//PB10--->>TFT --RST

/*********************************/

// deklaracje zmiennych
extern uint8_t Red, Green, Blue;
extern uint16_t Color;
extern uint16_t bkColor;

extern uint8_t Bk_red, Bk_green, Bk_blue;


extern uint8_t ico1[];

extern int CX, CY;

extern int frame_ptr;

// deklaracje funkcji


void Set_color32( uint32_t color );
void Set_bk_color32( uint32_t color );

void Draw_bk_pixel(void);
void Draw_pixel(void);

void Set_bk_color( uint8_t R, uint8_t G, uint8_t B );
void Set_color( uint8_t R, uint8_t G, uint8_t B );



#define	LCD_CS_SET  	GPIO_SetBits(GPIOB,LCD_CS);
#define	LCD_RS_SET  	GPIO_SetBits(GPIOB,LCD_RS);
#define	LCD_SDA_SET  	GPIO_SetBits(GPIOB,LCD_SDA);
#define	LCD_SCL_SET  	GPIO_SetBits(GPIOB,LCD_SCL);
#define	LCD_RST_SET  	GPIO_SetBits(GPIOB,LCD_RST);


#define	LCD_CS_CLR  	GPIO_ResetBits(GPIOB,LCD_CS);
#define	LCD_RS_CLR  	GPIO_ResetBits(GPIOB,LCD_RS);
#define	LCD_SDA_CLR  	GPIO_ResetBits(GPIOB,LCD_SDA);
#define	LCD_SCL_CLR  	GPIO_ResetBits(GPIOB,LCD_SCL);
#define	LCD_RST_CLR  	GPIO_ResetBits(GPIOB,LCD_RST);





#define LCD_DATAOUT(x) LCD_DATA->ODR=x; //数据输出
#define LCD_DATAIN     LCD_DATA->IDR;   //数据输入

#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
LCD_DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 



void LCD_GPIO_Init(void);
void Lcd_WriteIndex(u8 Index);
void Lcd_WriteData(u8 Data);
//void Lcd_WriteReg(u8 Index,u8 Data);
u16 Lcd_ReadReg(u8 LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Init2(void);
void Lcd_Clear(u16 Color);
void Lcd_Clear2(u16 Color);
void Lcd_SetXY(u16 x,u16 y);
void Gui_DrawPoint(u16 x,u16 y,u16 Data);
unsigned int Lcd_ReadPoint(u16 x,u16 y);
char readID(void);
char Read_Register(char Addr, char xParameter);
static uint8_t spi_readwrite(uint32_t spi, uint8_t data);
void LCD_Write_TimeBCD(u16 xpos,u16 ypos,RTC_TimeTypeDef * RTC_TimeStructure1);
void LCD_Write_Colon(u16 xpos,u16 ypos);
void itoa_mod(uint16_t n, uint8_t s[]);
void reverse(int8_t s[]);
uint32_t LCD_RGB_24to16(uint32_t color);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color);
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end);
static void SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte);
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void SPI_WriteWord(SPI_TypeDef* SPIx,uint16_t Word);
void Lcd_WritePixelData(uint16_t bkColor);
