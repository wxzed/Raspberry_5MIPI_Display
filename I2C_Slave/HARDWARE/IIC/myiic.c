#include "myiic.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "stdio.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//IIC驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
typedef enum { true=0, false} bool;
uint16_t  MY_I2C_ADDRESS_TOUCH = 0x70 ;
uint16_t  MY_I2C_ADDRESS_DISPLAY = 0x8A ;
uint8_t ReadTouchWho = 1;
//初始化IIC
bool stopTim = false;
uint8_t freq = 0;

void I2C2_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	// enable APB1 peripheral clock for I2C1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	// enable clock for SCL and SDA pins
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/* setup SCL and SDA pins
	 * You can connect I2C2 to two different
	 * pairs of pins:
	 * 1. SCL on PB10 and SDA on PB11
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//I2C必须开漏输出，实现线与逻辑
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// configure I2C1 
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = MY_I2C_ADDRESS_DISPLAY;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress= I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C2, &I2C_InitStructure);

		//setup interrupts
	I2C_ITConfig(I2C2, I2C_IT_ERR | I2C_IT_EVT | I2C_IT_BUF, ENABLE);

	
	// Configure the I2C event priority
	NVIC_InitStructure.NVIC_IRQChannel                   = I2C2_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// enable I2C1
	I2C_Cmd(I2C2, ENABLE);
}

void I2C1_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	// enable APB1 peripheral clock for I2C1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	// enable clock for SCL and SDA pins
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/* setup SCL and SDA pins
	 * You can connect I2C1 to two different
	 * pairs of pins:
	 * 1. SCL on PB6 and SDA on PB7 
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//I2C必须开漏输出，实现线与逻辑
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// configure I2C1 
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = MY_I2C_ADDRESS_TOUCH;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress= I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &I2C_InitStructure);

		//setup interrupts
	I2C_ITConfig(I2C1, I2C_IT_ERR | I2C_IT_EVT | I2C_IT_BUF, ENABLE);

	
	// Configure the I2C event priority
	NVIC_InitStructure.NVIC_IRQChannel                   = I2C1_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// enable I2C1
	I2C_Cmd(I2C1, ENABLE);
}
//Clear ADDR by reading SR1, then SR2
void I2C_clear_ADDR(I2C_TypeDef* I2Cx) {
	I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR);
	((void)(I2Cx->SR2));
}

//Clear STOPF by reading SR1, then writing CR1
void I2C_clear_STOPF(I2C_TypeDef* I2Cx) {
	I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF);
	I2C_Cmd(I2Cx, ENABLE);
}
uint8_t data = 0;
uint8_t senddate = 0;
/*
static void changAddress(void){
	I2C_clear_ADDR(I2C1);
	if(MY_I2C_ADDRESS == 0x8A){
		MY_I2C_ADDRESS = 0x70;
		I2C1_Init();
	}else{
		MY_I2C_ADDRESS = 0x8A;
		I2C1_Init();
	}
}

static void changIICTouch(void){
	I2C_clear_ADDR(I2C1);
	MY_I2C_ADDRESS = 0x70;
	I2C1_Init();
}
static void changIICDisplay(void){
	I2C_clear_ADDR(I2C1);
	MY_I2C_ADDRESS = 0x8A;
	I2C1_Init();
}*/
static void RestartTouch(void){
	I2C_clear_ADDR(I2C1);
	I2C1_Init();
}

bool sendTouchOK = false;
static void sendDisplay(uint16_t data){
		I2C_clear_ADDR(I2C2);
		I2C_SendData(I2C2, senddate);
}
uint8_t time = 0;
bool first = true;
bool RefreshTouch = false; 
static void sendTouch(){
		I2C_clear_ADDR(I2C1);
		if(sendTouchOK == true){
			sendTouchOK = false;
			I2C_SendData(I2C1,MyTouchData.point);//上报触摸的点数
		}else{
			I2C_SendData(I2C1, 0x80|MyTouchData.xHigh[ReadTouchWho-1]);
		}
}
uint8_t TouorDISFreq = 0;
uint8_t TouchFreq = 0,DISFreq = 0;

void TIM3_IRQHandler(void)  
{ /* 
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
	if(TouchFreq > 20){
		RestartTouch();
		DISFreq = 0;
		TouchFreq = 0;
		BACKLIGHT = 0;
	}else{
		BACKLIGHT = 1;
	}
	TouchFreq++;*/
} 
uint8_t NumTime = 0;
void I2C1_EV_IRQHandler(void) {
		
//        GPIO_SetBits(GPIOD, GREEN); //Show that we got here
		//KV1=0;
		//Clear AF from slave-transmission end
		if(I2C_GetITStatus(I2C1, I2C_IT_AF)) {
			//printf("I2C_GetITStatus\r\n");
			I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
		}
		//Big state machine response, since doesn't actually keep state
		//printf("I2C1_EV_IRQHandler=%d\r\n",I2C_GetLastEvent(I2C1));
		switch(I2C_GetLastEvent(I2C1)) {
			
			//SLAVE
			//Receive
			case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED: //EV1
				//printf("I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED\r\n");
				I2C_clear_ADDR(I2C1);
				break;
			case I2C_EVENT_SLAVE_BYTE_RECEIVED: //EV2
				//printf("I2C_EVENT_SLAVE_BYTE_RECEIVED\r\n");
				//Read it, so no one is waiting, clears BTF if necessary
				data = I2C_ReceiveData(I2C1);
				RefreshTouch = true;
				sendTouchOK = false;
				TouchFreq = 0;
				if(data == 0x02){
					sendTouchOK = true;
				}else if(data == 0x03){//Touch1
					ReadTouchWho = ReadTouch1;
				}else if(data == 0x09){//Touch2
					ReadTouchWho = ReadTouch2;
				}else if(data == 0x0F){//Touch3
					ReadTouchWho = ReadTouch3;
				}else if(data == 0x15){//Touch4
					ReadTouchWho = ReadTouch4;
				}else if(data == 0x1B){//Touch5
					ReadTouchWho = ReadTouch5;
				}else if(data == 0xFF){
					RestartTouch();
				}

				//Do something with it
				/*
				if(I2C_GetFlagStatus(I2C1, I2C_FLAG_DUALF)) {//Secondary Receive
				} else if(I2C_GetFlagStatus(I2C1, I2C_FLAG_GENCALL)) {//General Receive
				} else {//Normal
				}*/
				break;
			case I2C_EVENT_SLAVE_STOP_DETECTED: //End of receive, EV4
				I2C_clear_STOPF(I2C1);
				break;

			//Transmit
			case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED: //EV1
				//printf("I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED\r\n");
				sendTouch();
				break;
			case I2C_EVENT_SLAVE_BYTE_TRANSMITTED: //EV3
				//Determine what you want to send
				//data = 5;
				if(NumTime == 0){
					I2C_SendData(I2C1, MyTouchData.xLow[ReadTouchWho-1]);
					NumTime++;
				}else if(NumTime == 1){
					I2C_SendData(I2C1, (MyTouchData.yHigh[ReadTouchWho-1]&0x0F) | (MyTouchData.finger[ReadTouchWho-1]) << 4);
					NumTime++;
				}else if( NumTime == 2){
					I2C_SendData(I2C1, MyTouchData.yLow[ReadTouchWho-1]);
					NumTime = 0;
				}
				break;
			case I2C_EVENT_SLAVE_ACK_FAILURE://End of transmission EV3_2
				//TODO: Doesn't seem to be getting reached, so just
				//check at top-level
				I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
				break;
			//Alternative Cases for address match
			case I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED: //EV1
				break;
			case I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED: //EV1
				break;
			case I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED: //EV1
				break;


			//MASTER
			case I2C_EVENT_MASTER_MODE_SELECT: //EV5, just sent start bit
				break;
			//Receive
			case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED: //EV6, just sent addr    
				break;
			case I2C_EVENT_MASTER_BYTE_RECEIVED: //EV7
				break;
			//Transmit
			case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: //EV6, just sent addr     
				break;
			case I2C_EVENT_MASTER_BYTE_TRANSMITTING: //EV8, about to send data
				break;
			case I2C_EVENT_MASTER_BYTE_TRANSMITTED: //EV8_2, just sent data
				break;

			//Alternative addressing stuff, not going to worry about
			case I2C_EVENT_MASTER_MODE_ADDRESS10: //EV9
				break;
			default:
				//printf("I2C1_EV_IRQHandler=\r\n");
				//How the FUCK did you get here?
				//I should probably raise some error, but fuck it,
				//it's late
				break;
		}
}

void I2C1_ER_IRQHandler(void) {
 //       GPIO_SetBits(GPIOD, RED);
//	LED3=0;
		//Can't use nice switch statement, because no fxn available
		if(I2C_GetITStatus(I2C1,        I2C_IT_SMBALERT)) {
		} else if(I2C_GetITStatus(I2C1, I2C_IT_TIMEOUT)) {
		} else if(I2C_GetITStatus(I2C1, I2C_IT_PECERR)) {
		} else if(I2C_GetITStatus(I2C1, I2C_IT_OVR)) {
			//Overrun
			//CLK stretch disabled and receiving
			//DR has not been read, b4 next byte comes in
			//effect: lose byte
			//should:clear RxNE and transmitter should retransmit

			//Underrun
			//CLK stretch disabled and I2C transmitting
			//haven't updated DR since new clock
			//effect: same byte resent
			//should: make sure discarded, and write next
		} else if(I2C_GetITStatus(I2C1, I2C_IT_AF)) {
			//Detected NACK
			//Transmitter must reset com
				//Slave: lines released
				//Master: Stop or repeated Start must must be generated
				//Master = MSL bit
			//Fixup
			I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
		} else if(I2C_GetITStatus(I2C1, I2C_IT_ARLO)) {
			//Arbitration Lost
			//Goes to slave mode, but can't ack slave address in same transfer
			//Can after repeat Start though
		} else if(I2C_GetITStatus(I2C1, I2C_IT_BERR)) {
			//Bus Error
			//In slave mode: data discarded, lines released, acts like restart
			//In master mode: current transmission continues
		}
}


void I2C2_EV_IRQHandler(void) {
//        GPIO_SetBits(GPIOD, GREEN); //Show that we got here
		//KV1=0;
		//Clear AF from slave-transmission end
		if(I2C_GetITStatus(I2C2, I2C_IT_AF)) {
			//printf("I2C_GetITStatus\r\n");
			I2C_ClearITPendingBit(I2C2, I2C_IT_AF);
		}
		//Big state machine response, since doesn't actually keep state
		//printf("I2C1_EV_IRQHandler=%d\r\n",I2C_GetLastEvent(I2C1));
		switch(I2C_GetLastEvent(I2C2)) {
			
			//SLAVE
			//Receive
			case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED: //EV1
				//printf("I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED\r\n");
				I2C_clear_ADDR(I2C2);
				break;
			case I2C_EVENT_SLAVE_BYTE_RECEIVED: //EV2
				//printf("I2C_EVENT_SLAVE_BYTE_RECEIVED\r\n");
				//Read it, so no one is waiting, clears BTF if necessary
				data = I2C_ReceiveData(I2C2);
				if(data == 0x01){
					senddate = 0x01;
				}else if(data == 0x80){
					senddate = 0xC3;
				}else if(data == 0x82){
					senddate = 0x85;
				}else if(data == 0x04){
				}else if(data == 0xFF){//打开背光
					BACKLIGHT = 1;
				}else if(data == 0x00){//关闭背光
					BACKLIGHT = 0;
				}
				//Do something with it
				/*
				if(I2C_GetFlagStatus(I2C1, I2C_FLAG_DUALF)) {//Secondary Receive
				} else if(I2C_GetFlagStatus(I2C1, I2C_FLAG_GENCALL)) {//General Receive
				} else {//Normal
				}*/
				break;
			case I2C_EVENT_SLAVE_STOP_DETECTED: //End of receive, EV4
				I2C_clear_STOPF(I2C2);
				break;

			//Transmit
			case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED: //EV1
				//printf("I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED\r\n");
				sendDisplay(senddate);
				break;
			case I2C_EVENT_SLAVE_BYTE_TRANSMITTED: //EV3
				//Determine what you want to send
				//data = 5;
			/*
				if(I2C_GetFlagStatus(I2C1, I2C_FLAG_DUALF)) {//Secondary Transmit
				} else if(I2C_GetFlagStatus(I2C1, I2C_FLAG_GENCALL)) {//General Transmit
				} else {//Normal
				}
				I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF);*/
				break;
			case I2C_EVENT_SLAVE_ACK_FAILURE://End of transmission EV3_2
				//TODO: Doesn't seem to be getting reached, so just
				//check at top-level
				I2C_ClearITPendingBit(I2C2, I2C_IT_AF);
				break;
			//Alternative Cases for address match
			case I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED: //EV1
				break;
			case I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED: //EV1
				break;
			case I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED: //EV1
				break;


			//MASTER
			case I2C_EVENT_MASTER_MODE_SELECT: //EV5, just sent start bit
				break;
			//Receive
			case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED: //EV6, just sent addr    
				break;
			case I2C_EVENT_MASTER_BYTE_RECEIVED: //EV7
				break;
			//Transmit
			case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: //EV6, just sent addr     
				break;
			case I2C_EVENT_MASTER_BYTE_TRANSMITTING: //EV8, about to send data
				break;
			case I2C_EVENT_MASTER_BYTE_TRANSMITTED: //EV8_2, just sent data
				break;

			//Alternative addressing stuff, not going to worry about
			case I2C_EVENT_MASTER_MODE_ADDRESS10: //EV9
				break;
			default:
				//printf("I2C1_EV_IRQHandler=\r\n");
				//How the FUCK did you get here?
				//I should probably raise some error, but fuck it,
				//it's late
				break;
		}
}

void I2C2_ER_IRQHandler(void) {
 //       GPIO_SetBits(GPIOD, RED);
//	LED3=0;
		//Can't use nice switch statement, because no fxn available
		if(I2C_GetITStatus(I2C2,        I2C_IT_SMBALERT)) {
		} else if(I2C_GetITStatus(I2C2, I2C_IT_TIMEOUT)) {
		} else if(I2C_GetITStatus(I2C2, I2C_IT_PECERR)) {
		} else if(I2C_GetITStatus(I2C2, I2C_IT_OVR)) {
			//Overrun
			//CLK stretch disabled and receiving
			//DR has not been read, b4 next byte comes in
			//effect: lose byte
			//should:clear RxNE and transmitter should retransmit

			//Underrun
			//CLK stretch disabled and I2C transmitting
			//haven't updated DR since new clock
			//effect: same byte resent
			//should: make sure discarded, and write next
		} else if(I2C_GetITStatus(I2C2, I2C_IT_AF)) {
			//Detected NACK
			//Transmitter must reset com
				//Slave: lines released
				//Master: Stop or repeated Start must must be generated
				//Master = MSL bit
			//Fixup
			I2C_ClearITPendingBit(I2C2, I2C_IT_AF);
		} else if(I2C_GetITStatus(I2C2, I2C_IT_ARLO)) {
			//Arbitration Lost
			//Goes to slave mode, but can't ack slave address in same transfer
			//Can after repeat Start though
		} else if(I2C_GetITStatus(I2C2, I2C_IT_BERR)) {
			//Bus Error
			//In slave mode: data discarded, lines released, acts like restart
			//In master mode: current transmission continues
		}
}






















