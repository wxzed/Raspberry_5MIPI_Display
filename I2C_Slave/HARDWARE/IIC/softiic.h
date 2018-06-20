#ifndef __SOFT_IIC_H
#define __SOFT_IIC_H
#include "sys.h"	    




  	   		   
//IO方向设置
#define Soft_SDA_IN()  {GPIOA->CRH&=0XFFFFF0FF;GPIOA->CRH|=8<<4*2;}
#define Soft_SDA_OUT() {GPIOA->CRH&=0XFFFFF0FF;GPIOA->CRH|=3<<4*2;}

//IO操作函数	 
#define Soft_IIC_SCL    PAout(11) 			//SCL     
#define Soft_IIC_SDA    PAout(10) 			//SDA	 
#define Soft_READ_SDA   PAin(10)  			//输入SDA 

//IIC所有操作函数
void Soft_IIC_Init(void);                	//初始化IIC的IO口				 
void Soft_IIC_Start(void);				//发送IIC开始信号
void Soft_IIC_Stop(void);	  				//发送IIC停止信号
void Soft_IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 Soft_IIC_Read_Byte(unsigned char ack);	//IIC读取一个字节
u8 Soft_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void Soft_IIC_Ack(void);					//IIC发送ACK信号
void Soft_IIC_NAck(void);					//IIC不发送ACK信号

#endif







