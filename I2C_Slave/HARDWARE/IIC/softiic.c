#include "softiic.h"
#include "delay.h"	 


//控制I2C速度的延时
void Soft_Delay(void)
{
	delay_us(4);
} 
//电容触摸芯片IIC接口初始化
#define Soft_SDA_Pin    GPIO_Pin_10
#define Soft_SCL_Pin    GPIO_Pin_11
void Soft_IIC_Init(void)
{		
/*	
 	RCC->APB2ENR|=1<<3;		//先使能外设IO PORTB时钟    
	GPIOB->CRH&=0XFFFF00FF;	//PB10,PB11 推挽输出
	GPIOB->CRH|=0X00003300;
 GPIOB->ODR|=0<<11;	    //PB11 输出高	 	
	GPIOB->ODR|=0<<10;	    //PB10 输出高	*/ 
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = Soft_SDA_Pin | Soft_SCL_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,Soft_SDA_Pin|Soft_SCL_Pin);
}
//产生IIC起始信号
void Soft_IIC_Start(void)
{
	Soft_SDA_OUT();     //sda线输出
	Soft_IIC_SDA=1;	  	  
	Soft_IIC_SCL=1;
	Soft_Delay();
 	Soft_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	Soft_Delay();
	Soft_IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void Soft_IIC_Stop(void)
{ 
	Soft_SDA_OUT();//sda线输出
	Soft_IIC_SCL=0;
	Soft_IIC_SDA=0;
	Soft_Delay();
	Soft_IIC_SCL=1;
	Soft_Delay();
	Soft_IIC_SDA=1;//STOP:when CLK is high DATA change form low to high 
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 Soft_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	Soft_SDA_IN();      //SDA设置为输入  
	Soft_IIC_SDA=1;delay_us(1);	   
	Soft_IIC_SCL=1;delay_us(1);	 
	while(Soft_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			Soft_IIC_Stop();
			return 1;
		} 
	}
	Soft_IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void Soft_IIC_Ack(void)
{
	Soft_IIC_SCL=0;
	Soft_SDA_OUT();
	Soft_IIC_SDA=0;
	Soft_Delay();
	Soft_IIC_SCL=1;
	Soft_Delay();
	Soft_IIC_SCL=0;
}
//不产生ACK应答		    
void Soft_IIC_NAck(void)
{
	Soft_IIC_SCL=0;
	Soft_SDA_OUT();
	Soft_IIC_SDA=1;
	Soft_Delay();
	Soft_IIC_SCL=1;
	Soft_Delay();
	Soft_IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void Soft_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	Soft_SDA_OUT(); 	    
    Soft_IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        Soft_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	      
		Soft_IIC_SCL=1;
		Soft_Delay();
		Soft_IIC_SCL=0;	
		Soft_Delay();
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 Soft_IIC_Read_Byte(unsigned char ack)
{
	u8 i,receive=0;
 	Soft_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        Soft_IIC_SCL=0; 	    	   
		delay_us(30);
		Soft_IIC_SCL=1;  
		receive<<=1;
		if(Soft_READ_SDA)receive++;   
	}	  				 
	if (!ack)Soft_IIC_NAck();//发送nACK
	else Soft_IIC_Ack(); //发送ACK   
 	return receive;
}




























