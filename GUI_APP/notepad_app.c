#include "notepad_app.h"
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
#include "t9input.h"
#include "rtc.h"
#include "key.h"


//定义最大的编辑长度.也就是新建文本文件的大小.或者打开一个旧文件后最大允许增加的长度.
#define NOTEPAD_EDIT_LEN	2048

//模式选择
u8*const notepad_mode_tbl[GUI_LANGUAGE_NUM][2]=
{
{"新建文本文件","打开已有文件",},
{"新建文本文件","打開已有文件",},		 
{"Create new text file","Open exist file",},
};  
 
//通过时间获取文件名
//仅限在sd卡保存,不支持flash disk保存
//组合成:形如"0:TEXT/TEXT20120321210633.txt"/"2:TEXT/TEXT20120321210633.txt"的文件名 
void notepad_new_pathname(u8 *pname)
{	 
	RTC_Get();
	if(gui_phy.memdevflag&(1<<0))sprintf((char*)pname,"0:TEXT/TEXT%04d%02d%02d%02d%02d%02d.txt",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);		//首选保存在SD卡
} 
 
//记事本功能
//所有新建文本文件,都保存在SD卡的TEXT文件夹内
void Notepad_APP_Test(void)
{
	FIL* f_txt=0;					//文件
	DIR notepaddir;					//notepaddir专用	  
	FILINFO *notepadinfo;		 	   
  	_btn_obj* rbtn=0;				//返回按钮控件
   	_filelistbox_obj * flistbox=0;
	_filelistbox_list * filelistx=0;//文件表
	_t9_obj * t9=0;					//输入法  
	_memo_obj * tmemo=0;	    	//memo控件
	u8 editmask=0;					//标志是否进行了编辑
 	u8 *pname=0;
	u8 *fn;	 
	u8 res;
	u8 rval=0;
	u8 mode=0;
	u16 t9height=0;		//t9输入法高度
   	br=0; 
	
	//先选择模式 
	rval=app_items_sel((tftlcd_data.width-180)/2,(tftlcd_data.height-152)/2,180,72+40*2,(u8**)notepad_mode_tbl[gui_phy.language],2,(u8*)&mode,0X90,(u8*)APP_MODESEL_CAPTION_TBL[gui_phy.language]);//2个选择
	f_txt=(FIL *)gui_memin_malloc(sizeof(FIL));	//开辟FIL字节的内存区域 
	if(f_txt==NULL)rval=1;					//申请失败
	rbtn=btn_creat(tftlcd_data.width-2*gui_phy.tbfsize-8-1,tftlcd_data.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//创建文字按钮
	notepadinfo=(FILINFO*)gui_memin_malloc(sizeof(FILINFO));//申请FILENFO内存
	if(!notepadinfo||!rbtn)rval=1;			//没有足够内存够分配
	else
	{																				
	 	rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//返回 
	 	rbtn->font=gui_phy.tbfsize;//设置新的字体大小	 	 
		rbtn->bcfdcolor=WHITE;	//按下时的颜色
		rbtn->bcfucolor=WHITE;	//松开时的颜色
		btn_draw(rbtn);			//重画按钮
	}  
  	if(rval==0)//确认键按下了,同时按键f_txt创建OK,RBTN创建OK
	{	  
		LCD_Clear(0);  	 
   		if(mode==0)				//新建文本文件
		{
 			if(gui_phy.memdevflag&(1<<0))f_mkdir("0:TEXT");		//强制创建文件夹,给记事本用
			if(gui_phy.memdevflag&(1<<3))f_mkdir("3:TEXT");		//强制创建文件夹,给记事本用
			pname=gui_memin_malloc(40);//申请40个字节内存,类似"0:TEXT/TEXT20120321210633.txt"
			pname[0]='\0';//添加结束符
			notepad_new_pathname(pname);
			f_txt->obj.objsize=0;
			if(pname==NULL)rval=1;
 		}else	//选择一个文本文件打开
		{
			app_filebrower((u8*)notepad_mode_tbl[gui_phy.language][1],0X07);//显示信息
			btn_read_backcolor(rbtn);//重新读取按钮背景色
			btn_draw(rbtn);//画按钮  
			flistbox=filelistbox_creat(0,gui_phy.tbheight,tftlcd_data.width,tftlcd_data.height-gui_phy.tbheight*2,1,gui_phy.listfsize);//创建一个filelistbox
			if(flistbox==NULL)rval=1;							//申请内存失败.
			else  
			{
				flistbox->fliter=FLBOX_FLT_TEXT|FLBOX_FLT_LRC;	//查找TEXT文件
				filelistbox_add_disk(flistbox);	//添加磁盘路径
				filelistbox_draw_listbox(flistbox);
			} 	 
  		   	while(rval==0)
			{
				tp_dev.scan(0);    
				in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
				delay_ms(5);		//延时一个时钟节拍
		 		if(KEY_Scan(1)==KEY1_PRESS)				//KEY1按键返回
				{
					rval=1;//强制退出
					break;			
				}
				filelistbox_check(flistbox,&in_obj);	//扫描文件
				res=btn_check(rbtn,&in_obj);
				if(res)
				{
					if(((rbtn->sta&0X80)==0))//按钮状态改变了
					{
						if(flistbox->dbclick!=0X81)
						{
		 					filelistx=filelist_search(flistbox->list,flistbox->selindex);//得到此时选中的list的信息
							if(filelistx->type==FICO_DISK)
							{
								rval=1;
								break;	//已经不能再往上了
						 	}
							else filelistbox_back(flistbox);	//退回上一层目录	 
						} 
		 			}	 
				}
				if(flistbox->dbclick==0X81)//双击文件了
				{	    
 		     		rval=f_opendir(&notepaddir,(const TCHAR*)flistbox->path); //打开选中的目录
					if(rval)break;	 			   
					dir_sdi(&notepaddir,flistbox->findextbl[flistbox->selindex-flistbox->foldercnt]);
			 		rval=f_readdir(&notepaddir,notepadinfo);//读取文件信息
					if(rval)break;//打开失败  
					fn=(u8*)(notepadinfo->fname);
					pname=gui_memin_malloc(strlen((const char*)fn)+strlen((const char*)flistbox->path)+2);//申请内存
					if(pname==NULL)rval=1;//申请失败
					{
						pname=gui_path_name(pname,flistbox->path,fn);	//文件名加入路径  
			 			res=f_open(f_txt,(const TCHAR*)pname,FA_READ);//以读方式打开文件
						if(res)rval=1; 
    					break;
					} 
				}
			}   
		}   
		if(rval==0)//前面操作均无问题
		{
			if(tftlcd_data.width==800)t9height=548;		//t9输入法高度
			else if(tftlcd_data.width==600)t9height=368;	//t9输入法高度
			else if(tftlcd_data.width==480)t9height=266;	//t9输入法高度
			else if(tftlcd_data.width==320||tftlcd_data.width==272)t9height=176;	//t9输入法高度
			else if(tftlcd_data.width==240)t9height=134; 
			tmemo=memo_creat(0,gui_phy.tbheight,tftlcd_data.width,tftlcd_data.height-gui_phy.tbheight-t9height,0,1,gui_phy.tbfsize,f_txt->obj.objsize+NOTEPAD_EDIT_LEN);//申请整个txt这么多的内存	
			if(tmemo==NULL)rval=1;
			else//memo创建成功
			{
	 			if(mode==1)
				{
					res=f_read(f_txt,tmemo->text,f_txt->obj.objsize,(UINT*)&br);//读出txt里面的内容  
					f_close(f_txt);//关闭文件
				} 
				memo_draw_memo(tmemo,0);
		  		app_filebrower(pname,0X05);	//显示标题栏背景 
				t9=t9_creat((tftlcd_data.width%5)/2,tftlcd_data.height-t9height,tftlcd_data.width-(tftlcd_data.width%5),t9height,0); 
				if(t9==NULL)rval=1;	 
				else t9_draw(t9);	    
	 		}
		} 
	 	while(rval==0) 
		{		 
			tp_dev.scan(0); 		 
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值 	   
			delay_ms(5);		//延时一个时钟节拍						  
 			if(KEY_Scan(1)==KEY1_PRESS)break;			//KEY1按键返回
			t9_check(t9,&in_obj);		   
			memo_check(tmemo,&in_obj);
 			if(t9->outstr[0]!=NULL)//添加字符
			{
				//printf("%s\r\n",t9->outstr);
 				memo_add_text(tmemo,t9->outstr);
				t9->outstr[0]=NULL;	 			//清空输出字符
				editmask=1;						//标记进行了编辑
 			}
		}
 	}   
 	if(editmask)//有编辑过,需要保存
	{
 		res=window_msg_box((tftlcd_data.width-200)/2,(tftlcd_data.height-80)/2,200,80,"",(u8*)APP_SAVE_CAPTION_TBL[gui_phy.language],12,0,0X03,0);
		if(res==1)//需要保存
		{
			if(mode==0)res=f_open(f_txt,(const TCHAR*)pname,FA_READ|FA_WRITE|FA_CREATE_NEW);//创建新的文件
			else res=f_open(f_txt,(const TCHAR*)pname,FA_READ|FA_WRITE);//以读写方式打开文件
			if(res==0)//打开成功
			{
				f_lseek(f_txt,0);														//指向文件的开始地址		 
				f_write(f_txt,tmemo->text,strlen((const char*)tmemo->text),(UINT*)&br);	//保存文件
				f_close(f_txt);
			}else//打开文件失败,提示信息
			{
				window_msg_box((tftlcd_data.width-200)/2,(tftlcd_data.height-100)/2,200,100,(u8*)APP_CREAT_ERR_MSG_TBL[gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//提示SD卡是否存在
				delay_ms(2000);//等待2秒钟
			}
		}
	}
	t9_delete(t9);							  
	filelistbox_delete(flistbox);
 	btn_delete(rbtn);
 	memo_delete(tmemo);
	gui_memin_free(f_txt);
 	gui_memin_free(pname);  
	gui_memin_free(notepadinfo); 
	ICON_UI_Init();	
//	return rval;
}
