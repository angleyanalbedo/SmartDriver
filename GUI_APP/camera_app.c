#include "camera_app.h"
#include "button.h"
#include "touch.h"
#include "common.h"	
#include "ov7670.h"  
#include "ov7725.h"
#include "sd_sdio.h"
#include "beep.h"
#include "key.h"
#include "led.h"
#include "math.h"	 
#include "time.h" 
#include "exti.h"
#include "piclib.h"	
#include "string.h"	
#include "rtc.h"	
#include "exti.h"

//传感器名字定义
#define  OV7725 1
#define  OV7670 2

extern vu8 ov_frame;//在time.c里面定义
extern vu8 ov_sta;//在exit.c里面定义	

//摄像头提示
u8*const camera_remind_tbl[3][GUI_LANGUAGE_NUM]=
{
{"初始化摄像头,请稍侯...","初始化摄像头,請稍後...","Camera Init,Please wait...",},	 
{"未检测到摄像头,请检查...","未檢測到摄像头,請檢查...","No Camera find,Please check...",},	 
{"保存为:","保存為:","SAVE AS:",},	 
};
//拍照成功提示框标题
u8*const camera_saveok_caption[GUI_LANGUAGE_NUM]=
{
"拍照成功！","拍照成功！","Take Photo OK！",
};
													    
//00级功能选项表标题
u8*const camera_l00fun_caption[GUI_LANGUAGE_NUM]=
{
"相机设置","相機設置","Camera Set",
};

//00级功能选项表
u8*const camera_l00fun_table[GUI_LANGUAGE_NUM][6]=
{
{"优先模式","场景设置","特效设置","亮度设置","色度设置","对比度设置",},
{"優先模式","場景設置","特效設置","亮度設置","色度設置","對比度設置",},
{"Priority","Scene","Effects","Brightness","Saturation","Contrast"},
};	
//////////////////////////////////////////////////////////////////////////	  
//10级功能选项表
//速度
u8*const camera_l10fun_table[GUI_LANGUAGE_NUM][2]=
{
{"清晰度优先","速度优先",},
{"清晰度優先","速度優先",},
{"Definition","Speed",},
};
//11级功能选项表
//场景模式
u8*const camera_l11fun_table[GUI_LANGUAGE_NUM][5]=
{
{"自动","晴天","阴天","办公室","家庭",},
{"自動","晴天","陰天","辦公室","家庭",},
{"Auto","Sunny","Cloudy","Office","Home"},
};	
//12级功能选项表
//特效设置
u8*const camera_l12fun_table[GUI_LANGUAGE_NUM][7]=
{
{"普通","负片","黑白","偏红色","偏绿色","偏蓝色","复古"},
{"普通","負片","黑白","偏紅色","偏綠色","偏藍色","復古"},
{"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique",},
};	
//13~15级功能选项表
//都是-2~2的5個值
u8*const camera_l135fun_table[GUI_LANGUAGE_NUM][5]=
{
{"-2","-1","0","+1","+2",},					   
{"-2","-1","0","+1","+2",},					   
{"-2","-1","0","+1","+2",},					   
};	   
//////////////////////////////////////////////////////////////////////////	  
//更新LCD显示
void camera_refresh(void)
{
	u32 j;
 	u16 color;	 
	if(ov_sta)//有帧中断更新？
	{
		LCD_Display_Dir(1); 
		LCD_Set_Window(0,0,320-1,240-1);//将显示区域设置到屏幕	
		OV7670_RRST=0;				//开始复位读指针 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST=1;				//复位读指针结束 
		OV7670_RCK_H;
		for(j=0;j<76800;j++)
		{
			OV7670_RCK_L;
			color=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK_H; 
			color<<=8;  
			OV7670_RCK_L;
			color|=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK_H; 
			LCD_WriteData_Color(color);     
		}   							  
 		ov_sta=0;					//清零帧中断标记
		ov_frame++; 
		LCD_Display_Dir(0);
	}
}	  
//通过时间获取文件名
//组合成:形如"0:PHOTO/PIC20120321210633.bmp"的文件名
void camera_new_pathname(u8 *pname)
{	 
	RTC_Get();//更新时间
	sprintf((char*)pname,"0:PHOTO/PIC%04d%02d%02d%02d%02d%02d.bmp",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
}
//摄像头初始化
//所有照片文件,均保存在SD卡PHOTO文件夹内.
void Camera_APP_Test(void)
{	
 	u8 rval=0;
	u8 res;
 	u8 *caption=0;
	u8 *pname;
	u8 selx=0;
	u8 l00sel=0,l10sel=0,l11sel=0,l12sel=0;//默认选择项
	u8 l345sel[3];
	u8 *psn;
	u8 speedctrl=0;
	u8 sensor=OV7725;

 	//提示开始检测OV7670
 	window_msg_box((tftlcd_data.width-200)/2,(tftlcd_data.height-80)/2,200,80,(u8*)camera_remind_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
	if(gui_phy.memdevflag&(1<<0))f_mkdir("0:PHOTO");//强制创建文件夹,给照相机用
	res=OV7725_Init();			//初始化OV7725
	if(res)
	{
		sensor=OV7670;
		res=OV7670_Init();	//OV7725失败，尝试初始化OV7670
	}
	if(res)					//初始化失败
	{
		sensor=0;
		window_msg_box((tftlcd_data.width-200)/2,(tftlcd_data.height-80)/2,200,80,(u8*)camera_remind_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
 		delay_ms(500);  
		rval=1;
	} 
	pname=gui_memin_malloc(40);	//申请40个字节内存,用于存放文件名
	psn=gui_memin_malloc(50);	//申请50个字节内存,用于存放类似：“保存为:0:PHOTO/PIC20120321210633.bmp"”的提示语
	if(pname==NULL)rval=1;		//申请失败	  
	if(rval==0)//OV7670正常
	{ 
		l345sel[0]=2;//亮度默认为2,实际值0
		l345sel[1]=2;//色度默认为2,实际值0
		l345sel[2]=2;//对比度默认为2,实际值0
		if(sensor==OV7670)
		{
			OV7670_Light_Mode(l11sel);
			OV7670_Special_Effects(l12sel);	 
			OV7670_Brightness(l345sel[0]);
			OV7670_Color_Saturation(l345sel[1]);
			OV7670_Contrast(l345sel[2]); 
			OV7670_Window_Set(12,176,240,320);	//设置窗口	  
			OV7670_CS=0; 	
		}else
		{	
			OV7725_Window_Set(320,240,1);	//VGA模式输出
			OV7725_CS=0;
		}			
		LCD_Clear(BLACK); 
		EXTI8_Init();					//使能外部中断8,捕获帧中断
		TIM5_Init(10000-1,7200-1);		//启动TIM5 ,1s中断一次	
//		SCCB_WR_Reg(0x0c,0x10);//镜像设置。如果画面出现镜像，可以通过该语句还原
		while(1)	 
		{		 	
			tp_dev.scan(0);
			if(tp_dev.sta&TP_PRES_DOWN)
			{    		 
				caption=(u8*)camera_l00fun_caption[gui_phy.language];
				res=app_items_sel((tftlcd_data.width-160)/2,(tftlcd_data.height-72-32*6)/2,160,72+32*6,(u8**)camera_l00fun_table[gui_phy.language],6,(u8*)&l00sel,0X90,caption);//单选 
				if(res==0)
				{
					caption=(u8*)camera_l00fun_table[gui_phy.language][l00sel];
					camera_refresh();//刷新一下
					switch(l00sel)
					{
						case 0://优先模式	  
							res=app_items_sel((tftlcd_data.width-160)/2,(tftlcd_data.height-72-32*2)/2,160,72+32*2,(u8**)camera_l10fun_table[gui_phy.language],2,(u8*)&l10sel,0X90,caption);//单选
							if(res==0)
							{
								if(l10sel==0)speedctrl=1;	//15帧
								else speedctrl=0; 			//30帧
								if(sensor==OV7670)SCCB_WR_Reg(0X11,speedctrl);//15/30帧速度,15帧时，清晰度变好
								else 	//7725通过PLL控制帧率
								{
									if(speedctrl)SCCB_WR_Reg(0x0D,0x01);
									else SCCB_WR_Reg(0x0D,0xc1);
								}
							}
							break;
						case 1://场景设置		 
							res=app_items_sel((tftlcd_data.width-160)/2,(tftlcd_data.height-72-32*5)/2,160,72+32*5,(u8**)camera_l11fun_table[gui_phy.language],5,(u8*)&l11sel,0X90,caption);//单选
							if(res==0)
							{
								if(sensor==OV7670)OV7670_Light_Mode(l11sel);		//白平衡设置	 
								else OV7725_Light_Mode(l11sel);						//白平衡设置
							}
							break;
						case 2://特效设置	  
							res=app_items_sel((tftlcd_data.width-160)/2,(tftlcd_data.height-72-32*7)/2,160,72+32*7,(u8**)camera_l12fun_table[gui_phy.language],7,(u8*)&l12sel,0X90,caption);//单选
							if(res==0)
							{
								if(sensor==OV7670)OV7670_Special_Effects(l12sel);	//特效设置
								else OV7725_Special_Effects(l12sel);				//特效设置
							}
							break;
						case 3://亮度,色度,对比度设置.	    
						case 4://亮度,色度,对比度设置.
						case 5://亮度,色度,对比度设置.
							selx=l345sel[l00sel-3];//得到之前的选择
							res=app_items_sel((tftlcd_data.width-160)/2,(tftlcd_data.height-72-32*5)/2,160,72+32*5,(u8**)camera_l135fun_table[gui_phy.language],5,(u8*)&selx,0X90,caption);//单选
							if(res==0)
							{
								l345sel[l00sel-3]=selx;//记录新值
							 	if(l00sel==3)
								{
									if(sensor==OV7670)OV7670_Brightness(selx);	   	//亮度设置
									else OV7725_Brightness((s8)selx-2);	  		 	//亮度设置 
								}
								if(l00sel==4)
								{
									if(sensor==OV7670)OV7670_Color_Saturation(selx);//色度设置
									else OV7725_Color_Saturation((s8)selx-2);		//色度设置 
								}
								if(l00sel==5)
								{
									if(sensor==OV7670)OV7670_Contrast(selx);		//对比度设置
									else OV7725_Brightness((s8)selx-2);				//对比度设置 
								}
							}
							break;
					}
				}
				LCD_Clear(BLACK);
 			}
 			if(KEY_Scan(1)==KEY1_PRESS)break;//KEY1返回
 			camera_refresh();
 			if(KEY_Scan(0)==KEY_UP_PRESS)//KEY_UP按下了一次,拍照一次
			{					 	
				LED1=0;//DS1亮,提示正在保存
				pname[0]='\0';//添加结束符
				camera_new_pathname(pname);
				if(pname!=NULL)
				{		    
					if(bmp_encode(pname,(tftlcd_data.width-240)/2,(tftlcd_data.height-320)/2,240,320,0))//拍照有误
					{
						window_msg_box((tftlcd_data.width-200)/2,(tftlcd_data.height-100)/2,200,100,(u8*)APP_CREAT_ERR_MSG_TBL[gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//提示SD卡是否存在
						delay_ms(2000);//等待2秒钟
					}else
					{
						strcpy((char*)psn,(const char *)camera_remind_tbl[2][gui_phy.language]);
						strcat((char*)psn,(const char *)pname);
 						window_msg_box((tftlcd_data.width-180)/2,(tftlcd_data.height-80)/2,180,80,psn,(u8*)camera_saveok_caption[gui_phy.language],12,0,0,0);
	 					BEEP=1;	//蜂鸣器短叫，提示拍照完成
						delay_ms(100);
			 		}
					system_task_return=0;//清除电容触摸
				}
				LED1=1;	//DS1灭,提示保存完成
				BEEP=0;	//蜂鸣器短叫
				delay_ms(1800);
			}   
 		}
	}
	gui_memin_free(pname);	
	gui_memin_free(psn);							  
	TIM_Cmd(TIM5, DISABLE);//关闭定时器5
	EXTI->IMR&=~(1<<8);//关闭line8中断	
	ICON_UI_Init();	
//	return 0;	 
}
