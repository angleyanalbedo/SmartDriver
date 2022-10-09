#include "com_app.h"
#include "button.h"
#include "touch.h"
#include "common.h"
#include "can.h"
#include "rs485.h"
#include "rs232.h"
#include "string.h"
#include "t9input.h"



u8 *COM_BUTTON_CAPTION_TBL[]={"CAN测试","RS232测试","RS485测试"};
#define COM_BUTTON_FONT		16
#define COM_BUTTON_WIDTH	100
#define COM_BUTTON_HEIGHT	40
#define COM_BUTTON_YSPACE	20
#define COM_BUTTON_XSTART	tftlcd_data.width/2-COM_BUTTON_WIDTH/2
#define COM_BUTTON_YSTART	gui_phy.tbheight+COM_BUTTON_YSPACE




void CAN_Test(void)//CAN通信测试
{
	_btn_obj* rbtn=0;				//返回按钮控件
	_btn_obj* modebtn=0;//模式切换按键
	_btn_obj* sendbtn=0;//发送数据按键
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
	
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);//500Kbps波特率
		
	FRONT_COLOR=WHITE;  
  	BACK_COLOR=LGRAY;  
	LCD_Clear(BACK_COLOR);		//清屏
	app_filebrower("CAN通信应用",0X05);//显示标题
	app_gui_tcbar(0,tftlcd_data.height-gui_phy.tbheight,tftlcd_data.width,gui_phy.tbheight,0x01);	//上分界线
	rbtn=btn_creat(tftlcd_data.width-2*gui_phy.tbfsize-8-1,tftlcd_data.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮
	if(!rbtn)rval=1;			//没有足够内存够分配
	else
	{																				
		rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//返回 
		rbtn->font=gui_phy.tbfsize;//设置新的字体大小	 	 
		rbtn->bcfdcolor=WHITE;	//按下时的颜色
		rbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(rbtn);			//重画按钮
	}
	BACK_COLOR=WHITE;
	tedit_mode=edit_creat(10,gui_phy.tbheight+10,10+4*16+10,20,0,0,16);
	if(tedit_mode==NULL)return;//创建失败
	tedit_mode->textcolor=RED;
	tedit_mode->text="正常模式";
	edit_draw(tedit_mode);
	
	modebtn=btn_creat(10+4*24+30,gui_phy.tbheight+10,50,30,0,0x02);
	if(!modebtn)rval=1;			//没有足够内存够分配
	else
	{																				
		modebtn->caption="模式"; 
		modebtn->font=16;//设置新的字体大小	 	 
		modebtn->bcfdcolor=WHITE;	//按下时的颜色
		modebtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(modebtn);			//重画按钮
	}
	
	gui_show_string("请注意：波特率为500Kbps",10,gui_phy.tbheight+170,tftlcd_data.width,16,16,GREEN);
	gui_show_string("数据发送：",10,gui_phy.tbheight+50,tftlcd_data.width,16,16,BLUE);
	tedit_send=edit_creat(10+5*16,gui_phy.tbheight+50,10+4*16+10,20,0,0,16);
	if(tedit_send==NULL)return;//创建失败
	for(i=0;i<8;i++)//将数据转换为字符
	{
		tbuf[i]=i;
		tempbuf[i]=tbuf[i]+0x30;
	}
	tempbuf[i]='\0';
	tedit_send->text=tempbuf;
	tedit_send->textcolor=RED;
	edit_draw(tedit_send);
	sendbtn=btn_creat(tftlcd_data.width/2-25,gui_phy.tbheight+80,50,30,0,0x02);
	if(!sendbtn)rval=1;			//没有足够内存够分配
	else
	{																				
		sendbtn->caption="发送"; 
		sendbtn->font=16;//设置新的字体大小	 	 
		sendbtn->bcfdcolor=WHITE;	//按下时的颜色
		sendbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(sendbtn);			//重画按钮
	}
	gui_show_string("数据接收：",10,gui_phy.tbheight+120,tftlcd_data.width,16,16,BLUE);
	tedit_recv=edit_creat(10+5*16,gui_phy.tbheight+120,10+4*16+10,20,0,0,16);
	if(tedit_recv==NULL)return;//创建失败
	tedit_recv->textcolor=RED;
	edit_draw(tedit_recv);
	while(rval==0)//主循环
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		delay_ms(5);
		
		//检测返回键是否按下
		key=btn_check(rbtn,&in_obj);
		if(key&&((rbtn->sta&(1<<7))==0)&&(rbtn->sta&(1<<6)))//有按键按下且松开,并且TP松开了
		{
			btn_delete(rbtn);
			btn_delete(modebtn);
			btn_delete(sendbtn);
			edit_delete(tedit_mode);
			edit_delete(tedit_send);
			edit_delete(tedit_recv);
			return;
		}
		
		//检测模式键是否按下
		key=btn_check(modebtn,&in_obj);
		if(key&&((modebtn->sta&(1<<7))==0)&&(modebtn->sta&(1<<6)))//有按键按下且松开,并且TP松开了
		{
			mode=!mode;
			CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,mode);
			tedit_recv->text="        ";
			edit_draw(tedit_recv);
			if(mode)
			{
				tedit_mode->text="回环模式";
				edit_draw(tedit_mode);
			}
			else
			{
				tedit_mode->text="正常模式";
				edit_draw(tedit_mode);
			}
		}
		
		//检测发送键是否按下
		key=btn_check(sendbtn,&in_obj);
		if(key&&((sendbtn->sta&(1<<7))==0)&&(sendbtn->sta&(1<<6)))//有按键按下且松开,并且TP松开了
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
u8 send_buf[]="欢迎使用普中科技STM32开发板，我们竭诚为您服务！";
void RS485_Test(void)//RS485通信测试
{
	_btn_obj* rbtn=0;				//返回按钮控件
	_btn_obj* sendbtn=0;//发送数据按键
	u8 rval=0;
	u8 key;
	u8 recbuf[64];
	_memo_obj* memo_send;
	_memo_obj* memo_recv;
	_t9_obj * t9=0;		//输入法
	u16 t9height=0;		//t9输入法高度
	
	RS485_Init(9600);
	
	FRONT_COLOR=WHITE;  
  	BACK_COLOR=LGRAY;  
	LCD_Clear(BACK_COLOR);		//清屏
	app_filebrower("RS485通信应用",0X05);//显示标题
	app_gui_tcbar(0,tftlcd_data.height-gui_phy.tbheight,tftlcd_data.width,gui_phy.tbheight,0x01);	//上分界线
	rbtn=btn_creat(tftlcd_data.width-2*gui_phy.tbfsize-8-1,tftlcd_data.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮
	if(!rbtn)rval=1;			//没有足够内存够分配
	else
	{																				
		rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//返回 
		rbtn->font=gui_phy.tbfsize;//设置新的字体大小	 	 
		rbtn->bcfdcolor=WHITE;	//按下时的颜色
		rbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(rbtn);			//重画按钮
	}
	
	gui_show_string("请注意：波特率为9600",10,gui_phy.tbheight+205,tftlcd_data.width,16,16,GREEN);
	gui_show_string("数据发送：",10,gui_phy.tbheight+10,tftlcd_data.width,16,16,BLUE);
	memo_send=memo_creat(10,gui_phy.tbheight+30,tftlcd_data.width-20,50,0,1,16,COM_MAX_DATA_LEN);
	if(memo_send==NULL)return;//创建失败
	memo_send->text=send_buf;
	memo_send->textcolor=RED;
	memo_draw_memo(memo_send,0);
	sendbtn=btn_creat(tftlcd_data.width/2-25,gui_phy.tbheight+90,50,30,0,0x02);
	if(!sendbtn)rval=1;			//没有足够内存够分配
	else
	{																				
		sendbtn->caption="发送"; 
		sendbtn->font=16;//设置新的字体大小	 	 
		sendbtn->bcfdcolor=WHITE;	//按下时的颜色
		sendbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(sendbtn);			//重画按钮
	}
	gui_show_string("数据接收：",10,gui_phy.tbheight+130,tftlcd_data.width,16,16,BLUE);
	memo_recv=memo_creat(10,gui_phy.tbheight+150,tftlcd_data.width-20,50,0,0,16,COM_MAX_DATA_LEN);
	if(memo_recv==NULL)return;//创建失败
	memo_recv->textcolor=RED;
	memo_draw_memo(memo_recv,0);
	
	if(tftlcd_data.width==800)t9height=548;		//t9输入法高度
	else if(tftlcd_data.width==600)t9height=368;	//t9输入法高度
	else if(tftlcd_data.width==480)t9height=266;	//t9输入法高度
	else if(tftlcd_data.width==320||tftlcd_data.width==272)t9height=176;	//t9输入法高度
	else if(tftlcd_data.width==240)t9height=134; 		
	t9=t9_creat((tftlcd_data.width%5)/2,tftlcd_data.height-t9height-gui_phy.tbheight,tftlcd_data.width-(tftlcd_data.width%5),t9height,0); 
	if(t9==NULL)rval=1;	 
	else t9_draw(t9);
	
	while(rval==0)//主循环
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		delay_ms(5);
		
		memo_check(memo_send,&in_obj);
		
		t9_check(t9,&in_obj);	
		memo_check(memo_send,&in_obj);
		if(t9->outstr[0]!=NULL)//添加字符
		{
			//printf("%s\r\n",t9->outstr);
			memo_add_text(memo_send,t9->outstr);
			t9->outstr[0]=NULL;	 			//清空输出字符
		}
		
		//检测返回键是否按下
		key=btn_check(rbtn,&in_obj);
		if(key&&((rbtn->sta&(1<<7))==0)&&(rbtn->sta&(1<<6)))//有按键按下且松开,并且TP松开了
		{
			USART_Cmd(USART2, DISABLE);
			btn_delete(rbtn);
			btn_delete(sendbtn);
			memo_delete(memo_send);
			memo_delete(memo_recv);
			t9_delete(t9);
			return;
		}
		
		//检测发送键是否按下
		key=btn_check(sendbtn,&in_obj);
		if(key&&((sendbtn->sta&(1<<7))==0)&&(sendbtn->sta&(1<<6)))//有按键按下且松开,并且TP松开了
		{
			RS485_Send_Data(send_buf,sizeof(send_buf));
		}
		
		RS485_Receive_Data(recbuf,&key);
		if(key)//接收到数据
		{
			memo_recv->text=recbuf;
			memo_draw_memo(memo_recv,0);
		}
	}
}

void RS232_Test(void)//RS232通信测试
{
	_btn_obj* rbtn=0;				//返回按钮控件
	_btn_obj* sendbtn=0;//发送数据按键
	u8 rval=0;
	u8 key;
	_memo_obj* memo_send;
	_memo_obj* memo_recv;
	_t9_obj * t9=0;		//输入法
	u16 t9height=0;		//t9输入法高度
	
	RS232_Init(9600);
	
	FRONT_COLOR=WHITE;  
  	BACK_COLOR=LGRAY;  
	LCD_Clear(BACK_COLOR);		//清屏
	app_filebrower("RS232通信应用",0X05);//显示标题
	app_gui_tcbar(0,tftlcd_data.height-gui_phy.tbheight,tftlcd_data.width,gui_phy.tbheight,0x01);	//上分界线
	rbtn=btn_creat(tftlcd_data.width-2*gui_phy.tbfsize-8-1,tftlcd_data.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮
	if(!rbtn)rval=1;			//没有足够内存够分配
	else
	{																				
		rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//返回 
		rbtn->font=gui_phy.tbfsize;//设置新的字体大小	 	 
		rbtn->bcfdcolor=WHITE;	//按下时的颜色
		rbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(rbtn);			//重画按钮
	}
	
	gui_show_string("请注意：波特率为9600",10,gui_phy.tbheight+205,tftlcd_data.width,16,16,GREEN);
	gui_show_string("数据发送：",10,gui_phy.tbheight+10,tftlcd_data.width,16,16,BLUE);
	memo_send=memo_creat(10,gui_phy.tbheight+30,tftlcd_data.width-20,50,0,1,16,COM_MAX_DATA_LEN);
	if(memo_send==NULL)return;//创建失败
	memo_send->text=send_buf;
	memo_send->textcolor=RED;
	memo_draw_memo(memo_send,0);
	
	sendbtn=btn_creat(tftlcd_data.width/2-25,gui_phy.tbheight+90,50,30,0,0x02);
	if(!sendbtn)rval=1;			//没有足够内存够分配
	else
	{																				
		sendbtn->caption="发送"; 
		sendbtn->font=16;//设置新的字体大小	 	 
		sendbtn->bcfdcolor=WHITE;	//按下时的颜色
		sendbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(sendbtn);			//重画按钮
	}
	gui_show_string("数据接收：",10,gui_phy.tbheight+130,tftlcd_data.width,16,16,BLUE);
	memo_recv=memo_creat(10,gui_phy.tbheight+150,tftlcd_data.width-20,50,0,0,16,COM_MAX_DATA_LEN);
	if(memo_recv==NULL)return;//创建失败
	memo_recv->textcolor=RED;
	memo_draw_memo(memo_recv,0);
	
	if(tftlcd_data.width==800)t9height=548;		//t9输入法高度
	else if(tftlcd_data.width==600)t9height=368;	//t9输入法高度
	else if(tftlcd_data.width==480)t9height=266;	//t9输入法高度
	else if(tftlcd_data.width==320||tftlcd_data.width==272)t9height=176;	//t9输入法高度
	else if(tftlcd_data.width==240)t9height=134; 		
	t9=t9_creat((tftlcd_data.width%5)/2,tftlcd_data.height-t9height-gui_phy.tbheight,tftlcd_data.width-(tftlcd_data.width%5),t9height,0); 
	if(t9==NULL)rval=1;	 
	else t9_draw(t9);
	
	while(rval==0)//主循环
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		delay_ms(5);
		
		t9_check(t9,&in_obj);	
		memo_check(memo_send,&in_obj);
		if(t9->outstr[0]!=NULL)//添加字符
		{
			//printf("%s\r\n",t9->outstr);
			memo_add_text(memo_send,t9->outstr);
			t9->outstr[0]=NULL;	 			//清空输出字符
		}
		
		//检测返回键是否按下
		key=btn_check(rbtn,&in_obj);
		if(key&&((rbtn->sta&(1<<7))==0)&&(rbtn->sta&(1<<6)))//有按键按下且松开,并且TP松开了
		{
			USART_Cmd(USART2, DISABLE);
			btn_delete(rbtn);
			btn_delete(sendbtn);
			memo_delete(memo_send);
			memo_delete(memo_recv);
			t9_delete(t9);
			return;
		}
		
		//检测发送键是否按下
		key=btn_check(sendbtn,&in_obj);
		if(key&&((sendbtn->sta&(1<<7))==0)&&(sendbtn->sta&(1<<6)))//有按键按下且松开,并且TP松开了
		{
			RS232_SendString(memo_send->text);
		}
		
		if(RS232_RX_STA&0x8000)//接收到数据
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
	_btn_obj* rbtn=0;				//返回按钮控件
	_btn_obj* ctbtn[3];//CAN/RS232/RS485通信控制按钮
	u8 rval=0;
	u8 key;
	u8 i=0;
	
	
	FRONT_COLOR=WHITE;  
  	BACK_COLOR=LGRAY;  
	LCD_Clear(BACK_COLOR);		//清屏
	app_filebrower("通信应用",0X05);//显示标题
	app_gui_tcbar(0,tftlcd_data.height-gui_phy.tbheight,tftlcd_data.width,gui_phy.tbheight,0x01);	//上分界线
	rbtn=btn_creat(tftlcd_data.width-2*gui_phy.tbfsize-8-1,tftlcd_data.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮
	if(!rbtn)rval=1;			//没有足够内存够分配
	else
	{																				
		rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//返回 
		rbtn->font=gui_phy.tbfsize;//设置新的字体大小	 	 
		rbtn->bcfdcolor=WHITE;	//按下时的颜色
		rbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(rbtn);			//重画按钮
	}
	
	//创建COM控制按钮
	for(i=0;i<3;i++)
	{
		ctbtn[i]=btn_creat(COM_BUTTON_XSTART,COM_BUTTON_YSTART+(COM_BUTTON_HEIGHT+COM_BUTTON_YSPACE)*i,COM_BUTTON_WIDTH,COM_BUTTON_HEIGHT,0,0x02);//创建按钮
		if(ctbtn[i]==NULL)rval=1;	//没有足够内存够分配
		else
		{																				
			ctbtn[i]->caption=COM_BUTTON_CAPTION_TBL[i];//返回 
			ctbtn[i]->font=COM_BUTTON_FONT;//设置新的字体大小	 	 
			ctbtn[i]->bcfdcolor=WHITE;	//按下时的颜色
			ctbtn[i]->bcfucolor=WHITE;	//松开时的颜色
			btn_draw(ctbtn[i]);			//重画按钮
		}
	}
	//等待某个控制按键按下
	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
		
		//检测返回键是否按下
		key=btn_check(rbtn,&in_obj);
		if(key&&((rbtn->sta&(1<<7))==0)&&(rbtn->sta&(1<<6)))//有按键按下且松开,并且TP松开了
		{
			btn_delete(rbtn);
			ICON_UI_Init();
			return;
		}
		
		for(i=0;i<3;i++)
		{
			key=btn_check(ctbtn[i],&in_obj);
			if(key&&((ctbtn[i]->sta&(1<<7))==0)&&(ctbtn[i]->sta&(1<<6)))//有按键按下且松开,并且TP松开了
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
