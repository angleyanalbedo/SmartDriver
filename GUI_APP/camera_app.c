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

//���������ֶ���
#define  OV7725 1
#define  OV7670 2

extern vu8 ov_frame;//��time.c���涨��
extern vu8 ov_sta;//��exit.c���涨��	

//����ͷ��ʾ
u8*const camera_remind_tbl[3][GUI_LANGUAGE_NUM]=
{
{"��ʼ������ͷ,���Ժ�...","��ʼ������ͷ,Ո����...","Camera Init,Please wait...",},	 
{"δ��⵽����ͷ,����...","δ�z�y������ͷ,Ո�z��...","No Camera find,Please check...",},	 
{"����Ϊ:","�����:","SAVE AS:",},	 
};
//���ճɹ���ʾ�����
u8*const camera_saveok_caption[GUI_LANGUAGE_NUM]=
{
"���ճɹ���","���ճɹ���","Take Photo OK��",
};
													    
//00������ѡ������
u8*const camera_l00fun_caption[GUI_LANGUAGE_NUM]=
{
"�������","���C�O��","Camera Set",
};

//00������ѡ���
u8*const camera_l00fun_table[GUI_LANGUAGE_NUM][6]=
{
{"����ģʽ","��������","��Ч����","��������","ɫ������","�Աȶ�����",},
{"����ģʽ","�����O��","��Ч�O��","�����O��","ɫ���O��","���ȶ��O��",},
{"Priority","Scene","Effects","Brightness","Saturation","Contrast"},
};	
//////////////////////////////////////////////////////////////////////////	  
//10������ѡ���
//�ٶ�
u8*const camera_l10fun_table[GUI_LANGUAGE_NUM][2]=
{
{"����������","�ٶ�����",},
{"�����ȃ���","�ٶȃ���",},
{"Definition","Speed",},
};
//11������ѡ���
//����ģʽ
u8*const camera_l11fun_table[GUI_LANGUAGE_NUM][5]=
{
{"�Զ�","����","����","�칫��","��ͥ",},
{"�Ԅ�","����","���","�k����","��ͥ",},
{"Auto","Sunny","Cloudy","Office","Home"},
};	
//12������ѡ���
//��Ч����
u8*const camera_l12fun_table[GUI_LANGUAGE_NUM][7]=
{
{"��ͨ","��Ƭ","�ڰ�","ƫ��ɫ","ƫ��ɫ","ƫ��ɫ","����"},
{"��ͨ","ؓƬ","�ڰ�","ƫ�tɫ","ƫ�Gɫ","ƫ�{ɫ","�͹�"},
{"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique",},
};	
//13~15������ѡ���
//����-2~2��5��ֵ
u8*const camera_l135fun_table[GUI_LANGUAGE_NUM][5]=
{
{"-2","-1","0","+1","+2",},					   
{"-2","-1","0","+1","+2",},					   
{"-2","-1","0","+1","+2",},					   
};	   
//////////////////////////////////////////////////////////////////////////	  
//����LCD��ʾ
void camera_refresh(void)
{
	u32 j;
 	u16 color;	 
	if(ov_sta)//��֡�жϸ��£�
	{
		LCD_Display_Dir(1); 
		LCD_Set_Window(0,0,320-1,240-1);//����ʾ�������õ���Ļ	
		OV7670_RRST=0;				//��ʼ��λ��ָ�� 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST=1;				//��λ��ָ����� 
		OV7670_RCK_H;
		for(j=0;j<76800;j++)
		{
			OV7670_RCK_L;
			color=GPIOC->IDR&0XFF;	//������
			OV7670_RCK_H; 
			color<<=8;  
			OV7670_RCK_L;
			color|=GPIOC->IDR&0XFF;	//������
			OV7670_RCK_H; 
			LCD_WriteData_Color(color);     
		}   							  
 		ov_sta=0;					//����֡�жϱ��
		ov_frame++; 
		LCD_Display_Dir(0);
	}
}	  
//ͨ��ʱ���ȡ�ļ���
//��ϳ�:����"0:PHOTO/PIC20120321210633.bmp"���ļ���
void camera_new_pathname(u8 *pname)
{	 
	RTC_Get();//����ʱ��
	sprintf((char*)pname,"0:PHOTO/PIC%04d%02d%02d%02d%02d%02d.bmp",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
}
//����ͷ��ʼ��
//������Ƭ�ļ�,��������SD��PHOTO�ļ�����.
void Camera_APP_Test(void)
{	
 	u8 rval=0;
	u8 res;
 	u8 *caption=0;
	u8 *pname;
	u8 selx=0;
	u8 l00sel=0,l10sel=0,l11sel=0,l12sel=0;//Ĭ��ѡ����
	u8 l345sel[3];
	u8 *psn;
	u8 speedctrl=0;
	u8 sensor=OV7725;

 	//��ʾ��ʼ���OV7670
 	window_msg_box((tftlcd_data.width-200)/2,(tftlcd_data.height-80)/2,200,80,(u8*)camera_remind_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
	if(gui_phy.memdevflag&(1<<0))f_mkdir("0:PHOTO");//ǿ�ƴ����ļ���,���������
	res=OV7725_Init();			//��ʼ��OV7725
	if(res)
	{
		sensor=OV7670;
		res=OV7670_Init();	//OV7725ʧ�ܣ����Գ�ʼ��OV7670
	}
	if(res)					//��ʼ��ʧ��
	{
		sensor=0;
		window_msg_box((tftlcd_data.width-200)/2,(tftlcd_data.height-80)/2,200,80,(u8*)camera_remind_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
 		delay_ms(500);  
		rval=1;
	} 
	pname=gui_memin_malloc(40);	//����40���ֽ��ڴ�,���ڴ���ļ���
	psn=gui_memin_malloc(50);	//����50���ֽ��ڴ�,���ڴ�����ƣ�������Ϊ:0:PHOTO/PIC20120321210633.bmp"������ʾ��
	if(pname==NULL)rval=1;		//����ʧ��	  
	if(rval==0)//OV7670����
	{ 
		l345sel[0]=2;//����Ĭ��Ϊ2,ʵ��ֵ0
		l345sel[1]=2;//ɫ��Ĭ��Ϊ2,ʵ��ֵ0
		l345sel[2]=2;//�Աȶ�Ĭ��Ϊ2,ʵ��ֵ0
		if(sensor==OV7670)
		{
			OV7670_Light_Mode(l11sel);
			OV7670_Special_Effects(l12sel);	 
			OV7670_Brightness(l345sel[0]);
			OV7670_Color_Saturation(l345sel[1]);
			OV7670_Contrast(l345sel[2]); 
			OV7670_Window_Set(12,176,240,320);	//���ô���	  
			OV7670_CS=0; 	
		}else
		{	
			OV7725_Window_Set(320,240,1);	//VGAģʽ���
			OV7725_CS=0;
		}			
		LCD_Clear(BLACK); 
		EXTI8_Init();					//ʹ���ⲿ�ж�8,����֡�ж�
		TIM5_Init(10000-1,7200-1);		//����TIM5 ,1s�ж�һ��	
//		SCCB_WR_Reg(0x0c,0x10);//�������á����������־��񣬿���ͨ������仹ԭ
		while(1)	 
		{		 	
			tp_dev.scan(0);
			if(tp_dev.sta&TP_PRES_DOWN)
			{    		 
				caption=(u8*)camera_l00fun_caption[gui_phy.language];
				res=app_items_sel((tftlcd_data.width-160)/2,(tftlcd_data.height-72-32*6)/2,160,72+32*6,(u8**)camera_l00fun_table[gui_phy.language],6,(u8*)&l00sel,0X90,caption);//��ѡ 
				if(res==0)
				{
					caption=(u8*)camera_l00fun_table[gui_phy.language][l00sel];
					camera_refresh();//ˢ��һ��
					switch(l00sel)
					{
						case 0://����ģʽ	  
							res=app_items_sel((tftlcd_data.width-160)/2,(tftlcd_data.height-72-32*2)/2,160,72+32*2,(u8**)camera_l10fun_table[gui_phy.language],2,(u8*)&l10sel,0X90,caption);//��ѡ
							if(res==0)
							{
								if(l10sel==0)speedctrl=1;	//15֡
								else speedctrl=0; 			//30֡
								if(sensor==OV7670)SCCB_WR_Reg(0X11,speedctrl);//15/30֡�ٶ�,15֡ʱ�������ȱ��
								else 	//7725ͨ��PLL����֡��
								{
									if(speedctrl)SCCB_WR_Reg(0x0D,0x01);
									else SCCB_WR_Reg(0x0D,0xc1);
								}
							}
							break;
						case 1://��������		 
							res=app_items_sel((tftlcd_data.width-160)/2,(tftlcd_data.height-72-32*5)/2,160,72+32*5,(u8**)camera_l11fun_table[gui_phy.language],5,(u8*)&l11sel,0X90,caption);//��ѡ
							if(res==0)
							{
								if(sensor==OV7670)OV7670_Light_Mode(l11sel);		//��ƽ������	 
								else OV7725_Light_Mode(l11sel);						//��ƽ������
							}
							break;
						case 2://��Ч����	  
							res=app_items_sel((tftlcd_data.width-160)/2,(tftlcd_data.height-72-32*7)/2,160,72+32*7,(u8**)camera_l12fun_table[gui_phy.language],7,(u8*)&l12sel,0X90,caption);//��ѡ
							if(res==0)
							{
								if(sensor==OV7670)OV7670_Special_Effects(l12sel);	//��Ч����
								else OV7725_Special_Effects(l12sel);				//��Ч����
							}
							break;
						case 3://����,ɫ��,�Աȶ�����.	    
						case 4://����,ɫ��,�Աȶ�����.
						case 5://����,ɫ��,�Աȶ�����.
							selx=l345sel[l00sel-3];//�õ�֮ǰ��ѡ��
							res=app_items_sel((tftlcd_data.width-160)/2,(tftlcd_data.height-72-32*5)/2,160,72+32*5,(u8**)camera_l135fun_table[gui_phy.language],5,(u8*)&selx,0X90,caption);//��ѡ
							if(res==0)
							{
								l345sel[l00sel-3]=selx;//��¼��ֵ
							 	if(l00sel==3)
								{
									if(sensor==OV7670)OV7670_Brightness(selx);	   	//��������
									else OV7725_Brightness((s8)selx-2);	  		 	//�������� 
								}
								if(l00sel==4)
								{
									if(sensor==OV7670)OV7670_Color_Saturation(selx);//ɫ������
									else OV7725_Color_Saturation((s8)selx-2);		//ɫ������ 
								}
								if(l00sel==5)
								{
									if(sensor==OV7670)OV7670_Contrast(selx);		//�Աȶ�����
									else OV7725_Brightness((s8)selx-2);				//�Աȶ����� 
								}
							}
							break;
					}
				}
				LCD_Clear(BLACK);
 			}
 			if(KEY_Scan(1)==KEY1_PRESS)break;//KEY1����
 			camera_refresh();
 			if(KEY_Scan(0)==KEY_UP_PRESS)//KEY_UP������һ��,����һ��
			{					 	
				LED1=0;//DS1��,��ʾ���ڱ���
				pname[0]='\0';//��ӽ�����
				camera_new_pathname(pname);
				if(pname!=NULL)
				{		    
					if(bmp_encode(pname,(tftlcd_data.width-240)/2,(tftlcd_data.height-320)/2,240,320,0))//��������
					{
						window_msg_box((tftlcd_data.width-200)/2,(tftlcd_data.height-100)/2,200,100,(u8*)APP_CREAT_ERR_MSG_TBL[gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//��ʾSD���Ƿ����
						delay_ms(2000);//�ȴ�2����
					}else
					{
						strcpy((char*)psn,(const char *)camera_remind_tbl[2][gui_phy.language]);
						strcat((char*)psn,(const char *)pname);
 						window_msg_box((tftlcd_data.width-180)/2,(tftlcd_data.height-80)/2,180,80,psn,(u8*)camera_saveok_caption[gui_phy.language],12,0,0,0);
	 					BEEP=1;	//�������̽У���ʾ�������
						delay_ms(100);
			 		}
					system_task_return=0;//������ݴ���
				}
				LED1=1;	//DS1��,��ʾ�������
				BEEP=0;	//�������̽�
				delay_ms(1800);
			}   
 		}
	}
	gui_memin_free(pname);	
	gui_memin_free(psn);							  
	TIM_Cmd(TIM5, DISABLE);//�رն�ʱ��5
	EXTI->IMR&=~(1<<8);//�ر�line8�ж�	
	ICON_UI_Init();	
//	return 0;	 
}
