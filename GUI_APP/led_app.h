#ifndef _led_app_H
#define _led_app_H

#include "gui.h"


typedef struct 
{
	u16 x;	//x,y,r LEDλ�ð뾶
	u16 y;
	u8 r;
	u16 fcolor;//LED��ʾ��ɫ
	u16	bcolor;//LED������ɫ
	u8 *caption;//LED����
	u8 xspace;	//LED���X
	u8 yspace;	//LED���Y
}_led_ctrl;

extern _led_ctrl led_ctrl;


void LED_APP_Test(void);


#endif
