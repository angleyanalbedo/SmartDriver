#include "picture_app.h"
#include "button.h"
#include "touch.h"
#include "common.h"
#include "ff.h"
#include "fatfs_app.h"
#include "font_show.h"
#include "piclib.h"
#include "string.h"		
#include "math.h"
#include "sd_sdio.h"
#include "led.h"
#include "key.h"

//ͼƬ���ģʽ
//0��˳�򲥷ţ�1���������
u8 picmode=0;

//ͼƬ���
void Picture_APP_Test(void)
{				 	 
	DIR picdir;			//picdirר��	  
	FILINFO *picinfo;   		   
	u8 rval=0;			//����ֵ	  
	u8 *pname=0;
	u8 *fn=0;
	u8 picsta=0;		//ebook״̬
						//0,�����ļ����״̬    
						//1,˳�򲥷�ͼƬ
						//2,��ͣ״̬    
	
	u16 curindex=0;	//��ǰ�����ͼƬ�ļ���������
	u8  endecode=0;	//ʹ�ܽ���
	u8 key; 
	u16 dtime=0;	//��ʱʱ��
	u8 keyup=1;		//�ɿ����
	u8 pictype=0;	//ͼƬ���� 

   	_filelistbox_obj * flistbox; 
	_filelistbox_list * filelistx; 	//�ļ�
 	_btn_obj* rbtn;		//���ذ�ť�ؼ�

	app_filebrower("ͼƬ��ʾӦ��",0X07);//��ʾ����
	flistbox=filelistbox_creat(0,gui_phy.tbheight,tftlcd_data.width,tftlcd_data.height-gui_phy.tbheight*2,1,gui_phy.listfsize);//����һ��filelistbox
	picinfo=(FILINFO*)gui_memin_malloc(sizeof(FILINFO));//����FILENFO�ڴ�
	if(!picinfo||!flistbox)rval=1;						//�����ڴ�ʧ��.
	else  
	{
		flistbox->fliter=FLBOX_FLT_PICTURE;				//ͼƬ�ļ�
		filelistbox_add_disk(flistbox);					//��Ӵ���·��
 		filelistbox_draw_listbox(flistbox);
	} 	  
  	rbtn=btn_creat(tftlcd_data.width-2*gui_phy.tbfsize-8-1,tftlcd_data.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//�������ְ�ť
 	if(rbtn==NULL)rval=1;	//û���㹻�ڴ湻����
	else
	{																				
	 	rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//���� 
	 	rbtn->font=gui_phy.tbfsize;//�����µ������С	 	 
		rbtn->bcfdcolor=WHITE;	//����ʱ����ɫ
		rbtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
		btn_draw(rbtn);//����ť
	}
	LED1=1;//�ر�LED1
	while(rval==0)//��ѭ��
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
		delay_ms(5);
		if(system_task_return)break;			//���ݴ�����������   	
		if(picsta==0)
		{
			filelistbox_check(flistbox,&in_obj);	//ɨ���ļ�
	 		if(flistbox->dbclick==0X81)				//˫���ļ���
			{	
				curindex=flistbox->selindex-flistbox->foldercnt;//�õ���ǰͼƬ������
				picsta=1;	//ͼƬ����״̬		 
				endecode=1;	//��һ��ͼƬ�Զ����� 
				LCD_Clear(0x0);//���� 	 
			}
		}
		key=btn_check(rbtn,&in_obj);
  		if(key&&((rbtn->sta&0X80)==0))
		{		  
			if(flistbox->dbclick!=0X81)//���ļ������ʱ���˷��ذ�ť,�򷵻���һ��Ŀ¼
			{
				filelistx=filelist_search(flistbox->list,flistbox->selindex);//�õ���ʱѡ�е�list����Ϣ
				if(filelistx->type==FICO_DISK)//�Ѿ�������������,���˳�ͼƬ���
				{				 
					break;
				}else filelistbox_back(flistbox);//�˻���һ��Ŀ¼	 
			} 
		} 
		if(endecode)
		{
     		rval=f_opendir(&picdir,(const TCHAR*)flistbox->path); //��ѡ�е�Ŀ¼
			if(rval)break;	 			   
//			ff_enter(picdir.obj.fs);//����fatfs,��ֹ�����.
			dir_sdi(&picdir,flistbox->findextbl[curindex]);
//			ff_leave(picdir.obj.fs);//�˳�fatfs,��������os��
	 		rval=f_readdir(&picdir,picinfo);//��ȡ�ļ���Ϣ
			if(rval)break;//�򿪳ɹ�    
			fn=(u8*)(picinfo->fname);
			pname=gui_memin_malloc(strlen((const char*)fn)+strlen((const char*)flistbox->path)+2);//�����ڴ�
			if(pname==NULL)break;	//����ʧ��
			pname=gui_path_name(pname,flistbox->path,fn);	//�ļ�������·�� 
			pictype=f_typetell(pname); 
			if(pictype==T_GIF)gui_show_string(fn,5,5,tftlcd_data.width-5,gui_phy.tbfsize,gui_phy.tbfsize,RED);	//��ʾGIF����
			picfile_display(pname,0,0,tftlcd_data.width,tftlcd_data.height,1);			//�������ͼƬ
			if(pictype!=T_GIF)gui_show_string(fn,5,5,tftlcd_data.width-5,gui_phy.tbfsize,gui_phy.tbfsize,RED);	//��ʾͼƬ����						  
 			gui_memin_free(pname);			//�ͷ��ڴ� 
			pname=NULL;
			endecode=0;		    
		}
		keyup=0;
		dtime=0;
		while(picsta)//����ɨ��ѭ��
		{     
			key=pic_tp_scan();   
			dtime++;
			if(dtime>400&&(picsta==1))key=3;//˳�򲥷�״̬��ʱ�����,�Զ�������һ��ͼƬ    
			if(key)//�а�������
			{ 
				dtime=0;
				if(keyup)
				{   
					keyup=0;
					if(key==1)//��һ��ͼƬ
					{
						if(curindex)curindex--;
						else curindex=flistbox->filecnt-1;
						endecode=1;
						LCD_Clear(0x0);//���� 	 
						break;
					}else if(key==2)//����ͣ�ͷ���֮ͣ���л�
					{ 
						if(picsta==1)
						{
							picsta=2;
							LED1=0;	 	//��ʾ��ͣ
						}else 
						{
							picsta=1;
							LED1=1;	   	//��ͣ����
						} 
					}else if(key==3)
					{
						if(picmode==0)//˳�򲥷�
						{
							if(curindex<(flistbox->filecnt-1))curindex++;
							else curindex=0;
						}else	//�������
						{   
							curindex=app_get_rand(flistbox->filecnt);//����õ���һ��ͼƬ�ı��							
						}
						endecode=1;
						LCD_Clear(0x0);//���� 	 
						break;	 
					}  
				}
			}else keyup=1;//��ǰ����ɿ�
			delay_ms(10);  
			if(KEY_Scan(1)==KEY1_PRESS)picsta=0;//KEY1��������
			if(picsta==0)//�ص��ļ����״̬֮ǰ�Ĵ���
			{
				LED1=1;	   				//�ر�LED1
				flistbox->dbclick=0;	//���÷��ļ����״̬
				app_filebrower("ͼƬ��ʾӦ��",0X07);//��ʾ����
 				btn_draw(rbtn);			//����ť
				filelistbox_rebuild_filelist(flistbox);//�ؽ�flistbox
 				system_task_return=0;	//�������˳�ͼƬ���
				break;
			}
		}
	}	
	LED1=1;//�ر�LED1
	filelistbox_delete(flistbox);	//ɾ��filelist 	  
	btn_delete(rbtn);				//ɾ����ť	 
	gui_memin_free(picinfo);	  
 	gui_memin_free(pname);			//�ͷ��ڴ�
	ICON_UI_Init();
// 	return rval; 
}
//ͼƬ������������
u8 pic_tp_scan(void)
{
	u8 res=0;
	tp_dev.scan(0);    
	if(tp_dev.sta&TP_PRES_DOWN)//�а�������
	{ 
		if(tp_dev.y[0]<tftlcd_data.height/3)res=1;			//��������
		else if(tp_dev.y[0]<tftlcd_data.height*2/3)res=2;	//�����м�
		else if(tp_dev.y[0]<tftlcd_data.height)res=3;		//�������� 
	} 
	while(res)//�ȴ������ɿ�
	{
		tp_dev.scan(0);  
		if((tp_dev.sta&TP_PRES_DOWN)==0)break;
		delay_ms(5);
	} 
	return res;
}
