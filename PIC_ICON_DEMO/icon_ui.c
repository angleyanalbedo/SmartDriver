#include "icon_ui.h"
#include "tftlcd.h"
#include "SysTick.h"
#include "gui.h"
#include "touch.h"
#include "led.h"

#include "pic_kj_logo.h"
#include "led_icon.h"
#include "clock_icon.h"
#include "calc_icon.h"
#include "paint_icon.h"
#include "picture_icon.h"
#include "3d_icon.h"
#include "ebook_icon.h"
#include "notepad_icon.h"
#include "earthnet_icon.h"
#include "usb_icon.h"
#include "music_icon.h"
#include "camera_icon.h"
#include "com_icon.h"
#include "qrcode_icon.h"
#include "phone_icon.h"
#include "wireless_icon.h"



//ICON应用APP图标列表索引
const unsigned char* icon_ui_app_tbl[][2]=
{
	{gImage_led_icon,"LED"},
	{gImage_clock_icon,"时钟"},
	{gImage_calc_icon,"计算器"},
	{gImage_3d_icon,"3D"},
	{gImage_picture_icon,"相册"},
	{gImage_paint_icon,"小画家"},
	{gImage_ebook_icon,"电子书"},
	{gImage_notepad_icon,"记事本"},
	{gImage_usb_icon,"读卡器"},
	{gImage_earthnet_icon,"以太网"},
	{gImage_music_icon,"音乐"},
	{gImage_camera_icon,"照相机"},
	{gImage_com_icon,"通信"},
	{gImage_wireless_icon,"飞书"},
	{gImage_phone_icon,"电话"},
	{gImage_qrcode_icon,"二维码"}
};
	

//开机LOGO界面初始化
void ICON_KjDisplay_Init(void)
{
#if defined(TFTLCD_PIXEL_240X320)||defined(TFTLCD_PIXEL_320X480)		
	LCD_Clear(WHITE);
	LCD_ShowPicture(tftlcd_data.width/2-PIC_KJ_LOGO_WIDTH/2,tftlcd_data.height/2-PIC_KJ_LOGO_HEIGHT/2,PIC_KJ_LOGO_WIDTH,PIC_KJ_LOGO_HEIGHT,(u8 *)gImage_pic_kj_logo);
	delay_ms(1000);
#endif	
}

extern u8 rtc_showflag;
//人机界面初始化
void ICON_UI_Init(void)
{
	u8 i=0,j=0;
	
	LCD_Clear(UI_BACKCOLOR);//LIGHTGREEN	
	FRONT_COLOR=UI_FRONTCOLOR;
	BACK_COLOR=UI_BACKCOLOR;
	
	//顶部产品型号、公司名称、时间显示
	gui_show_string("STM32F103",10,2,100,TOP_STATUS_NAME_HEIGHT,TOP_STATUS_NAME_FONT_SIZE,FRONT_COLOR);
	gui_show_strmid(0,0,tftlcd_data.width,TOP_STATUS_NAME_HEIGHT,FRONT_COLOR,TOP_STATUS_NAME_FONT_SIZE,"PRECHIN");
	rtc_showflag=1;
	
	//APP应用图标、名称显示
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			LCD_ShowPicture(PIC_ICON_APP_XSTART+(PIC_ICON_APP_XSPACE+PIC_ICON_APP_WIDTH)*j,
							PIC_ICON_APP_YSTART+(PIC_ICON_APP_YSPACE+PIC_ICON_APP_HEIGHT)*i,
							PIC_ICON_APP_WIDTH,PIC_ICON_APP_HEIGHT,
							(u8 *)icon_ui_app_tbl[j+4*i][0]);
			gui_show_strmid(PIC_ICON_APP_XSTART+(PIC_ICON_APP_XSPACE+PIC_ICON_APP_WIDTH)*j,
							PIC_ICON_APP_YSTART+PIC_ICON_APP_HEIGHT+(PIC_ICON_APP_YSPACE+PIC_ICON_APP_HEIGHT)*i,
							PIC_ICON_APP_NAME_WIDTH,PIC_ICON_APP_NAME_HEIGHT,
							FRONT_COLOR,PIC_ICON_APP_NAME_FONT_SIZE,
							(u8 *)icon_ui_app_tbl[j+4*i][1]);
		}
	}
}

//判断哪个应用APP图标被按下，并返回对应索引
//返回值:0~15,被双击的图标编号
//		0xff,没有任何图标被双击或者按下
u8 get_icon_app_table(void)
{
	u8 index=0xff;
	static u16 curxpos=0;//当前tp按下的x坐标
	static u16 curypos=0;//当前tp按下的y坐标
	static u8 curtpsta=0;//触摸按下标志
	u8 i=0,j=0;
	
	tp_dev.scan(0);	//扫描
	if(tp_dev.sta&TP_PRES_DOWN)//有按键被按下
	{
		curxpos=tp_dev.x[0];	//记录当前坐标
		curypos=tp_dev.y[0];	//记录当前坐标
		curtpsta=1;
	}
	else	//按键松开了
	{
		if(curtpsta)//之前有按下
		{
			for(i=0;i<4;i++)
			{
				for(j=0;j<4;j++)
				{
					if(curxpos>(PIC_ICON_APP_XSTART+(PIC_ICON_APP_XSPACE+PIC_ICON_APP_WIDTH)*j)
						&& curxpos<(PIC_ICON_APP_XSTART+(PIC_ICON_APP_XSPACE+PIC_ICON_APP_WIDTH)*j+PIC_ICON_APP_WIDTH)
						&& curypos>(PIC_ICON_APP_YSTART+(PIC_ICON_APP_YSPACE+PIC_ICON_APP_HEIGHT)*i)
						&& curypos<(PIC_ICON_APP_YSTART+(PIC_ICON_APP_YSPACE+PIC_ICON_APP_HEIGHT)*i+PIC_ICON_APP_HEIGHT))
					{
						index=j+4*i;//得到选中的编号
						break;
					}
				}
			}
//			printf("index=%d\r\n",index);
//			printf("curxpos=%d  curypos=%d\r\n",curxpos,curypos);
		}
		curtpsta=0;//清空标志
	}
	return index;
}
