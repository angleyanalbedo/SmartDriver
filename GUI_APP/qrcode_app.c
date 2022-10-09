#include "qrcode_app.h"
#include "button.h"
#include "touch.h"
#include "common.h"
#include "qrcode.h"
#include "ws2812.h"


#define FLOWLED_PORT			GPIOF
#define FLOWLED_PIN 			GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3 \
								|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7
#define FLOWLED_PORT_RCC		RCC_APB2Periph_GPIOF

u8 flowled_buf1[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//�������ҹ���
u8 flowled_buf2[]={0x7e,0xbd,0xdb,0xe7,0xe7,0xdb,0xbd,0x7e};//˫�ƾ�£��ɢ

void FlowLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(FLOWLED_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=FLOWLED_PIN;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  
	GPIO_Init(FLOWLED_PORT,&GPIO_InitStructure); 	   
	GPIO_ResetBits(FLOWLED_PORT,FLOWLED_PIN);   
}

void TIM6_Init(u16 per,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	
	TIM_TimeBaseInitStructure.TIM_Period=per;   //�Զ�װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //�������ϼ���ģʽ
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); //������ʱ���ж�
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;//��ʱ���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	
	
	TIM_Cmd(TIM6,ENABLE); //ʹ�ܶ�ʱ��	
}

void TIM6_IRQHandler(void)
{
	static u8 cnt=0;
	static u8 leddir=0;
	static u8 ledcnt=0;
	static u8 ledmode=0;
	
	if(TIM_GetITStatus(TIM6,TIM_IT_Update))
	{
		cnt++;
		if(cnt==5)
		{
			cnt=0;
			if(leddir==0)
			{
				if(ledmode==0)
				{
					GPIO_Write(FLOWLED_PORT,flowled_buf1[ledcnt]);
					ledcnt++;
					if(ledcnt==8)leddir=1;
				}
				else
				{
					GPIO_Write(FLOWLED_PORT,flowled_buf2[ledcnt]);
					ledcnt++;
					if(ledcnt==4)leddir=1;
				}
			}
			else
			{
				if(ledmode==0)
				{
					ledcnt--;
					GPIO_Write(FLOWLED_PORT,flowled_buf1[ledcnt]);
					if(ledcnt==0)
					{
						leddir=0;
						ledmode=1;
					}
				}
				else
				{
					ledcnt--;
					GPIO_Write(FLOWLED_PORT,flowled_buf2[ledcnt]);
					if(ledcnt==0)
					{
						leddir=0;
						ledmode=0;
					}
				}
			}
		}
	}
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);	
}

#define DCMOTOR_PORT			GPIOE
#define DCMOTOR_PIN 			GPIO_Pin_6
#define DCMOTOR_PORT_RCC		RCC_APB2Periph_GPIOE
#define DCMOTOR PEout(6)
void DCMotor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(DCMOTOR_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=DCMOTOR_PIN;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  
	GPIO_Init(DCMOTOR_PORT,&GPIO_InitStructure); 	   
	GPIO_ResetBits(DCMOTOR_PORT,DCMOTOR_PIN);   
}

void RGB_ShowDemo(void)
{
	while(1)
	{
		RGB_LED_Red();
		delay_ms(1000);
		RGB_LED_Green();
		delay_ms(1000);
		RGB_LED_Blue();
		delay_ms(1000);
		break;
	}
	RGB_LED_Clear();
}

void Qrcode_APP_Test(void)
{
	_btn_obj* rbtn=0;				//���ذ�ť�ؼ�
	u8 rval=0;
	u8 key; 
	u8 num=0;
	u8 i=0;
	u32 color[]={RGB_COLOR_RED,RGB_COLOR_GREEN,RGB_COLOR_BLUE,RGB_COLOR_WHITE,RGB_COLOR_YELLOW};
	
	
	
	RGB_LED_Init();
	FlowLED_Init();
	TIM6_Init(200,7199);//20ms
	DCMotor_Init();
	FRONT_COLOR=WHITE;  
  	BACK_COLOR=LGRAY;  
	LCD_Clear(BACK_COLOR);		//����
	app_filebrower("RGB�ʵ�Ӧ��",0X05);//��ʾ����
	app_gui_tcbar(0,tftlcd_data.height-gui_phy.tbheight,tftlcd_data.width,gui_phy.tbheight,0x01);	//�Ϸֽ���
	rbtn=btn_creat(tftlcd_data.width-2*gui_phy.tbfsize-8-1,tftlcd_data.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//�������ְ�ť
	if(!rbtn)rval=1;			//û���㹻�ڴ湻����
	else
	{																				
		rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//���� 
		rbtn->font=gui_phy.tbfsize;//�����µ������С	 	 
		rbtn->bcfdcolor=WHITE;	//����ʱ����ɫ
		rbtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
		btn_draw(rbtn);			//�ػ���ť
	}
	gui_show_string("RGB�ʵƲ���...",10,50,tftlcd_data.width,32,16,BLUE);
	gui_show_string("LED����Ʋ���...",10,70,tftlcd_data.width,32,16,BLUE);
	gui_show_string("ֱ���������...",10,90,tftlcd_data.width,32,16,BLUE);
	RGB_ShowDemo();
	while(rval==0)//��ѭ��
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
		delay_ms(5);
		DCMOTOR=!DCMOTOR;
		i++;
		if(i==100)
		{
			i=0;
			RGB_ShowCharNum(num%16,color[num%5]);
			num++;
		}
		
		//��ⷵ�ؼ��Ƿ���
		key=btn_check(rbtn,&in_obj);
		if(key&&((rbtn->sta&(1<<7))==0)&&(rbtn->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
		{
			btn_delete(rbtn);
			RGB_LED_Clear();
			TIM_Cmd(TIM6,DISABLE);
			GPIO_Write(FLOWLED_PORT,0xff);
			DCMOTOR=0;
			ICON_UI_Init();
			return;
		}
	}
}
