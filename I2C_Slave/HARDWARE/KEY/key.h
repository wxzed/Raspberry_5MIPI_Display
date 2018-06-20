#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"


//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2
//#define KEY3 PAin(0)	//PA0  WK_UP

#define KEY1 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)//读取按键0
#define KEY2 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)//读取按键1
#define SW1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)//读取按键2 
#define SW2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//读取按键3(WK_UP) 



void KEY_Init(void);//IO初始化
u8 KEY_Scan(void);  	//按键扫描函数	

#endif
