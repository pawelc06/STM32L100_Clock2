#define LCD_8BIT_MODE   0	//定义液晶驱动为8位模式   1为使能 数据口使用高8位DP_H
#define LCD_16BIT_MODE  1	//定义液晶驱动为16位模式  1为使能

#define LCD_DRIVER_ILI9320  0
#define LCD_DRIVER_ILI9325  0
#define LCD_DRIVER_ILI9328  0
#define LCD_DRIVER_ILI9331  0
#define LCD_DRIVER_R61509V  0
#define LCD_DRIVER_HX8352   1

//#define X_MAX_PIXEL	        240
//#define Y_MAX_PIXEL	        320

#define ILI9341_NOP 0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID 0x04
#define ILI9341_RDDST 0x09

#define ILI9341_SLPIN 0x10
#define ILI9341_SLPOUT 0x11
#define ILI9341_PTLON 0x12
#define ILI9341_NORON 0x13

#define ILI9341_RDMODE 0x0A
#define ILI9341_RDMADCTL 0x0B
#define ILI9341_RDPIXFMT 0x0C
#define ILI9341_RDIMGFMT 0x0A
#define ILI9341_RDSELFDIAG 0x0F

#define ILI9341_INVOFF 0x20
#define ILI9341_INVON 0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON 0x29

#define ILI9341_CASET 0x2A
#define ILI9341_PASET 0x2B
#define ILI9341_RAMWR 0x2C
#define ILI9341_RAMRD 0x2E

#define ILI9341_PTLAR 0x30
#define ILI9341_MADCTL 0x36
#define ILI9341_PIXFMT 0x3A

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR 0xB4
#define ILI9341_DFUNCTR 0xB6

#define ILI9341_PWCTR1 0xC0
#define ILI9341_PWCTR2 0xC1
#define ILI9341_PWCTR3 0xC2
#define ILI9341_PWCTR4 0xC3
#define ILI9341_PWCTR5 0xC4
#define ILI9341_VMCTR1 0xC5
#define ILI9341_VMCTR2 0xC7

#define ILI9341_RDID1 0xDA
#define ILI9341_RDID2 0xDB
#define ILI9341_RDID3 0xDC
#define ILI9341_RDID4 0xDD

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1


#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH 0x04



#define X_MAX_PIXEL	        320
#define Y_MAX_PIXEL	        240


#if ((LCD_DRIVER_ILI9320)||(LCD_DRIVER_ILI9325)||(LCD_DRIVER_ILI9328)||(LCD_DRIVER_ILI9331))
//9320功能寄存器地址
#define WINDOW_XADDR_START		0x0050 // 水平的开始地址组
#define WINDOW_XADDR_END		0x0051 // 水平的结束地址组
#define WINDOW_YADDR_START		0x0052 // 垂直的开始地址组
#define WINDOW_YADDR_END		0x0053 // 垂直的结束地址组
#define GRAM_XADDR		    	0x0020 // GRAM 水平的地址组
#define GRAM_YADDR		    	0x0021 // GRAM 垂直的地址组
#define GRAMWR 			    	0x0022 // GRAM
#endif

#if (LCD_DRIVER_R61509V)
//9325功能寄存器地址
#define WINDOW_XADDR_START		0x0210 // 水平的开始地址组
#define WINDOW_XADDR_END		0x0211 // 水平的结束地址组
#define WINDOW_YADDR_START		0x0212 // 垂直的开始地址组
#define WINDOW_YADDR_END		0x0213 // 垂直的结束地址组
#define GRAM_XADDR		    	0x0200 // GRAM 水平的地址组
#define GRAM_YADDR		    	0x0201 // GRAM 垂直的地址组
#define GRAMWR 			    	0x0202 // GRAM
#endif

#if (LCD_DRIVER_HX8352)
#define WINDOW_XADDR_START_H	0x0002 // 水平的开始地址组
#define WINDOW_XADDR_START_L	0x0003 // 水平的开始地址组
#define WINDOW_XADDR_END_H		0x0004 // 水平的结束地址组
#define WINDOW_XADDR_END_L		0x0005 // 水平的结束地址组

#define WINDOW_YADDR_START_H	0x0006 // 垂直的开始地址组
#define WINDOW_YADDR_START_L	0x0007 // 垂直的开始地址组
#define WINDOW_YADDR_END_H		0x0008 // 垂直的结束地址组
#define WINDOW_YADDR_END_L		0x0009 // 垂直的结束地址组

#define GRAM_XADDR_H		    0x0002 // GRAM 水平的地址组
#define GRAM_XADDR_L		    0x0003 // GRAM 水平的地址组
#define GRAM_YADDR_H		    0x0004 // GRAM 垂直的地址组
#define GRAM_YADDR_L		    0x0005 // GRAM 垂直的地址组
#define GRAMWR 			    	0x0022 // GRAM
#endif

