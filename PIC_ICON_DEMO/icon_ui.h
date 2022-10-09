#ifndef _icon_ui_H
#define _icon_ui_H

#include "system.h"
#include "tftlcd.h"

//根据TFTLCD触摸屏型号选择像素大小，选择对应的宏定义值
#if	defined(TFTLCD_HX8357D)||defined(TFTLCD_HX8352C)||defined(TFTLCD_ILI9341)|| \
	defined(TFTLCD_R61509V)||defined(TFTLCD_R61509VN)||defined(TFTLCD_R61509V3)|| \
	defined(TFTLCD_ST7793)||defined(TFTLCD_ILI9325)||defined(TFTLCD_R61509VE)
#define TFTLCD_PIXEL_240X320	//TFTLCD触摸屏像素：240*320、240*400
#endif

#if	defined(TFTLCD_ILI9486)||defined(TFTLCD_ILI9327)|| \
	defined(TFTLCD_HX8357DN)||defined(TFTLCD_ILI9481)
#define TFTLCD_PIXEL_320X480	//TFTLCD触摸屏像素：320*480
#endif

#if	defined(TFTLCD_NT35510)||defined(TFTLCD_SSD1963) 
#define TFTLCD_PIXEL_480X800	//TFTLCD触摸屏像素：480*800
#endif


//开机LOGO图标尺寸
#define PIC_KJ_LOGO_WIDTH		175
#define PIC_KJ_LOGO_HEIGHT		83

//UI界面背景、前景色
#define UI_BACKCOLOR			0x000a	//深蓝
#define UI_FRONTCOLOR			0xFFFF	//白色



#ifdef TFTLCD_PIXEL_240X320		//TFTLCD触摸屏像素：240*320、240*400
//ICON应用图标尺寸
#define PIC_ICON_APP_WIDTH		46
#define PIC_ICON_APP_HEIGHT		46

#define PIC_ICON_APP_XSTART		10
#define PIC_ICON_APP_YSTART		20

#define PIC_ICON_APP_XSPACE		10
#define PIC_ICON_APP_YSPACE		20

//APP应用图标名称显示尺寸
#define PIC_ICON_APP_NAME_WIDTH		PIC_ICON_APP_WIDTH
#define PIC_ICON_APP_NAME_HEIGHT	16
#define PIC_ICON_APP_NAME_FONT_SIZE	12

//顶部状态显示（产品型号、时间）
#define TOP_STATUS_NAME_HEIGHT		16
#define TOP_STATUS_NAME_FONT_SIZE	12
#endif

#ifdef TFTLCD_PIXEL_320X480		//TFTLCD触摸屏像素：320*480
#define PIC_ICON_APP_WIDTH		66
#define PIC_ICON_APP_HEIGHT		66

#define PIC_ICON_APP_XSTART		13
#define PIC_ICON_APP_YSTART		25

#define PIC_ICON_APP_XSPACE		10
#define PIC_ICON_APP_YSPACE		30

//APP应用图标名称显示尺寸
#define PIC_ICON_APP_NAME_WIDTH		PIC_ICON_APP_WIDTH
#define PIC_ICON_APP_NAME_HEIGHT	20
#define PIC_ICON_APP_NAME_FONT_SIZE	12

//顶部状态显示（产品型号、时间）
#define TOP_STATUS_NAME_HEIGHT		20
#define TOP_STATUS_NAME_FONT_SIZE	16
#endif

#ifdef TFTLCD_PIXEL_480X800		//TFTLCD触摸屏像素：480*800
#define PIC_ICON_APP_WIDTH		86
#define PIC_ICON_APP_HEIGHT		86

#define PIC_ICON_APP_XSTART		25
#define PIC_ICON_APP_YSTART		25

#define PIC_ICON_APP_XSPACE		30
#define PIC_ICON_APP_YSPACE		30

//APP应用图标名称显示尺寸
#define PIC_ICON_APP_NAME_WIDTH		PIC_ICON_APP_WIDTH
#define PIC_ICON_APP_NAME_HEIGHT	20
#define PIC_ICON_APP_NAME_FONT_SIZE	16

//顶部状态显示（产品型号、时间）
#define TOP_STATUS_NAME_HEIGHT		20
#define TOP_STATUS_NAME_FONT_SIZE	16
#endif





void ICON_KjDisplay_Init(void);
void ICON_UI_Init(void);
u8 get_icon_app_table(void);

#endif
