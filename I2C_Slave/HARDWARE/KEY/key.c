#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"


void KEY_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//初始化KEY1-->GPIOC.14,KEY2-->GPIOC.15  上拉输入
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.14/15
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //设置成浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.8/9
	
}
u8 KEY_Scan()
{
	u8 temp=0;
	if(KEY1==0) return temp;
	else return 0;
}

