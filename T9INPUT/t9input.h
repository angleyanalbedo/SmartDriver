#ifndef __T9INPUT_H
#define __T9INPUT_H


#include "gui.h"  
#include "button.h"
#include "pyinput.h"



//��׼��ť�߿���ɫ����
#define EDIT_RIM_LTOC 	0XA535	    //����������ɫ
#define EDIT_RIM_LTIC 	0X8431		//����������ɫ
#define EDIT_RIM_RBOC 	0XFFFF		//����������ɫ
#define EDIT_RIM_RBIC 	0XDEFC		//����������ɫ
#define EDIT_BACK_COLOR	0XFFFF		//�༭�򱳾�ɫ

#define EDIT_DFT_TBKC 	0XFFFF		//Ĭ�ϵ��ı�����ɫ
#define EDIT_DFT_TXTC 	0X0000		//Ĭ�ϵ��ı���ɫ


#define T9_SIGNTBL_SIZE 5 

#define T9_OUTSEL_BKC	0XFFFF		//���ѡ�񱳾�ɫ
#define T9_OUTSEL_SELC	0X0000		//���ѡ�б���ɫ
#define T9_BTN_EBC		0XD6BA		//��ťʧЧ��ɫ
#define T9_BTN_DBC		0XFFFF		//��ť��Ч��ɫ


#define T9_PANEL_BKC	0X4A6A		//���뷨����ɫ

//EDIT�ṹ�嶨��
__packed typedef struct 
{
	u16 top; 				  		//��ť��������
	u16 left;                       //��ť�������
	u16 width; 				  		//����(�̶�240)			    
	u16 height;						//�߶�(�̶�134) 
 	u8 mode;						//���������
									//[7:5]:0,����
									//[4:3]:(�������ť״̬)0,���뷨�л�״̬;1,��������״̬;2,������״̬;
									//[2:0]:0,ƴ��;1,123;2,abc;3,ABC,4,��д.
	u8 sta;							//���뷨״̬
									//[7]:0,������һҳ��ť��Ч.1,������һҳ��ť��Ч
									//[6]:0,������һҳ��ť��Ч.1,������һҳ��ť��Ч
									//[5:2]:����
									//[0]:0,û���κ���������
	u8 signpage;					//��ǰ�ַ�ҳ�� 
	u8 pynum;						//ƴ����ƥ���� 
	
	_btn_obj * mbtn[12];			//12������ť  
 	_btn_obj * hbtn[8];				//����8��ѡ��ť  
	_btn_obj * vbtn[5];				//����5��ѡ��ť

	py_index *pymb[MAX_MATCH_PYMB];	//������λ��
	u8 pypage;						//��ǰƴ������ҳ
	  
	u8 *disstr;						//������ʾ���ַ���ָ��
	u8 dispagenum;			 		//�ַ�����ҳ��
	u8 dispagecur;					//��ǰҳ
	u8 disbuf[6][3];				//����ť��caption����		

	u8 instr[7];					//�����ַ���,���Ϊ6���ֽ�+������1��.����ƴ���������дʶ��
	u8 outstr[5];					//����ַ�,���Ϊ2���ֽ�*2+������1��,����ֻҪ3���ֽ�,��Ϊ�����ſ��ܳɶ�.������Ҫ5���ֽ�

}_t9_obj;

_t9_obj * t9_creat(u16 left,u16 top,u16 width,u16 height,u8 mode);
void t9_delete(_t9_obj * t9_del);

u8 t9_check(_t9_obj * t9x,void * in_key);
void t9_draw_mbtn(_t9_obj * t9x);
void t9_draw_hbtn(_t9_obj * t9x);
void t9_draw_vbtn(_t9_obj * t9x);
void t9_draw(_t9_obj * t9x);
void t9_hbtn_updc(_t9_obj * t9x,u8* str);

void t9_set_inputtype(_t9_obj *t9x,u8 type);
u8 t9_keyup_scan(_t9_obj *t9x);
u8 t9_id2num(u8 id);
void t9_clrhbtn_caption(_t9_obj *t9x,u8 btnx);
void t9_load_sign(_t9_obj *t9x);
void t9_drawvbtn_rim(_t9_obj *t9x,u16 rimcolor);
void t9_sign_process(_t9_obj *t9x,u8 id);
void t9_back_process(_t9_obj *t9x);
void t9_space_process(_t9_obj *t9x);
void t9_enter_process(_t9_obj *t9x);
void t9_hbtn_process(_t9_obj *t9x,u8 id);
void t9_vbtn_process(_t9_obj *t9x,u8 id);
void t9_pinyin_process(_t9_obj *t9x,u8 id);	   
void t9_process(_t9_obj *t9x);		 
void t9_test_task(void);	 
void t9_test(u16 x,u16 y,u8 mode);
void t9_tsdel(void);
void t9_tsdraw(void);
#endif
















