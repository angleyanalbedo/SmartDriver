#include "usb_app.h"
#include "button.h"
#include "touch.h"
#include "common.h"
#include "sd_sdio.h"
#include "mass_mal.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "memory.h"	    
#include "usb_bot.h"

//�����ֲUSB��ʱ����bool����TRUE��FLASE��δ���壬������stm32f10x.hͷ�ļ������������͵Ķ���
//��stm32f10x.hͷ�ļ�519�п��Կ���������ӵ����´���
/**
#ifndef __cplusplus			 //���岼������
typedef enum {FALSE = 0, TRUE = !FALSE} bool;
#endif
*/

void USB_APP_Test(void)
{
	_btn_obj* rbtn=0;				//���ذ�ť�ؼ�
	u8 rval=0;
	u8 key; 
	u8 i=0;
	u8 offline_cnt=0;
 	u8 USB_STA=0;
	u8 Divece_STA=0;
	
	FRONT_COLOR=WHITE;  
  	BACK_COLOR=LGRAY;  
	LCD_Clear(BACK_COLOR);		//����
	app_filebrower("USB������Ӧ��",0X05);//��ʾ����
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
	
	FRONT_COLOR=RED;
	if(SD_Init()==0)		//SD������
	{  		
		Mass_Memory_Size[1]=SDCardInfo.CardCapacity;		//�õ�SD���������ֽڣ�����SD����������4G��ʱ��,��Ҫ�õ�����u32����ʾ
		Mass_Block_Size[1] =512;							//��Ϊ������Init����������SD���Ĳ����ֽ�Ϊ512��,��������һ����512���ֽ�.
		Mass_Block_Count[1]=Mass_Memory_Size[1]/Mass_Block_Size[1];
	}	  
	Mass_Memory_Size[0]=1024*1024*12;	//ǰ12M�ֽ�
	Mass_Block_Size[0] =512;			//����SPI FLASH�Ĳ���������СΪ512
	Mass_Block_Count[0]=Mass_Memory_Size[0]/Mass_Block_Size[0];  
	
	//USB����
	delay_ms(1800);
 	USB_Port_Set(0); 	//USB�ȶϿ�
	delay_ms(700);
   	USB_Port_Set(1);	//USB�ٴ�����	   
 	LCD_ShowString(10,90,tftlcd_data.width,tftlcd_data.height,16,"USB Connecting...");//��ʾ���ڽ������� 	    
	printf("USB Connecting...\r\n");
	Data_Buffer=gui_memin_malloc(BULK_MAX_PACKET_SIZE*2*4);	//ΪUSB���ݻ����������ڴ�
	Bulk_Data_Buff=gui_memin_malloc(BULK_MAX_PACKET_SIZE);	//�����ڴ�	
   	//USB����
 	USB_Interrupts_Config();    
 	Set_USBClock();   
 	USB_Init();	    
	delay_ms(1800);
	
	while(rval==0)//��ѭ��
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
		delay_ms(5);
		
		if(USB_STA!=USB_STATUS_REG)//״̬�ı��� 
		{	 						   			  	   
			if(USB_STATUS_REG&0x01)//����д		  
			{
				LCD_ShowString(10,120,tftlcd_data.width,tftlcd_data.height,16,"USB Writing...");//��ʾUSB����д������	 
				printf("USB Writing...\r\n"); 
			}
			if(USB_STATUS_REG&0x02)//���ڶ�
			{
				LCD_ShowString(10,120,tftlcd_data.width,tftlcd_data.height,16,"USB Reading...");//��ʾUSB���ڶ�������  		 
				printf("USB Reading...\r\n"); 		 
			}	 										  
			if(USB_STATUS_REG&0x04)
				LCD_ShowString(10,140,tftlcd_data.width,tftlcd_data.height,16,"USB Write Err ");//��ʾд�����
			else
				gui_fill_rectangle(10,140,tftlcd_data.width,140+16,BACK_COLOR);//�����ʾ	    	  
			if(USB_STATUS_REG&0x08)
				LCD_ShowString(10,140,tftlcd_data.width,tftlcd_data.height,16,"USB Read  Err ");//��ʾ��������
			else 
				gui_fill_rectangle(10,140,tftlcd_data.width,140+16,BACK_COLOR);//�����ʾ      
			USB_STA=USB_STATUS_REG;//��¼����״̬
		}
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==CONFIGURED)
			{
				LCD_ShowString(10,90,tftlcd_data.width,tftlcd_data.height,16,"USB Connected    ");//��ʾUSB�����Ѿ�����	
				printf("USB Connected\r\n");
			}
			else 
			{
				LCD_ShowString(10,90,tftlcd_data.width,tftlcd_data.height,16,"USB DisConnected ");//��ʾUSB���γ���
				printf("USB DisConnected\r\n");
			}
			Divece_STA=bDeviceState;
		}
		i++;
		if(i==200)
		{
			i=0;
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB������,�����offline������
				bDeviceState=CONFIGURED;
			}
			else//û�еõ���ѯ 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=UNCONNECTED;//2s��û�յ����߱��,����USB���γ���
			}
			USB_STATUS_REG=0;
		}
		
		//��ⷵ�ؼ��Ƿ���
		key=btn_check(rbtn,&in_obj);
		if(key&&((rbtn->sta&(1<<7))==0)&&(rbtn->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
		{
			if(USB_STATUS_REG&0x03)//USB���ڶ�д
			{
				FRONT_COLOR=RED;  					    
				LCD_ShowString(10,110,tftlcd_data.width,tftlcd_data.height,16,"USB BUSY!!!");   
			}
			else 
			{
				gui_fill_rectangle(10,110,tftlcd_data.width,110+16,BACK_COLOR);//�����ʾ
				break;//USB����,���˳�USB
			}
		}
	}
	USB_Port_Set(0); 				//USB�ȶϿ�
	gui_memin_free(Data_Buffer);
	gui_memin_free(Bulk_Data_Buff);
	btn_delete(rbtn);
	ICON_UI_Init();
}
