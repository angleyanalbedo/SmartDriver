#include "rtc_app.h"
#include "button.h"
#include "touch.h"
#include "common.h"
#include "math.h"
#include "rtc.h" 
#include "ds18b20.h"
#include "adc_temp.h"
#include "font_show.h"


//��ֵ����
#define	app_pi	3.1415926535897932384626433832795

#define CALENDAR_DIAMETER			160
#define CALENDAR_CENTER_X			tftlcd_data.width/2
#define CALENDAR_CENTER_Y			tftlcd_data.height/2-CALENDAR_DIAMETER/2+gui_phy.tbheight+5
#define CALENDAR_SCALE				10
#define CALENDAR_BACKCOLOR			BACK_COLOR
#define CALENDAR_SECPOINTCOLOR		RED
#define CALENDAR_MINPOINTCOLOR		GREEN
#define CALENDAR_HOURPOINTCOLOR		YELLOW
#define CALENDAR_SCALECOLOR			YELLOW
#define CALENDAR_CIRCLECOLOR		BLUE
#define CALENDAR_NUMCOLOR			RED

u8 rome[]={12,1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 } ; //��������

//��Բ��ָ�����
//x,y:�������ĵ�
//size:���̴�С(ֱ��)
//d:���̷ָ�,���ӵĸ߶�
void calendar_circle_clock_drawpanel(u16 x,u16 y,u16 size,u16 d)
{
	u16 r=size/2;//�õ��뾶 
	u16 sx=x-r;
	u16 sy=y-r;
	u16 px0,px1;
	u16 py0,py1; 
	u16 i;
	u16 fcolor;

	fcolor=FRONT_COLOR;
	FRONT_COLOR=CALENDAR_CIRCLECOLOR;
	LCD_Draw_Circle(x,y,r);		//����Ȧ
	LCD_Draw_Circle(x,y,r-1);		//����Ȧ

	for(i=0;i<60;i++)//�����Ӹ�
	{ 
		px0=sx+r+(r-4)*sin((app_pi/30)*i); 
		py0=sy+r-(r-4)*cos((app_pi/30)*i); 
		px1=sx+r+(r-d)*sin((app_pi/30)*i); 
		py1=sy+r-(r-d)*cos((app_pi/30)*i);  
		gui_draw_bline1(px0,py0,px1,py1,0,CALENDAR_SCALECOLOR);		
	}
	for(i=0;i<12;i++)//��Сʱ��
	{ 
		px0=sx+r+(r-5)*sin((app_pi/6)*i); 
		py0=sy+r-(r-5)*cos((app_pi/6)*i); 
		px1=sx+r+(r-d)*sin((app_pi/6)*i); 
		py1=sy+r-(r-d)*cos((app_pi/6)*i);  
		gui_draw_bline1(px0,py0,px1,py1,1,CALENDAR_SCALECOLOR);		
	}
	for(i=0;i<4;i++)//��3Сʱ��
	{ 
		px0=sx+r+(r-5)*sin((app_pi/2)*i); 
		py0=sy+r-(r-5)*cos((app_pi/2)*i); 
		px1=sx+r+(r-d-3)*sin((app_pi/2)*i); 
		py1=sy+r-(r-d-3)*cos((app_pi/2)*i);  
		gui_draw_bline1(px0,py0,px1,py1,2,CALENDAR_SCALECOLOR);		
	}
	for(i=0;i<60;i++)//��ʾ����
	{  
		px1=sx+r+(r-d-10)*sin((app_pi/30)*i); 
		py1=sy+r-(r-d-10)*cos((app_pi/30)*i);  
		if(i%5==0)
		{
			FRONT_COLOR=CALENDAR_NUMCOLOR;
			LCD_ShowxNum(px1-8,py1-5,rome[i/5],2,12,1);	
		}		
	}
	gui_fill_circle(x,y,d/2,CALENDAR_CIRCLECOLOR);		//������Ȧ
	FRONT_COLOR=fcolor;
}

//��ʾʱ��
//x,y:�������ĵ�
//size:���̴�С(ֱ��)
//d:���̷ָ�,���ӵĸ߶�
//hour:ʱ��
//min:����
//sec:����
void calendar_circle_clock_showtime(u16 x,u16 y,u16 size,u16 d,u8 hour,u8 min,u8 sec)
{
	static u8 oldhour=0;	//���һ�ν���ú�����ʱ������Ϣ
	static u8 oldmin=0;
	static u8 oldsec=0;
	float temp;
	u16 r=size/2;//�õ��뾶 
	u16 sx=x-r;
	u16 sy=y-r;
	u16 px0,px1;
	u16 py0,py1;  
	u8 r1; 
	if(hour>11)hour-=12;
/////////////////////////////////////////////
	//������� 
	r1=d/2+3;  //3
	//�����һ�ε�����
	px0=sx+r+(r-2*d-7)*sin((app_pi/30)*oldsec); 
	py0=sy+r-(r-2*d-7)*cos((app_pi/30)*oldsec); 
	px1=sx+r+r1*sin((app_pi/30)*oldsec); 
	py1=sy+r-r1*cos((app_pi/30)*oldsec); 
	gui_draw_bline1(px0,py0,px1,py1,0,CALENDAR_BACKCOLOR);	

	//�������
	r1=d/2+3;	//3
	temp=(float)oldsec/60;
	temp+=oldmin;
	//�����һ�ε�����
	px0=sx+r+(r-3*d-7)*sin((app_pi/30)*temp); 
	py0=sy+r-(r-3*d-7)*cos((app_pi/30)*temp); 
	px1=sx+r+r1*sin((app_pi/30)*temp); 
	py1=sy+r-r1*cos((app_pi/30)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,1,CALENDAR_BACKCOLOR);		

	//���Сʱ
	r1=d/2+4;
	//�����һ�ε�����
	temp=(float)oldmin/60;
	temp+=oldhour;
	px0=sx+r+(r-4*d-7)*sin((app_pi/6)*temp); 
	py0=sy+r-(r-4*d-7)*cos((app_pi/6)*temp); 
	px1=sx+r+r1*sin((app_pi/6)*temp); 
	py1=sy+r-r1*cos((app_pi/6)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,2,CALENDAR_BACKCOLOR);	
		
	
///////////////////////////////////////////////
	//��ʾ���� 
	r1=d/2+3;
	//��ʾ�µ�����
	px0=sx+r+(r-2*d-7)*sin((app_pi/30)*sec); 
	py0=sy+r-(r-2*d-7)*cos((app_pi/30)*sec); 
	px1=sx+r+r1*sin((app_pi/30)*sec); 
	py1=sy+r-r1*cos((app_pi/30)*sec); 
	gui_draw_bline1(px0,py0,px1,py1,0,CALENDAR_SECPOINTCOLOR);
	
	
	//��ʾ����
	r1=d/2+3; 
	temp=(float)sec/60;
	temp+=min;
	//��ʾ�µķ���
	px0=sx+r+(r-3*d-7)*sin((app_pi/30)*temp); 
	py0=sy+r-(r-3*d-7)*cos((app_pi/30)*temp); 
	px1=sx+r+r1*sin((app_pi/30)*temp); 
	py1=sy+r-r1*cos((app_pi/30)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,1,CALENDAR_MINPOINTCOLOR);		

	//��ʾСʱ 
	r1=d/2+4; 
	//��ʾ�µ�ʱ��
	temp=(float)min/60;
	temp+=hour;
	px0=sx+r+(r-4*d-7)*sin((app_pi/6)*temp); 
	py0=sy+r-(r-4*d-7)*cos((app_pi/6)*temp); 
	px1=sx+r+r1*sin((app_pi/6)*temp); 
	py1=sy+r-r1*cos((app_pi/6)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,2,CALENDAR_HOURPOINTCOLOR);	
	 
	oldhour=hour;	//����ʱ
	oldmin=min;		//�����
	oldsec=sec;		//������
}

void calendar_circle_clock_shownum(u16 x,u16 y,u16 size,u16 d)
{
	u16 r=size/2;//�õ��뾶 
	u16 sx=x-r;
	u16 sy=y-r;
	u16 px1,py1; 
	u8 i;
	u16	fcolor;
	fcolor=FRONT_COLOR;
	for(i=0;i<60;i++)//��ʾ����
	{  
		px1=sx+r+(r-d-10)*sin((app_pi/30)*i); 
		py1=sy+r-(r-d-10)*cos((app_pi/30)*i);  
		if(i%5==0)
		{
			FRONT_COLOR=CALENDAR_NUMCOLOR;
			LCD_ShowxNum(px1-8,py1-5,rome[i/5],2,12,1);	
		}		
	}
	FRONT_COLOR=fcolor;	
}

u8*const calendar_week_table[7]=
{
"������","����һ","���ڶ�","������","������","������","������"
};

void RTC_APP_Test(void)
{
	_btn_obj* rbtn=0;//���ذ�ť�ؼ�
	u8 rval=0;
	u8 key; 
	u8 rtcbuf[9];
	u8 temp_type=0;//0��DS18B20��1���ڲ��¶ȴ�����
	u8 temp_buf[6];
	float temp;
	u8 tempsec=0;
	
	ADC_Temp_Init();
	
	if(DS18B20_Init())
	{
		temp_type=1;
		temp=Get_Temperture()/100;
		if(temp<0)temp=-temp;
	}
	else
	{
		temp_type=0;
		temp=DS18B20_GetTemperture();
	}
	
	FRONT_COLOR=WHITE;  
  	BACK_COLOR=LGRAY;  
	LCD_Clear(BACK_COLOR);		//����
	app_filebrower("RTCӦ��",0X05);//��ʾ����
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
	tempsec=calendar.sec;
	calendar_circle_clock_drawpanel(CALENDAR_CENTER_X,CALENDAR_CENTER_Y,CALENDAR_DIAMETER,CALENDAR_SCALE);
	FRONT_COLOR=MAGENTA;
	LCD_ShowFontString(tftlcd_data.width/2-(3*24),tftlcd_data.height/2+40+16+40,tftlcd_data.width,16,"�¶�:     ��C",16,1);
	FRONT_COLOR=RED;
	while(rval==0)//��ѭ��
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
		delay_ms(5);
		
		//��ⷵ�ؼ��Ƿ���
		key=btn_check(rbtn,&in_obj);
		if(key&&((rbtn->sta&(1<<7))==0)&&(rbtn->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
		{
			btn_delete(rbtn);
			ICON_UI_Init();
			break;
		}
		
		if(calendar.sec!=tempsec)
		{
			tempsec=calendar.sec;
			calendar_circle_clock_showtime(CALENDAR_CENTER_X,CALENDAR_CENTER_Y,CALENDAR_DIAMETER,CALENDAR_SCALE,calendar.hour,calendar.min,calendar.sec);			
			
			sprintf((char *)rtcbuf,"%d-%.2d-%.2d",calendar.w_year,calendar.w_month,calendar.w_date);
			LCD_ShowString(tftlcd_data.width/2-(5*8),tftlcd_data.height/2+40,tftlcd_data.width,16,16,rtcbuf);			
			sprintf((char *)rtcbuf,"%.2d:%.2d:%.2d",calendar.hour,calendar.min,calendar.sec);
			LCD_ShowString(tftlcd_data.width/2-(4*8),tftlcd_data.height/2+40+16,tftlcd_data.width,16,16,rtcbuf);
			FRONT_COLOR=BLUE;
			LCD_ShowFontString(tftlcd_data.width/2-(1*24),tftlcd_data.height/2+40+35,tftlcd_data.width,16,calendar_week_table[calendar.week],16,0);
		}
		if(temp_type)
		{
			temp=Get_Temperture()/100;
			if(temp<0)temp=-temp;
		}
		else
		{
			temp=DS18B20_GetTemperture();
		}
		FRONT_COLOR=RED;
		sprintf((char *)temp_buf,"%.1f",temp);
		temp_buf[5]='\0';
		LCD_ShowFontString(tftlcd_data.width/2-(3*24)+24*2+12,tftlcd_data.height/2+40+16+40,tftlcd_data.width,16,temp_buf,16,0);
	}
	
}
