#include "led_app.h"
#include "button.h"
#include "touch.h"
#include "common.h"
#include "led.h"
#include "hwjs.h"
#include "flash.h"
#include "lsens.h"


_led_ctrl led_ctrl;

u8 *LED_BUTTON_CAPTION_TBL[]={"LED1","LED2","Start"};
#define LED_BUTTON_FONT		16
#define LED_BUTTON_XSTART	10
#define LED_BUTTON_YSTART	80
#define LED_BUTTON_WIDTH	50
#define LED_BUTTON_HEIGHT	40
#define LED_BUTTON_XSPACE	20



//LED_APP������ʼ��
void led_param_init(void)
{
	led_ctrl.x=13;
	led_ctrl.y=gui_phy.tbheight+20;
	led_ctrl.r=10;
	led_ctrl.fcolor=FRONT_COLOR;
	led_ctrl.bcolor=BACK_COLOR;
	led_ctrl.xspace=10;
}

void LED_APP_Test(void)
{
	_btn_obj* rbtn=0;				//���ذ�ť�ؼ�
	_btn_obj* ctbtn[3];//LED���ư�ť
	u8 rval=0;
	u8 key; 
	u8 *txtbuf[]={"led1","led2"};
	u8 i=0,j=0;
	u8 t=0;
	u8 led_start=0;
	u8 led1_flag=0;
	u8 led2_flag=0;
	u8 dat[4],irdisp[9];
	u8 num[16]="0123456789ABCDEF";
	u8 lsens_temp=0;
	u16 adc_temp=0;
	u8 lsens_buf[5];
	u8 cnt=0;
	_edit_obj* tedit_ired;
	_edit_obj* tedit_lsens;
	_edit_obj* tedit_adc;
	
	Hwjs_Init();
	
	FRONT_COLOR=WHITE;  
  	BACK_COLOR=LGRAY;  
	LCD_Clear(BACK_COLOR);		//����
	app_filebrower("LEDӦ��",0X05);//��ʾ����
	app_gui_tcbar(0,tftlcd_data.height-gui_phy.tbheight,tftlcd_data.width,gui_phy.tbheight,0x01);	//�Ϸֽ���
	rbtn=btn_creat(tftlcd_data.width-2*gui_phy.tbfsize-8-1,tftlcd_data.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//�������ְ�ť
	if(rbtn==NULL)rval=1;	//û���㹻�ڴ湻����
	else
	{																				
		rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//���� 
		rbtn->font=gui_phy.tbfsize;//�����µ������С	 	 
		rbtn->bcfdcolor=WHITE;	//����ʱ����ɫ
		rbtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
		btn_draw(rbtn);			//�ػ���ť
	}
	//����LEDͼ��
	led_param_init();
	for(i=0;i<2;i++)
	{
		gui_fill_circle(led_ctrl.x+(led_ctrl.r*2+led_ctrl.xspace)*i,led_ctrl.y,led_ctrl.r,led_ctrl.fcolor);
		gui_show_strmid(led_ctrl.x-led_ctrl.r+(led_ctrl.r*2+led_ctrl.xspace)*i,led_ctrl.y+led_ctrl.r,led_ctrl.r*2+5,12,RED,12,txtbuf[i]);
	}
	//����LED���ư�ť
	for(i=0;i<3;i++)
	{
		ctbtn[i]=btn_creat(LED_BUTTON_XSTART+(LED_BUTTON_WIDTH+LED_BUTTON_XSPACE)*i,LED_BUTTON_YSTART,LED_BUTTON_WIDTH,LED_BUTTON_HEIGHT,0,0x02);//������ť
		if(ctbtn[i]==NULL)rval=1;	//û���㹻�ڴ湻����
		else
		{																				
			ctbtn[i]->caption=LED_BUTTON_CAPTION_TBL[i];//���� 
			ctbtn[i]->font=LED_BUTTON_FONT;//�����µ������С	 	 
			ctbtn[i]->bcfdcolor=WHITE;	//����ʱ����ɫ
			ctbtn[i]->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
			btn_draw(ctbtn[i]);			//�ػ���ť
		}
	}
	
	gui_show_string("����������ݣ�",10,150,tftlcd_data.width,16,16,BLUE);
	tedit_ired=edit_creat(10+7*16,150,10+4*16+10,20,0,0,16);
	if(tedit_ired==NULL)return;//����ʧ��
	tedit_ired->textcolor=RED;
	edit_draw(tedit_ired);
	
	gui_show_string("    ����ǿ�ȣ�",10,180,tftlcd_data.width,16,16,BLUE);
	tedit_lsens=edit_creat(10+7*16,180,10+4*16+10,20,0,0,16);
	if(tedit_lsens==NULL)return;//����ʧ��
	tedit_lsens->textcolor=RED;
	edit_draw(tedit_lsens);
	
	gui_show_string(" ��λ��ADCֵ��",10,210,tftlcd_data.width,16,16,BLUE);
	tedit_adc=edit_creat(10+7*16,210,10+4*16+10,20,0,0,16);
	if(tedit_adc==NULL)return;//����ʧ��
	tedit_adc->textcolor=RED;
	edit_draw(tedit_adc);
	
	i=0;
	while(rval==0)//��ѭ��
	{
		cnt++;
		if(cnt%100==0)
		{
			lsens_temp=Lsens_Get_Val();
			lsens_buf[0]=num[lsens_temp/100];
			lsens_buf[1]=num[lsens_temp%100/10];
			lsens_buf[2]=num[lsens_temp%100%10];
			lsens_buf[3]='\0';
			strcpy((char*)tedit_lsens->text,(const char *)lsens_buf);
			edit_draw(tedit_lsens);
			
			adc_temp=Get_ADC_Value(ADC_Channel_1,10);
			lsens_buf[0]=num[adc_temp/1000];
			lsens_buf[1]=num[adc_temp%1000/100];
			lsens_buf[2]=num[adc_temp%1000%100/10];
			lsens_buf[3]=num[adc_temp%1000%100%10];
			lsens_buf[4]='\0';
			strcpy((char*)tedit_adc->text,(const char *)lsens_buf);
			edit_draw(tedit_adc);
		}
		
		if(hw_jsbz)//���������յ�
		{
			hw_jsbz=0;//����
			dat[0]=hw_jsm>>24;
			dat[1]=(hw_jsm>>16)&0xff;
			dat[2]=(hw_jsm>>8)&0xff;
			dat[3]=hw_jsm&0xff;
			hw_jsm=0;//����������
			irdisp[0]=num[dat[0]/16];//��ʾ��λ
			irdisp[1]=num[dat[0]%16];//��ʾ��λ  
			irdisp[2]=num[dat[1]/16];
			irdisp[3]=num[dat[1]%16];	 //ͬһ��ң������2���ֽڵ������������ǲ���ı�ģ��ı��ֻ�������뼰����
			irdisp[4]=num[dat[2]/16];
			irdisp[5]=num[dat[2]%16];
			irdisp[6]=num[dat[3]/16];
			irdisp[7]=num[dat[3]%16];
			irdisp[8]='\0';
			strcpy((char*)tedit_ired->text,(const char *)irdisp);
			edit_draw(tedit_ired);
		}
		
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
		delay_ms(5);
		
		//��ⷵ�ؼ��Ƿ��� //�ͷ���Դ
		key=btn_check(rbtn,&in_obj);
		if(key&&((rbtn->sta&(1<<7))==0)&&(rbtn->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
		{
			LED1=1;
			LED2=1;
			btn_delete(rbtn);
			for(i=0;i<3;i++)
			{
				btn_delete(ctbtn[i]);
			}
			edit_delete(tedit_ired);
			edit_delete(tedit_lsens);
			edit_delete(tedit_adc);
			ICON_UI_Init();
			break;
		}
		//���LED���Ƽ��Ƿ���
		for(j=0;j<3;j++)
		{
			key=btn_check(ctbtn[j],&in_obj);
			if(key&&((ctbtn[j]->sta&(1<<7))==0)&&(ctbtn[j]->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
			{
				switch(j)
				{
					case 0: LED1=!LED1;led1_flag=!led1_flag;break;
					case 1: LED2=!LED2;led2_flag=!led2_flag;break;
					case 2: led_start=!led_start;break;
				}
			}
		}
		//LED1״ָ̬ʾ
		if(led1_flag)
		{
			if(led_start==0)
			{
				led_ctrl.fcolor=RED;
				gui_fill_circle(led_ctrl.x+(led_ctrl.r*2+led_ctrl.xspace)*0,led_ctrl.y,led_ctrl.r,led_ctrl.fcolor);
			}
		}
		else
		{
			if(led_start==0)
			{
				led_ctrl.fcolor=WHITE;
				gui_fill_circle(led_ctrl.x+(led_ctrl.r*2+led_ctrl.xspace)*0,led_ctrl.y,led_ctrl.r,led_ctrl.fcolor);
			}
		}
		//LED2״ָ̬ʾ
		if(led2_flag)
		{
			if(led_start==0)
			{
				led_ctrl.fcolor=RED;
				gui_fill_circle(led_ctrl.x+(led_ctrl.r*2+led_ctrl.xspace)*1,led_ctrl.y,led_ctrl.r,led_ctrl.fcolor);
			}
		}
		else
		{
			if(led_start==0)
			{
				led_ctrl.fcolor=WHITE;
				gui_fill_circle(led_ctrl.x+(led_ctrl.r*2+led_ctrl.xspace)*1,led_ctrl.y,led_ctrl.r,led_ctrl.fcolor);
			}
		}
		
		if(led_start)
		{
			if(led1_flag==0&&led2_flag==0)
			{
				i++;
				if(i==80)//LED1��
				{
					led_ctrl.fcolor=RED;
					gui_fill_circle(led_ctrl.x+(led_ctrl.r*2+led_ctrl.xspace)*0,led_ctrl.y,led_ctrl.r,led_ctrl.fcolor);
					led_ctrl.fcolor=WHITE;
					gui_fill_circle(led_ctrl.x+(led_ctrl.r*2+led_ctrl.xspace)*1,led_ctrl.y,led_ctrl.r,led_ctrl.fcolor);
					LED1=0;
					LED2=1;
				}
				if(i==160)//LED2��
				{
					i=0;
					led_ctrl.fcolor=WHITE;
					gui_fill_circle(led_ctrl.x+(led_ctrl.r*2+led_ctrl.xspace)*0,led_ctrl.y,led_ctrl.r,led_ctrl.fcolor);
					led_ctrl.fcolor=RED;
					gui_fill_circle(led_ctrl.x+(led_ctrl.r*2+led_ctrl.xspace)*1,led_ctrl.y,led_ctrl.r,led_ctrl.fcolor);
					LED1=1;
					LED2=0;
				}
				
			}
		}
		else
		{
			if(led1_flag==0&&led2_flag==0)
			{
				i=0;
				LED1=1;
				LED2=1;
			}
		}
		
	}
}

