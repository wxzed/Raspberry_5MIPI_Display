#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"	 
#include "stmflash.h"
#include "myiic.h"
#include "softiic.h"
#include "MyGlobal.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"

u8 buf[33];
u8 i,j;
u8 ICN6211_REG_ADD[25]={0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x34,0x36,0x86,0xB5,0x5C,0x2A,0x56,0x6B,0x69,0x10,0x11,0xB6,0x51,0x09};
u8 ICN6211_REG_DATA[25]={0x20,0xE0,0x13,0x28,0x30,0x58,0x00,0x0D,0x03,0x20,0x80,0x28,0x28,0xA0,0xFF,0x01,0x90,0x71,0x2A,0x40,0x88,0x20,0x20,0x10};
TouchData MyTouchData;
static void InitShow(){
	for(i = 0; i<25;i++){
			Soft_IIC_Start();
			Soft_IIC_Send_Byte(0x58);
			while(Soft_IIC_Wait_Ack()){
				Soft_IIC_Send_Byte(0x58);
			}
			Soft_IIC_Send_Byte(ICN6211_REG_ADD[i]);
			while(Soft_IIC_Wait_Ack()){
				Soft_IIC_Send_Byte(ICN6211_REG_ADD[i]);
			}
			Soft_IIC_Send_Byte(ICN6211_REG_DATA[i]);
			while(Soft_IIC_Wait_Ack()){
				Soft_IIC_Send_Byte(ICN6211_REG_DATA[i]);
			}
			Soft_IIC_Stop();
	}
}
static void InitTouchData(){
	MyTouchData.point = 0xFF;
	MyTouchData.event = 0xFF;
	for(i = 0; i < 5; i++){
		MyTouchData.xLow[i] = 0xFF;
		MyTouchData.xHigh[j] = 0xFF;
		MyTouchData.yHigh[j] = 0xFF;
		MyTouchData.yHigh[j] = 0xFF;
	}
}
void RCC_Configuration(void){
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);     
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
}
void GPIO_Configuration(void)    
{   
	
  GPIO_InitTypeDef GPIO_InitStructure;    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);   
} 
void TIM3_Configuration(void)    
{  
	
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
    
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
    
  TIM_TimeBaseStructure.TIM_Period = 500;//72MHz  72000000/36000=2000  
  TIM_TimeBaseStructure.TIM_Prescaler = 35999;//36000-1=35999  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
    
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );  
  TIM_Cmd(TIM3,ENABLE);  
}
void RCC1_Configuration(void)
{
  //ErrorStatus HSEStartUpStatus;
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();												//??????????????
  /* Enable HSE */
  //RCC_HSEConfig(RCC_HSE_ON);								//?????????(8M??)
  /* Wait till HSE is ready */
  //HSEStartUpStatus = RCC_WaitForHSEStartUp();				//????????
	RCC_HSICmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET){}
 // if(HSEStartUpStatus == SUCCESS)							//????????
  {
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);						//??AHB?????????1??
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);							//??APB2?????HCLK??1??
    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);							//??APB1?????HCLK??2??
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);						//????FLASH???????2??
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	//??FLASH??????
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_14);	//4MHz * 14 = 56 MHz
    /* Enable PLL */
    RCC_PLLCmd(ENABLE);										//??PLL??
    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)		//??PLL??????????
    {
    }
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);				//
    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)				
    {
    }
  }
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | 
												 RCC_APB2Periph_GPIOB | 
												 RCC_APB2Periph_GPIOC , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);
}

 int main(void)
 {	 

	 
	RCC1_Configuration();
	 RCC_Configuration();
	 	delay_init();	    	 //延时函数初始化	 
	//GPIO_Configuration();
	LED_Init();
	//TIM3_Configuration();
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	I2C2_Init();
	I2C1_Init();
	delay_ms(50);
	Soft_IIC_Init();
	delay_ms(50);
	//uart_init(115200);	 //串口初始化为9600
	InitTouchData();
	InitShow();
	BACKLIGHT =1;
		while(1){
		  i = 0,j=0;
			Soft_IIC_Start();
		 	Soft_IIC_Send_Byte(0x70);//读操作
			while(Soft_IIC_Wait_Ack()){
				Soft_IIC_Send_Byte(0x70);
			}
			Soft_IIC_Send_Byte(0x00);
			while(Soft_IIC_Wait_Ack()){
				Soft_IIC_Send_Byte(0x00);
			}
			Soft_IIC_Stop();
			Soft_IIC_Start();
		 	Soft_IIC_Send_Byte(0x70|0x01);
			while(Soft_IIC_Wait_Ack()){
				Soft_IIC_Send_Byte(0x70|0x01);
			}
			//delay_ms(3);
			for(i = 0;i < 32;i++){
				buf[i] = Soft_IIC_Read_Byte(1);
			}
			buf[32] = Soft_IIC_Read_Byte(0);
			Soft_IIC_Stop();
			if((buf[2] == 0x00) || (buf[2] == 0xFF)){//没有被触摸
				MyTouchData.point = buf[2];
			}else{
					//BACKLIGHT = 1;
					MyTouchData.point = buf[2];
				if(MyTouchData.point > 0x05){
					MyTouchData.point = 0x05;
				}
					for(j = 0;j<MyTouchData.point;j++){
						uint16_t x = (uint16_t)(buf[3+6*j] & 0x0F)<<8 | (uint16_t) buf[4+6*j];
						uint16_t y = (uint16_t)(buf[5+6*j] & 0x0F) << 8 | (uint16_t) buf[6+6*j];
						uint8_t event = buf[0x3+6*j] >> 6;
						uint8_t id = buf[5+6*j]>>4;
						if(x > 800){
							x =800;
						}
						if(y > 480){
							y = 480;
						}
						x=800-x;
						y=480-y;
						MyTouchData.xLow[j] = x & 0xFF;
						MyTouchData.xHigh[j] = (x >> 8) & 0xFF;
						MyTouchData.yLow[j] = y & 0xFF;;
						MyTouchData.yHigh[j] = (y >> 8) & 0xFF;
						MyTouchData.event = event;
						MyTouchData.finger[j] = id;
					}
			}	
			delay_ms(10);
	 }
	}

