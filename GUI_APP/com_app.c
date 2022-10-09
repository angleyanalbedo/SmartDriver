#include "com_app.h"
#include "button.h"
#include "touch.h"
#include "common.h"
#include "can.h"
#include "rs485.h"
#include "rs232.h"
#include "string.h"
#include "t9input.h"



u8 *COM_BUTTON_CAPTION_TBL[]={"CAN����","RS232����","RS485����"};
#define COM_BUTTON_FONT		16
#define COM_BUTTON_WIDTH	100
#define COM_BUTTON_HEIGHT	40
#define COM_BUTTON_YSPACE	20
#define COM_BUTTON_XSTART	tftlcd_data.width/2-COM_BUTTON_WIDTH/2
#define COM_BUTTON_YSTART	gui_phy.tbheight+COM_BUTTON_YSPACE




void CAN_Test(void)//CANͨ�Ų���
{
	_btn_obj* rbtn=0;				//���ذ�ť�ؼ�
	_btn_obj* modebtn=0;//ģʽ�л�����
	_btn_obj* sendbtn=0;//�������ݰ���
	_edit_obj* tedit_mode;
	_edit_obj* tedit_send;
	_edit_obj* tedit_recv;
	
	u8 rval=0;
	u8 key;
	u8 i=0;
	u8 mode=0;
	u8 res=0;
	u8 tbuf[8];
	u8 rbuf[8];
	u8 tempbuf[9];
	
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);//500Kbps������
		
	FRONT_COLOR=WHITE;  
  	BACK_COLOR=LGRAY;  
	LCD_Clear(BACK_COLOR);		//����
	app_filebrower("CANͨ��Ӧ��",0X05);//��ʾ����
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
	BACK_COLOR=WHITE;
	tedit_mode=edit_creat(10,gui_phy.tbheight+10,10+4*16+10,20,0,0,16);
	if(tedit_mode==NULL)return;//����ʧ��
	tedit_mode->textcolor=RED;
	tedit_mode->text="����ģʽ";
	edit_draw(tedit_mode);
	
	modebtn=btn_creat(10+4*24+30,gui_phy.tbheight+10,50,30,0,0x02);
	if(!modebtn)rval=1;			//û���㹻�ڴ湻����
	else
	{																				
		modebtn->caption="ģʽ"; 
		modebtn->font=16;//�����µ������С	 	 
		modebtn->bcfdcolor=WHITE;	//����ʱ����ɫ
		modebtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
		btn_draw(modebtn);			//�ػ���ť
	}
	
	gui_show_string("��ע�⣺������Ϊ500Kbps",10,gui_phy.tbheight+170,tftlcd_data.width,16,16,GREEN);
	gui_show_string("���ݷ��ͣ�",10,gui_phy.tbheight+50,tftlcd_data.width,16,16,BLUE);
	tedit_send=edit_creat(10+5*16,gui_phy.tbheight+50,10+4*16+10,20,0,0,16);
	if(tedit_send==NULL)return;//����ʧ��
	for(i=0;i<8;i++)//������ת��Ϊ�ַ�
	{
		tbuf[i]=i;
		tempbuf[i]=tbuf[i]+0x30;
	}
	tempbuf[i]='\0';
	tedit_send->text=tempbuf;
	tedit_send->textcolor=RED;
	edit_draw(tedit_send);
	sendbtn=btn_creat(tftlcd_data.width/2-25,gui_phy.tbheight+80,50,30,0,0x02);
	if(!sendbtn)rval=1;			//û���㹻�ڴ湻����
	else
	{																				
		sendbtn->caption="����"; 
		sendbtn->font=16;//�����µ������С	 	 
		sendbtn->bcfdcolor=WHITE;	//����ʱ����ɫ
		sendbtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
		btn_draw(sendbtn);			//�ػ���ť
	}
	gui_show_string("���ݽ��գ�",10,gui_phy.tbheight+120,tftlcd_data.width,16,16,BLUE);
	tedit_recv=edit_creat(10+5*16,gui_phy.tbheight+120,10+4*16+10,20,0,0,16);
	if(tedit_recv==NULL)return;//����ʧ��
	tedit_recv->textcolor=RED;
	edit_draw(tedit_recv);
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
			btn_delete(modebtn);
			btn_delete(sendbtn);
			edit_delete(tedit_mode);
			edit_delete(tedit_send);
			edit_delete(tedit_recv);
			return;
		}
		
		//���ģʽ���Ƿ���
		key=btn_check(modebtn,&in_obj);
		if(key&&((modebtn->sta&(1<<7))==0)&&(modebtn->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
		{
			mode=!mode;
			CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,mode);
			tedit_recv->text="        ";
			edit_draw(tedit_recv);
			if(mode)
			{
				tedit_mode->text="�ػ�ģʽ";
				edit_draw(tedit_mode);
			}
			else
			{
				tedit_mode->text="����ģʽ";
				edit_draw(tedit_mode);
			}
		}
		
		//��ⷢ�ͼ��Ƿ���
		key=btn_check(sendbtn,&in_obj);
		if(key&&((sendbtn->sta&(1<<7))==0)&&(sendbtn->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
		{
			res=CAN_Send_Msg(tbuf,8);
		}
		res=CAN_Receive_Msg(rbuf);
		if(res)
		{
			for(i=0;i<res;i++)
			{
				tempbuf[i]=rbuf[i]+0x30;
			}
			tedit_recv->text=tempbuf;
			edit_draw(tedit_recv);
		}	
	}
}
#define COM_MAX_DATA_LEN	100	
u8 send_buf[]="��ӭʹ�����пƼ�STM32�����壬���ǽ߳�Ϊ������";
void RS485_Test(void)//RS485ͨ�Ų���
{
	_btn_obj* rbtn=0;				//���ذ�ť�ؼ�
	_btn_obj* sendbtn=0;//�������ݰ���
	u8 rval=0;
	u8 key;
	u8 recbuf[64];
	_memo_obj* memo_send;
	_memo_obj* memo_recv;
	_t9_obj * t9=0;		//���뷨
	u16 t9height=0;		//t9���뷨�߶�
	
	RS485_Init(9600);
	
	FRONT_COLOR=WHITE;  
  	BACK_COLOR=LGRAY;  
	LCD_Clear(BACK_COLOR);		//����
	app_filebrower("RS485ͨ��Ӧ��",0X05);//��ʾ����
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
	
	gui_show_string("��ע�⣺������Ϊ9600",10,gui_phy.tbheight+205,tftlcd_data.width,16,16,GREEN);
	gui_show_string("���ݷ��ͣ�",10,gui_phy.tbheight+10,tftlcd_data.width,16,16,BLUE);
	memo_send=memo_creat(10,gui_phy.tbheight+30,tftlcd_data.width-20,50,0,1,16,COM_MAX_DATA_LEN);
	if(memo_send==NULL)return;//����ʧ��
	memo_send->text=send_buf;
	memo_send->textcolor=RED;
	memo_draw_memo(memo_send,0);
	sendbtn=btn_creat(tftlcd_data.width/2-25,gui_phy.tbheight+90,50,30,0,0x02);
	if(!sendbtn)rval=1;			//û���㹻�ڴ湻����
	else
	{																				
		sendbtn->caption="����"; 
		sendbtn->font=16;//�����µ������С	 	 
		sendbtn->bcfdcolor=WHITE;	//����ʱ����ɫ
		sendbtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
		btn_draw(sendbtn);			//�ػ���ť
	}
	gui_show_string("���ݽ��գ�",10,gui_phy.tbheight+130,tftlcd_data.width,16,16,BLUE);
	memo_recv=memo_creat(10,gui_phy.tbheight+150,tftlcd_data.width-20,50,0,0,16,COM_MAX_DATA_LEN);
	if(memo_recv==NULL)return;//����ʧ��
	memo_recv->textcolor=RED;
	memo_draw_memo(memo_recv,0);
	
	if(tftlcd_data.width==800)t9height=548;		//t9���뷨�߶�
	else if(tftlcd_data.width==600)t9height=368;	//t9���뷨�߶�
	else if(tftlcd_data.width==480)t9height=266;	//t9���뷨�߶�
	else if(tftlcd_data.width==320||tftlcd_data.width==272)t9height=176;	//t9���뷨�߶�
	else if(tftlcd_data.width==240)t9height=134; 		
	t9=t9_creat((tftlcd_data.width%5)/2,tftlcd_data.height-t9height-gui_phy.tbheight,tftlcd_data.width-(tftlcd_data.width%5),t9height,0); 
	if(t9==NULL)rval=1;	 
	else t9_draw(t9);
	
	while(rval==0)//��ѭ��
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
		delay_ms(5);
		
		memo_check(memo_send,&in_obj);
		
		t9_check(t9,&in_obj);	
		memo_check(memo_send,&in_obj);
		if(t9->outstr[0]!=NULL)//����ַ�
		{
			//printf("%s\r\n",t9->outstr);
			memo_add_text(memo_send,t9->outstr);
			t9->outstr[0]=NULL;	 			//�������ַ�
		}
		
		//��ⷵ�ؼ��Ƿ���
		key=btn_check(rbtn,&in_obj);
		if(key&&((rbtn->sta&(1<<7))==0)&&(rbtn->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
		{
			USART_Cmd(USART2, DISABLE);
			btn_delete(rbtn);
			btn_delete(sendbtn);
			memo_delete(memo_send);
			memo_delete(memo_recv);
			t9_delete(t9);
			return;
		}
		
		//��ⷢ�ͼ��Ƿ���
		key=btn_check(sendbtn,&in_obj);
		if(key&&((sendbtn->sta&(1<<7))==0)&&(sendbtn->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
		{
			RS485_Send_Data(send_buf,sizeof(send_buf));
		}
		
		RS485_Receive_Data(recbuf,&key);
		if(key)//���յ�����
		{
			memo_recv->text=recbuf;
			memo_draw_memo(memo_recv,0);
		}
	}
}

void RS232_Test(void)//RS232ͨ�Ų���
{
	_btn_obj* rbtn=0;				//���ذ�ť�ؼ�
	_btn_obj* sendbtn=0;//�������ݰ���
	u8 rval=0;
	u8 key;
	_memo_obj* memo_send;
	_memo_obj* memo_recv;
	_t9_obj * t9=0;		//���뷨
	u16 t9height=0;		//t9���뷨�߶�
	
	RS232_Init(9600);
	
	FRONT_COLOR=WHITE;  
  	BACK_COLOR=LGRAY;  
	LCD_Clear(BACK_COLOR);		//����
	app_filebrower("RS232ͨ��Ӧ��",0X05);//��ʾ����
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
	
	gui_show_string("��ע�⣺������Ϊ9600",10,gui_phy.tbheight+205,tftlcd_data.width,16,16,GREEN);
	gui_show_string("���ݷ��ͣ�",10,gui_phy.tbheight+10,tftlcd_data.width,16,16,BLUE);
	memo_send=memo_creat(10,gui_phy.tbheight+30,tftlcd_data.width-20,50,0,1,16,COM_MAX_DATA_LEN);
	if(memo_send==NULL)return;//����ʧ��
	memo_send->text=send_buf;
	memo_send->textcolor=RED;
	memo_draw_memo(memo_send,0);
	
	sendbtn=btn_creat(tftlcd_data.width/2-25,gui_phy.tbheight+90,50,30,0,0x02);
	if(!sendbtn)rval=1;			//û���㹻�ڴ湻����
	else
	{																				
		sendbtn->caption="����"; 
		sendbtn->font=16;//�����µ������С	 	 
		sendbtn->bcfdcolor=WHITE;	//����ʱ����ɫ
		sendbtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
		btn_draw(sendbtn);			//�ػ���ť
	}
	gui_show_string("���ݽ��գ�",10,gui_phy.tbheight+130,tftlcd_data.width,16,16,BLUE);
	memo_recv=memo_creat(10,gui_phy.tbheight+150,tftlcd_data.width-20,50,0,0,16,COM_MAX_DATA_LEN);
	if(memo_recv==NULL)return;//����ʧ��
	memo_recv->textcolor=RED;
	memo_draw_memo(memo_recv,0);
	
	if(tftlcd_data.width==800)t9height=548;		//t9���뷨�߶�
	else if(tftlcd_data.width==600)t9height=368;	//t9���뷨�߶�
	else if(tftlcd_data.width==480)t9height=266;	//t9���뷨�߶�
	else if(tftlcd_data.width==320||tftlcd_data.width==272)t9height=176;	//t9���뷨�߶�
	else if(tftlcd_data.width==240)t9height=134; 		
	t9=t9_creat((tftlcd_data.width%5)/2,tftlcd_data.height-t9height-gui_phy.tbheight,tftlcd_data.width-(tftlcd_data.width%5),t9height,0); 
	if(t9==NULL)rval=1;	 
	else t9_draw(t9);
	
	while(rval==0)//��ѭ��
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
		delay_ms(5);
		
		t9_check(t9,&in_obj);	
		memo_check(memo_send,&in_obj);
		if(t9->outstr[0]!=NULL)//����ַ�
		{
			//printf("%s\r\n",t9->outstr);
			memo_add_text(memo_send,t9->outstr);
			t9->outstr[0]=NULL;	 			//�������ַ�
		}
		
		//��ⷵ�ؼ��Ƿ���
		key=btn_check(rbtn,&in_obj);
		if(key&&((rbtn->sta&(1<<7))==0)&&(rbtn->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
		{
			USART_Cmd(USART2, DISABLE);
			btn_delete(rbtn);
			btn_delete(sendbtn);
			memo_delete(memo_send);
			memo_delete(memo_recv);
			t9_delete(t9);
			return;
		}
		
		//��ⷢ�ͼ��Ƿ���
		key=btn_check(sendbtn,&in_obj);
		if(key&&((sendbtn->sta&(1<<7))==0)&&(sendbtn->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
		{
			RS232_SendString(memo_send->text);
		}
		
		if(RS232_RX_STA&0x8000)//���յ�����
		{
			RS232_RX_BUF[RS232_RX_STA&0x3fff]='\0';
			memo_recv->text=RS232_RX_BUF;
			memo_draw_memo(memo_recv,0);
			RS232_RX_STA=0;
		}
	}
}

void COM_APP_Test(void)
{
	_btn_obj* rbtn=0;				//���ذ�ť�ؼ�
	_btn_obj* ctbtn[3];//CAN/RS232/RS485ͨ�ſ��ư�ť
	u8 rval=0;
	u8 key;
	u8 i=0;
	
	
	FRONT_COLOR=WHITE;  
  	BACK_COLOR=LGRAY;  
	LCD_Clear(BACK_COLOR);		//����
	app_filebrower("ͨ��Ӧ��",0X05);//��ʾ����
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
	
	//����COM���ư�ť
	for(i=0;i<3;i++)
	{
		ctbtn[i]=btn_creat(COM_BUTTON_XSTART,COM_BUTTON_YSTART+(COM_BUTTON_HEIGHT+COM_BUTTON_YSPACE)*i,COM_BUTTON_WIDTH,COM_BUTTON_HEIGHT,0,0x02);//������ť
		if(ctbtn[i]==NULL)rval=1;	//û���㹻�ڴ湻����
		else
		{																				
			ctbtn[i]->caption=COM_BUTTON_CAPTION_TBL[i];//���� 
			ctbtn[i]->font=COM_BUTTON_FONT;//�����µ������С	 	 
			ctbtn[i]->bcfdcolor=WHITE;	//����ʱ����ɫ
			ctbtn[i]->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
			btn_draw(ctbtn[i]);			//�ػ���ť
		}
	}
	//�ȴ�ĳ�����ư�������
	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
		
		//��ⷵ�ؼ��Ƿ���
		key=btn_check(rbtn,&in_obj);
		if(key&&((rbtn->sta&(1<<7))==0)&&(rbtn->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
		{
			btn_delete(rbtn);
			ICON_UI_Init();
			return;
		}
		
		for(i=0;i<3;i++)
		{
			key=btn_check(ctbtn[i],&in_obj);
			if(key&&((ctbtn[i]->sta&(1<<7))==0)&&(ctbtn[i]->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
			{
				switch(i)
				{
					case 0: CAN_Test();
							btn_delete(rbtn);
							for(i=0;i<3;i++)
								btn_delete(ctbtn[i]);
							ICON_UI_Init();
							return;
					case 1: RS232_Test();
							btn_delete(rbtn);
							for(i=0;i<3;i++)
								btn_delete(ctbtn[i]);
							ICON_UI_Init();
							return;
					case 2: RS485_Test();
							btn_delete(rbtn);
							for(i=0;i<3;i++)
								btn_delete(ctbtn[i]);
							ICON_UI_Init();
							return;
				}
			}
		}
	}
}
