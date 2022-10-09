#include "common.h"
#include "tftlcd.h"
#include "SysTick.h"
#include "gui.h"
#include "touch.h"
#include "led.h"

//#include "pic_kj_logo.h"
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


vu8 system_task_return;		//����ǿ�Ʒ��ر�־

u8* asc2_s6030=0;	//���������60*30���������
u8* asc2_5427=0;	//��ͨ����54*27���������
u8* asc2_3618=0;	//��ͨ����36*18���������
u8* asc2_2814=0;	//��ͨ����28*14���������


u8*const APP_OK_PIC="0:/SYSTEM/APP/COMMON/ok.bmp";				//ȷ��ͼ��
u8*const APP_CANCEL_PIC="0:/SYSTEM/APP/COMMON/cancel.bmp";		//ȡ��ͼ��
u8*const APP_UNSELECT_PIC="0:/SYSTEM/APP/COMMON/unselect.bmp";	//δѡ��ͼ��
u8*const APP_SELECT_PIC="0:/SYSTEM/APP/COMMON/select.bmp";		//ѡ��ͼ��
u8*const APP_VOL_PIC="0:/SYSTEM/APP/COMMON/VOL.bmp";			//����ͼƬ·��

//ģʽѡ���б�Ĵ�������
u8*const APP_MODESEL_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
"ģʽѡ��","ģʽ�x��","Mode Select",
}; 
//��ʾ��Ϣ�Ĵ�������
u8*const APP_REMIND_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
"��ʾ��Ϣ","��ʾ��Ϣ","Remind",	 
};
//���ѱ���Ĵ�������
u8*const APP_SAVE_CAPTION_TBL[GUI_LANGUAGE_NUM]=
{
"�Ƿ񱣴�༭����ļ�?","�Ƿ񱣴澎݋����ļ�?","Do you want to save?",
};
//�����ļ�ʧ��,��ʾ�Ƿ����SD��? ,200�Ŀ��
u8*const APP_CREAT_ERR_MSG_TBL[GUI_LANGUAGE_NUM]=
{							 
"�����ļ�ʧ��,����!","�����ļ�ʧ��,Ո�z��!","Creat File Failed,Please Check!",
};


//ICONӦ��APPͼ���б�����
const unsigned char* icon_ui_app_tbl[][2]=
{
	{gImage_led_icon,"LED"},
	{gImage_clock_icon,"ʱ��"},
	{gImage_calc_icon,"������"},
	{gImage_3d_icon,"3D"},
	{gImage_picture_icon,"���"},
	{gImage_paint_icon,"С����"},
	{gImage_ebook_icon,"������"},
	{gImage_notepad_icon,"���±�"},
	{gImage_usb_icon,"������"},
	{gImage_earthnet_icon,"��̫��"},
	{gImage_music_icon,"����"},
	{gImage_camera_icon,"�����"},
	{gImage_com_icon,"ͨ��"},
	{gImage_wireless_icon,"����"},
	{gImage_phone_icon,"�绰"},
	{gImage_qrcode_icon,"RGB�ʵ�"}
};
	

//����LOGO�����ʼ��
void LOGO_Display(void)
{
#if defined(TFTLCD_PIXEL_240X320)||defined(TFTLCD_PIXEL_320X480)		
	LCD_Clear(WHITE);
//	LCD_ShowPicture(tftlcd_data.width/2-PIC_KJ_LOGO_WIDTH/2,tftlcd_data.height/2-PIC_KJ_LOGO_HEIGHT/2,PIC_KJ_LOGO_WIDTH,PIC_KJ_LOGO_HEIGHT,(u8 *)gImage_pic_kj_logo);
	delay_ms(1000);
#endif	
}

u8 rtc_showflag=0;
//�˻������ʼ��
void ICON_UI_Init(void)
{
	u8 i=0,j=0;
	
	LCD_Clear(UI_BACKCOLOR);//LIGHTGREEN	
	FRONT_COLOR=UI_FRONTCOLOR;
	BACK_COLOR=UI_BACKCOLOR;
	
	//������Ʒ�ͺš���˾���ơ�ʱ����ʾ
	gui_show_string("SmartDriver",10,2,100,TOP_STATUS_NAME_HEIGHT,TOP_STATUS_NAME_FONT_SIZE,FRONT_COLOR);
	gui_show_strmid(0,0,tftlcd_data.width,TOP_STATUS_NAME_HEIGHT,FRONT_COLOR,TOP_STATUS_NAME_FONT_SIZE,"PRECHIN");
	rtc_showflag=1;
	
	//APPӦ��ͼ�ꡢ������ʾ
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

//�ж��ĸ�Ӧ��APPͼ�걻���£������ض�Ӧ����
//����ֵ:0~15,��˫����ͼ����
//		0xff,û���κ�ͼ�걻˫�����߰���
u8 get_icon_app_table(void)
{
	u8 index=0xff;
	static u16 curxpos=0;//��ǰtp���µ�x����
	static u16 curypos=0;//��ǰtp���µ�y����
	static u8 curtpsta=0;//�������±�־
	u8 i=0,j=0;
	
	tp_dev.scan(0);	//ɨ��
	if(tp_dev.sta&TP_PRES_DOWN)//�а���������
	{
		curxpos=tp_dev.x[0];	//��¼��ǰ����
		curypos=tp_dev.y[0];	//��¼��ǰ����
		curtpsta=1;
	}
	else	//�����ɿ���
	{
		if(curtpsta)//֮ǰ�а���
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
						index=j+4*i;//�õ�ѡ�еı��
						break;
					}
				}
			}
//			printf("index=%d\r\n",index);
//			printf("curxpos=%d  curypos=%d\r\n",curxpos,curypos);
		}
		curtpsta=0;//��ձ�־
	}
	return index;
}


//�ļ����������ʾ
//topname:�����ʱ��Ҫ��ʾ������	 
//mode:
//[0]:0,����ʾ�Ϸ�ɫ��;1,��ʾ�Ϸ�ɫ��
//[1]:0,����ʾ�·�ɫ��;1,��ʾ�·�ɫ��
//[2]:0,����ʾ����;1,��ʾ����
//[3~7]:����
//����ֵ:��	 						  
void app_filebrower(u8 *topname,u8 mode)
{		
  	if(mode&0X01)app_gui_tcbar(0,0,tftlcd_data.width,gui_phy.tbheight,0x02);								//�·ֽ���
	if(mode&0X02)app_gui_tcbar(0,tftlcd_data.height-gui_phy.tbheight,tftlcd_data.width,gui_phy.tbheight,0x01);//�Ϸֽ���
	if(mode&0X04)gui_show_strmid(0,0,tftlcd_data.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize,topname);	  
}

//2ɫ��
//x,y,width,height:���꼰�ߴ�.
//mode:	���÷ֽ���
//	    [3]:�ұ߷ֽ���
//		[2]:��߷ֽ���
//		[1]:�±߷ֽ���
//		[0]:�ϱ߷ֽ���
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode)
{
 	u16 halfheight=height/2;
 	gui_fill_rectangle(x,y,width,halfheight,LIGHTBLUE);  			//���ײ���ɫ(ǳ��ɫ)	
	gui_fill_rectangle(x,y+halfheight,width,halfheight,GRAYBLUE); 	//���ײ���ɫ(����ɫ)
	if(mode&0x01)gui_draw_hline(x,y,width,DARKBLUE);
	if(mode&0x02)gui_draw_hline(x,y+height-1,width,DARKBLUE);
	if(mode&0x04)gui_draw_vline(x,y,height,DARKBLUE);
	if(mode&0x08)gui_draw_vline(x+width-1,y,width,DARKBLUE);
} 


//��õ�ǰ��Ŀ��ͼ��·��
//mode:0,��ѡģʽ;1,��ѡģʽ
//selpath,unselpath:ѡ�кͷ�ѡ�е�ͼ��·��
//selx:ѡ��ͼ����(��ѡģʽ)/��Чͼ������(��ѡģʽ)
//index:��ǰ��Ŀ���
u8 * app_get_icopath(u8 mode,u8 *selpath,u8 *unselpath,u8 selx,u8 index)
{
	u8 *icopath=0;
	if(mode)//��ѡģʽ
	{
		if(selx&(1<<index))icopath=selpath;	//��ѡ�е���Ŀ
		else icopath=unselpath;			  	//�Ƿ�ѡ�е���Ŀ
	}else
	{
		if(selx==index)icopath=selpath;		//��ѡ�е���Ŀ
		else icopath=unselpath;			  	//�Ƿ�ѡ�е���Ŀ
	}
	return icopath;
}

//��ʾ��Ŀ
//x,y,itemwidth,itemheight:��Ŀ���꼰�ߴ�
//name:��Ŀ����
//icopath:ͼ��·��
void app_show_items(u16 x,u16 y,u16 itemwidth,u16 itemheight,u8*name,u8*icopath,u16 color,u16 bkcolor)
{
  	gui_fill_rectangle(x,y,itemwidth,itemheight,bkcolor);	//��䱳��ɫ
	gui_show_ptstr(x+5,y+(itemheight-16)/2,x+itemwidth-10-APP_ITEM_ICO_SIZE-5,y+itemheight,0,color,16,name,1);	//��ʾ��Ŀ����
	if(icopath)minibmp_decode(icopath,x+itemwidth-10-APP_ITEM_ICO_SIZE,y+(itemheight-APP_ITEM_ICO_SIZE)/2,APP_ITEM_ICO_SIZE,APP_ITEM_ICO_SIZE,0,0);			//����APP_ITEM_ICO_SIZE*APP_ITEM_ICO_SIZE��bmpͼƬ
}	

//��һ��ƽ�����ɵĲ�ɫ��(�����)
//���м�Ϊ���,����չ��
//x,y,width,height:����������ߴ�
//sergb,mrgb:��ֹ��ɫ���м���ɫ
void app_draw_smooth_line(u16 x,u16 y,u16 width,u16 height,u32 sergb,u32 mrgb)
{	  
	gui_draw_smooth_rectangle(x,y,width/2,height,sergb,mrgb);	   		//ǰ��ν���
	gui_draw_smooth_rectangle(x+width/2,y,width/2,height,mrgb,sergb);   //���ν���
}

//�жϴ�������ǰֵ�ǲ�����ĳ��������
//tp:������
//x,y,width,height:����ͳߴ� 
//����ֵ:0,����������;1,��������.
u8 app_tp_is_in_area(_m_tp_dev *tp,u16 x,u16 y,u16 width,u16 height)
{						 	 
	if(tp->x[0]<=(x+width)&&tp->x[0]>=x&&tp->y[0]<=(y+height)&&tp->y[0]>=y)return 1;
	else return 0;							 	
}

//��Ŀѡ��
//x,y,width,height:����ߴ�(width��СΪ150,height��СΪ72)
//items[]:��Ŀ���ּ�
//itemsize:����Ŀ��(��󲻳���8��)
//selx:���.��ѡģʽʱ,��Ӧ�����ѡ�����.��ѡģʽʱ,��Ӧѡ�����Ŀ.
//mode:
//[7]:0,��OK��ť;1,��OK��ť
//[6]:0,����ȡ����ɫ;1,��ȡ����ɫ
//[5]:0,��ѡģʽ;1,��ѡģʽ
//[4]:0,������ͼ��;1,����ͼ��
//[3:0]:����
//caption:��������	  
//����ֵ:0,ok;����,ȡ�����ߴ���.
u8 app_items_sel(u16 x,u16 y,u16 width,u16 height,u8 *items[],u8 itemsize,u8 *selx,u8 mode,u8*caption) 
{
	u8 rval=0,res;
	u8 selsta=0;	//ѡ��״̬Ϊ0,
					//[7]:����Ƿ��Ѿ���¼��һ�ΰ��µ���Ŀ;
					//[6:4]:����
	                //[3:0]:��һ�ΰ��µ���Ŀ
	u16 i;

	u8 temp;
	u16 itemheight=0;		//ÿ����Ŀ�ĸ߶�
	u16 itemwidth=0;		//ÿ����Ŀ�Ŀ��
	u8* unselpath=0;		//δѡ�е�ͼ���·��
	u8* selpath=0;			//ѡ��ͼ���·��
	u8* icopath=0;

 	_window_obj* twin=0;	//����
 	_btn_obj * okbtn=0;		//ȷ����ť
 	_btn_obj * cancelbtn=0; //ȡ����ť

	if(itemsize>8||itemsize<1)return 0xff;	//��Ŀ������
	if(width<150||height<72)return 0xff; 	//�ߴ����
	
	itemheight=(height-72)/itemsize-1;	//�õ�ÿ����Ŀ�ĸ߶�
	itemwidth=width-10;					//ÿ����Ŀ�Ŀ��

 	twin=window_creat(x,y,width,height,0,1|(1<<5)|((1<<6)&mode),16);//��������
	if(twin==NULL)
	{
		return 0XFE;
		//spb_delete();//�ͷ�SPBռ�õ��ڴ�
		//twin=window_creat(x,y,width,height,0,1|(1<<5)|((1<<6)&mode),16);//���´�������
 	}
  	if(mode&(1<<7))
	{
   		temp=(width-APP_ITEM_BTN1_WIDTH*2)/3;
		okbtn=btn_creat(x+temp,y+height-APP_ITEM_BTN_HEIGHT-5,APP_ITEM_BTN1_WIDTH,APP_ITEM_BTN_HEIGHT,0,0x02);							//����OK��ť
		cancelbtn=btn_creat(x+APP_ITEM_BTN1_WIDTH+temp*2,y+height-APP_ITEM_BTN_HEIGHT-5,APP_ITEM_BTN1_WIDTH,APP_ITEM_BTN_HEIGHT,0,0x02);//����CANCEL��ť
		if(twin==NULL||okbtn==NULL||cancelbtn==NULL)rval=1;
		else
		{
	 		okbtn->caption=(u8*)GUI_OK_CAPTION_TBL[gui_phy.language];//ȷ��
			okbtn->bkctbl[0]=0X8452;//�߿���ɫ
			okbtn->bkctbl[1]=0XAD97;//��һ�е���ɫ				
			okbtn->bkctbl[2]=0XAD97;//�ϰ벿����ɫ
			okbtn->bkctbl[3]=0X8452;//�°벿����ɫ
		}
	}else 
	{
   		temp=(width-APP_ITEM_BTN2_WIDTH)/2;
		cancelbtn=btn_creat(x+temp,y+height-APP_ITEM_BTN_HEIGHT-5,APP_ITEM_BTN2_WIDTH,APP_ITEM_BTN_HEIGHT,0,0x02);	//����CANCEL��ť
		if(twin==NULL||cancelbtn==NULL)rval=1;
	}
 	if(rval==0)//֮ǰ�Ĳ�������
	{
 		twin->caption=caption;
		twin->windowbkc=APP_WIN_BACK_COLOR;	     
 		cancelbtn->caption=(u8*)GUI_CANCEL_CAPTION_TBL[gui_phy.language];//ȡ��
 		cancelbtn->bkctbl[0]=0X8452;//�߿���ɫ
		cancelbtn->bkctbl[1]=0XAD97;//��һ�е���ɫ				
		cancelbtn->bkctbl[2]=0XAD97;//�ϰ벿����ɫ
		cancelbtn->bkctbl[3]=0X8452;//�°벿����ɫ

		if(mode&(1<<4))//��Ҫ����ͼ��
		{
  			if(mode&(1<<5))//��ѡģʽ
			{
				unselpath=(u8*)APP_CANCEL_PIC;		//δѡ�е�ͼ���·��
				selpath=(u8*)APP_OK_PIC;			//ѡ��ͼ���·��
			}else		   //��ѡģʽ
			{
				unselpath=(u8*)APP_UNSELECT_PIC;	//δѡ�е�ͼ���·��
				selpath=(u8*)APP_SELECT_PIC;		//ѡ��ͼ���·��
			}
		}
		window_draw(twin);				//��������
		btn_draw(cancelbtn);			//����ť
	    if(mode&(1<<7))btn_draw(okbtn);	//����ť
		for(i=0;i<itemsize;i++)
		{
			icopath=app_get_icopath(mode&(1<<5),selpath,unselpath,*selx,i); //�õ�ͼ��·��
			app_show_items(x+5,y+32+i*(itemheight+1),itemwidth,itemheight,items[i],icopath,BLACK,twin->windowbkc);//��ʾ���е���Ŀ
			if((i+1)!=itemsize)app_draw_smooth_line(x+5,y+32+(i+1)*(itemheight+1)-1,itemwidth,1,0Xb1ffc4,0X1600b1);//������
 		}
		while(rval==0)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
			delay_ms(5);		//��ʱһ��ʱ�ӽ���
			if(system_task_return){rval=1;break;};	//TPAD����	
			if(mode&(1<<7))
			{
				res=btn_check(okbtn,&in_obj);		//ȷ�ϰ�ť���
				if(res)
				{
					if((okbtn->sta&0X80)==0)//����Ч����
					{
						rval=0XFF;
						break;//ȷ�ϰ�ť
					}
				}
			}   
			res=btn_check(cancelbtn,&in_obj);		//ȡ����ť���
			if(res)
			{
				if((cancelbtn->sta&0X80)==0)//����Ч����
				{
					rval=1;
					break;//ȡ����ť	 
				}
			}
			temp=0XFF;//�����,���Ϊ0XFF,���ɿ���ʱ��,˵���ǲ�����Ч�����ڵ�.�����0XFF,���ʾTP�ɿ���ʱ��,������Ч������.
			for(i=0;i<itemsize;i++)
			{
				if(tp_dev.sta&TP_PRES_DOWN)//������������
				{
				 	if(app_tp_is_in_area(&tp_dev,x+5,y+32+i*(itemheight+1),itemwidth,itemheight))//�ж�ĳ��ʱ��,��������ֵ�ǲ�����ĳ��������
					{ 
						if((selsta&0X80)==0)//��û�а��¹�
						{
							icopath=app_get_icopath(mode&(1<<5),selpath,unselpath,*selx,i); //�õ�ͼ��·��
							app_show_items(x+5,y+32+i*(itemheight+1),itemwidth,itemheight,items[i],icopath,BLACK,APP_ITEM_SEL_BKCOLOR);//��ѡ��Ŀ
							selsta=i;		//��¼��һ�ΰ��µ���Ŀ
							selsta|=0X80;	//����Ѿ����¹���
						}
						break;		
					}
				}else //���������ɿ���
				{
				 	if(app_tp_is_in_area(&tp_dev,x+5,y+32+i*(itemheight+1),itemwidth,itemheight))//�ж�ĳ��ʱ��,��������ֵ�ǲ�����ĳ��������
					{ 
						temp=i;	   
						break;
					}
				}
			}
			if((selsta&0X80)&&(tp_dev.sta&TP_PRES_DOWN)==0)//�а��¹�,�Ұ����ɿ���
			{
				if((selsta&0X0F)==temp)//�ɿ�֮ǰ������Ҳ���ڰ���ʱ��������.
				{
					if(mode&(1<<5))//��ѡģʽ,ִ��ȡ������
					{
						if((*selx)&(1<<temp))*selx&=~(1<<temp);
						else *selx|=1<<temp;
					}else//��ѡģʽ
					{																					  
						app_show_items(x+5,y+32+(*selx)*(itemheight+1),itemwidth,itemheight,items[*selx],unselpath,BLACK,twin->windowbkc);//ȡ��֮ǰѡ�����Ŀ
						*selx=temp;
					}
				}else temp=selsta&0X0F;//�õ���ʱ���µ���Ŀ��
 				icopath=app_get_icopath(mode&(1<<5),selpath,unselpath,*selx,temp); //�õ�ͼ��·��
				app_show_items(x+5,y+32+temp*(itemheight+1),itemwidth,itemheight,items[temp],icopath,BLACK,twin->windowbkc);//��ѡ��Ŀ
				selsta=0;//ȡ��
			}
 		}
 	}
	window_delete(twin);
	btn_delete(okbtn);
	btn_delete(cancelbtn);
	system_task_return=0;
	if(rval==0XFF)return 0;
	return rval;
} 

////////////////////////////////α����������취////////////////////////////////
u32 random_seed=1;
void app_srand(u32 seed)
{
	random_seed=seed;
}
//��ȡα�����
//���Բ���0~RANDOM_MAX-1�������
//seed:����
//max:���ֵ	  		  
//����ֵ:0~(max-1)�е�һ��ֵ 		
u32 app_get_rand(u32 max)
{			    	    
	random_seed=random_seed*22695477+1;
	return (random_seed)%max; 
}

